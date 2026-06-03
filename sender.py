import discord
import asyncio
import random
import os
import json
import sys
from database import (
    get_active_tokens,
    update_token_status,
    get_users_to_friend,
    update_friend_status,
    update_message_status,
    init_db
)
from paraphraser import paraphrase_message, load_config

# Global lock to serialize AI requests across multiple accounts and prevent 429 Rate Limits
ai_lock = asyncio.Lock()

class MultiTokenSender(discord.Client):
    def __init__(self, token_info, *args, **kwargs):
        super().__init__(*args, **kwargs)
        self.token_id = token_info["id"]
        self.token_str = token_info["token"]
        self.username = token_info["username"]
        self.proxy = token_info.get("proxy")
        self.friend_loop_task = None
        self.is_running = True

    async def on_ready(self):
        print(f"[Аккаунт {self.user}] Успешно авторизован и онлайн.")

        # Set a random presence to look human
        activities = [
            discord.Game(name="DayZ"),
            discord.Activity(type=discord.ActivityType.watching, name="YouTube"),
            discord.Streaming(name="Survival", url="http://twitch.tv/directory"),
            None
        ]
        activity = random.choice(activities)
        status = random.choice([discord.Status.online, discord.Status.idle])
        await self.change_presence(status=status, activity=activity)

        # Start the background tasks
        self.friend_loop_task = self.loop.create_task(self.friend_request_loop())
        # Check existing friends for pending messages
        self.loop.create_task(self.check_existing_friends_for_messages())

    async def on_relationship_add(self, relationship):
        """Triggered when a relationship is added or updated (e.g. friend request accepted)."""
        if relationship.type == discord.RelationshipType.friend:
            user = relationship.user
            print(f"[Аккаунт {self.user}] Пользователь {user.name} (ID: {user.id}) принял запрос в друзья!")
            
            # Message the user with a slight delay to seem human
            self.loop.create_task(self.send_paraphrased_dm_with_delay(user))

    async def check_existing_friends_for_messages(self):
        """Checks current friends list at startup to see if anyone accepted while offline."""
        await self.wait_until_ready()
        print(f"[Аккаунт {self.user}] Проверка существующего списка друзей на наличие недоставленных сообщений...")
        
        # Access database users
        import sqlite3
        from database import DB_PATH
        
        conn = sqlite3.connect(DB_PATH)
        cursor = conn.cursor()
        
        # Get all users who are currently marked as 'none' or 'request_sent' in friend_status
        # but are actually in our friends list now. (And who are NOT marked as 'skipped'!)
        cursor.execute("SELECT user_id FROM users WHERE message_status = 'none' AND friend_status != 'skipped' AND is_admin = 0")
        unmessaged_ids = {row[0] for row in cursor.fetchall()}
        conn.close()
        
        for friend in self.friends:
            friend_user = friend.user
            friend_id_str = str(friend_user.id)
            if friend_id_str in unmessaged_ids:
                print(f"[Аккаунт {self.user}] Обнаружен друг {friend_user.name} (ID: {friend_id_str}), которому еще не отправлено сообщение. Отправляем...")
                # Update status and send message
                update_friend_status(friend_id_str, "accepted")
                self.loop.create_task(self.send_paraphrased_dm_with_delay(friend_user))

    async def send_paraphrased_dm_with_delay(self, user):
        """Paraphrases the base message and sends it to the user after a brief delay."""
        config = load_config()
        min_del = config.get("delay_message_min_seconds", 60)
        max_del = config.get("delay_message_max_seconds", 180)
        base_msg = config.get("base_message", "Привет!")
        
        delay = random.randint(min_del, max_del)
        print(f"[Аккаунт {self.user}] Планирование отправки ЛС пользователю {user.name} через {delay} сек...")
        await asyncio.sleep(delay)
        
        try:
            # Acquire global lock to query the AI sequentially, preventing rate-limiting on free providers
            async with ai_lock:
                unique_message = paraphrase_message(base_msg)
                # Keep lock acquired for 4 seconds to let the free provider breathe
                await asyncio.sleep(4)
            
            # Emulate typing
            async with user.typing():
                # typing speed: ~180 chars per minute
                typing_duration = len(unique_message) / 3
                await asyncio.sleep(min(typing_duration, 12)) # Max 12 seconds typing

            # Send DM
            await user.send(unique_message)
            print(f"[Аккаунт {self.user}] [УСПЕХ] Сообщение отправлено пользователю {user.name} (ID: {user.id})")
            
            update_friend_status(str(user.id), "accepted")
            update_message_status(str(user.id), "sent")
        except discord.Forbidden:
            print(f"[Аккаунт {self.user}] [ОШИБКА] Закрыто ЛС или заблокировано для {user.name} (ID: {user.id})")
            update_message_status(str(user.id), "failed")
        except Exception as e:
            print(f"[Аккаунт {self.user}] [ОШИБКА] Не удалось отправить ЛС пользователю {user.name}: {e}")
            update_message_status(str(user.id), "failed")

    async def friend_request_loop(self):
        """Loop to periodically send friend requests to scraped users."""
        await self.wait_until_ready()
        
        while self.is_running:
            config = load_config()
            min_delay = config.get("delay_friend_request_min_seconds", 120)
            max_delay = config.get("delay_friend_request_max_seconds", 300)
            
            # Fetch one user from DB to send friend request to
            users = get_users_to_friend(limit=1)
            if not users:
                print(f"[Аккаунт {self.user}] Нет новых пользователей для отправки запросов в друзья. Ожидание 60 секунд...")
                await asyncio.sleep(60)
                continue
                
            target_user = users[0]
            target_id = int(target_user["user_id"])
            target_name = target_user["username"]
            
            print(f"[Аккаунт {self.user}] Отправка запроса в друзья пользователю {target_name} (ID: {target_id})...")
            
            try:
                # Mark as request_sent first to avoid other accounts double-requesting instantly
                update_friend_status(str(target_id), "request_sent")
                
                # Fetch user object and send request
                user = await self.fetch_user(target_id)
                # Modern Discord requires 'location' context for friend requests from user accounts
                await user.send_friend_request(location="Guild Member List")
                print(f"[Аккаунт {self.user}] [УСПЕХ] Запрос в друзья отправлен {target_name}.")
                
            except discord.HTTPException as e:
                if e.status == 429: # Rate limit
                    print(f"[Аккаунт {self.user}] [WARNING] Превышен лимит запросов (Rate Limit). Переключаемся в режим ожидания.")
                    update_friend_status(str(target_id), "none") # Revert status
                    await asyncio.sleep(300) # Wait 5 minutes
                elif e.code == 40002: # Verified account required, etc.
                    print(f"[Аккаунт {self.user}] [ОШИБКА] Требуется верификация аккаунта. Токен отключен.")
                    update_token_status(self.token_id, "rate_limited")
                    self.is_running = False
                    await self.close()
                    break
                else:
                    print(f"[Аккаунт {self.user}] [ОШИБКА] Ошибка Discord API при отправке запроса: {e}")
                    update_friend_status(str(target_id), "failed")
            except Exception as e:
                print(f"[Аккаунт {self.user}] [ОШИБКА] Не удалось отправить запрос в друзья {target_name}: {e}")
                update_friend_status(str(target_id), "failed")
                
            # Sleep with random delay to look human
            sleep_time = random.randint(min_delay, max_delay)
            print(f"[Аккаунт {self.user}] Ожидание {sleep_time} секунд перед следующим запросом...")
            await asyncio.sleep(sleep_time)

    async def close(self):
        self.is_running = False
        if self.friend_loop_task:
            self.friend_loop_task.cancel()
        await super().close()

