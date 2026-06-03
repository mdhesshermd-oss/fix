import discord
import asyncio
import random
import os
import json
import sys
import sqlite3
from database import (
    get_active_tokens,
    update_token_status,
    get_users_to_friend,
    update_friend_status,
    update_message_status,
    init_db,
    DB_PATH
)
from paraphraser import paraphrase_message, load_config

# Глобальная блокировка для AI запросов
ai_lock = asyncio.Lock()

class MultiTokenSender(discord.Client):
    def __init__(self, token_info, *args, **kwargs):
        # Прокси полностью удалены из инициализации
        super().__init__(*args, **kwargs)

        self.token_id = token_info["id"]
        self.token_str = token_info["token"]
        self.username = token_info["username"]
        self.friend_loop_task = None
        self.is_running = True

    async def on_ready(self):
        print(f"\n[СИСТЕМА] Аккаунт @{self.user} онлайн (Локальный IP)")

        # Рандомная активность
        activities = [
            discord.Game(name="DayZ"),
            discord.Activity(type=discord.ActivityType.watching, name="YouTube"),
            discord.Streaming(name="Survival", url="http://twitch.tv/directory"),
            None
        ]
        await self.change_presence(
            status=random.choice([discord.Status.online, discord.Status.idle]),
            activity=random.choice(activities)
        )

        # Запуск задач
        self.friend_loop_task = self.loop.create_task(self.friend_request_loop())
        self.loop.create_task(self.check_existing_friends_for_messages())

    async def on_relationship_add(self, relationship):
        """Срабатывает, когда кто-то принимает дружбу."""
        if relationship.type == discord.RelationshipType.friend:
            user = relationship.user
            print(f"[ДРУЖБА] {user.name} принял запрос. Аккаунт: {self.user}")
            self.loop.create_task(self.send_paraphrased_dm_with_delay(user))

    async def check_existing_friends_for_messages(self):
        """Проверка тех, кто принял дружбу, пока бот был оффлайн."""
        await self.wait_until_ready()
        try:
            conn = sqlite3.connect(DB_PATH)
            cursor = conn.cursor()
            cursor.execute("SELECT user_id FROM users WHERE message_status = 'none' AND friend_status != 'skipped' AND is_admin = 0")
            unmessaged_ids = {row[0] for row in cursor.fetchall()}
            conn.close()

            for friend in self.friends:
                f_user = friend.user
                if str(f_user.id) in unmessaged_ids:
                    print(f"[ИНФО] Найден старый друг без сообщения: {f_user.name}")
                    update_friend_status(str(f_user.id), "accepted")
                    self.loop.create_task(self.send_paraphrased_dm_with_delay(f_user))
        except Exception as e:
            print(f"[ОШИБКА] Ошибка синхронизации списка друзей: {e}")

    async def send_paraphrased_dm_with_delay(self, user):
        """Процесс отправки сообщения."""
        config = load_config()
        base_msg = config.get("base_message", "Привет!")
        
        # 1. Большая задержка для имитации "человек увидел уведомление и зашел"
        delay = random.randint(
            config.get("delay_message_min_seconds", 60),
            config.get("delay_message_max_seconds", 180)
        )
        print(f"[ТАЙМЕР] Отправка ЛС для {user.name} через {delay} сек...")
        await asyncio.sleep(delay)
        
        try:
            # 2. ИИ Перефразирование
            async with ai_lock:
                print(f"[AI] Генерируем текст для {user.name}...")
                unique_message = paraphrase_message(base_msg)
                await asyncio.sleep(4) # Увеличили паузу для ИИ
            
            # 3. Эмуляция печатания с джиттером
            async with user.typing():
                # Имитируем разную скорость печати + рандомный джиттер
                typing_time = max(5, min(len(unique_message) / 4.0, 15)) + random.uniform(1.0, 3.0)
                await asyncio.sleep(typing_time)

            # 4. Отправка
            await user.send(unique_message)
            print(f"[УСПЕХ] Сообщение доставлено: {user.name} (@{self.user})")
            
            update_friend_status(str(user.id), "accepted")
            update_message_status(str(user.id), "sent")

        except discord.HTTPException as e:
            err_msg = str(e).lower()
            if "captcha" in err_msg or e.status == 403:
                print(f"[КРИТ] Капча при отправке ЛС {user.name}. ПАУЗА 1 ЧАС.")
                await asyncio.sleep(3600)
            else:
                print(f"[ОШИБКА] Сбой Discord API при ЛС {user.name}: {e}")
            update_message_status(str(user.id), "failed")
        except Exception as e:
            print(f"[ОШИБКА] Не удалось отправить ЛС {user.name}: {e}")
            update_message_status(str(user.id), "failed")

    async def friend_request_loop(self):
        """Цикл рассылки заявок в друзья."""
        await self.wait_until_ready()
        
        while self.is_running:
            config = load_config()
            users = get_users_to_friend(limit=1)

            if not users:
                print(f"[СОН] Аккаунт @{self.user} ждет новых целей...")
                await asyncio.sleep(60)
                continue
                
            u = users[0]
            print(f"[ЗАЯВКА] @{self.user} -> {u['username']}")
            
            try:
                update_friend_status(str(u['user_id']), "request_sent")
                user_obj = await self.fetch_user(int(u['user_id']))
                # Имитируем запрос из списка участников сервера
                await user_obj.send_friend_request(location="Guild Member List")
                print(f"[ЗАЯВКА] Успешно отправлена к {u['username']}.")
            except discord.HTTPException as e:
                err_msg = str(e).lower()
                if e.status == 429:
                    print(f"[LIMIT] Сработал Rate Limit Discord. Пауза 10 минут.")
                    update_friend_status(str(u['user_id']), "none")
                    await asyncio.sleep(600)
                elif "captcha" in err_msg or e.status == 403 or "unauthorized" in err_msg:
                    print(f"[КРИТ] Обнаружена капча или блокировка (403). ПАУЗА 1 ЧАС для безопасности.")
                    update_friend_status(str(u['user_id']), "none")
                    await asyncio.sleep(3600) # Safety stop for 1 hour
                else:
                    print(f"[ОШИБКА] Ошибка API Discord: {e}")
                    update_friend_status(str(u['user_id']), "failed")
            except Exception as e:
                print(f"[ОШИБКА] Сбой: {e}")
                update_friend_status(str(u['user_id']), "failed")
                
            # Пауза между заявками (ВАЖНО не ставить слишком мало)
            wait = random.randint(
                config.get("delay_friend_request_min_seconds", 120),
                config.get("delay_friend_request_max_seconds", 300)
            )
            print(f"[ПАУЗА] @{self.user} отдыхает {wait} сек...")
            await asyncio.sleep(wait)

    async def close(self):
        self.is_running = False
        if self.friend_loop_task:
            self.friend_loop_task.cancel()
        await super().close()

async def start_all_senders():
    """Запуск всех аккаунтов на прямом IP."""
    init_db()
    tokens = get_active_tokens()
    
    if not tokens:
        print("[!] Активных токенов нет. Проверьте базу или запустите fix_tokens.py")
        return
        
    print(f"[СИСТЕМА] Запуск рассылки без прокси для {len(tokens)} аккаунтов...")
    
    tasks = []
    for t_info in tokens:
        client = MultiTokenSender(t_info)

        async def run_client(cl=client):
            try:
                # Прямое подключение без прокси и лишних аргументов
                await cl.start(cl.token_str)
            except discord.LoginFailure:
                print(f"[КРИТ] Неверный токен (ID:{cl.token_id})")
                update_token_status(cl.token_id, "invalid")
            except Exception as e:
                print(f"[КРИТ] Ошибка работы (ID:{cl.token_id}): {e}")
                update_token_status(cl.token_id, "rate_limited")
        
        tasks.append(run_client())
        
    await asyncio.gather(*tasks)

if __name__ == "__main__":
    try:
        asyncio.run(start_all_senders())
    except KeyboardInterrupt:
        print("\nРассылка остановлена пользователем.")