async def start_all_senders():
    """Starts all active accounts in parallel."""
    init_db()
    tokens = get_active_tokens()
    
    if not tokens:
        print("[Ошибка] Нет активных токенов в базе данных! Сначала добавьте токены.")
        return
        
    print(f"Запуск рассылки для {len(tokens)} аккаунтов...")
    
    clients = []
    for t in tokens:
        client = MultiTokenSender(t)
        clients.append(client)
        
    # Start all clients tasks
    tasks = []
    for client in clients:
        # We define a startup wrapper to handle invalid tokens without crashing everything
        async def run_client(cl=client):
            try:
                # Use proxy if available
                proxy_url = cl.proxy if cl.proxy else None
                await cl.start(cl.token_str, proxy=proxy_url)
            except discord.LoginFailure:
                print(f"[Ошибка] Неверный токен для аккаунта ID {cl.token_id} (Username: {cl.username}). Токен помечен как недействительный.")
                update_token_status(cl.token_id, "invalid")
            except Exception as e:
                print(f"[Ошибка] Критическая ошибка работы аккаунта ID {cl.token_id}: {e}")
                update_token_status(cl.token_id, "rate_limited")
        
        tasks.append(run_client())
        
    await asyncio.gather(*tasks)

if __name__ == "__main__":
    try:
        asyncio.run(start_all_senders())
    except KeyboardInterrupt:
        print("\nРассылка остановлена пользователем.")
