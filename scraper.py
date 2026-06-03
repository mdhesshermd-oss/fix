import discord
import asyncio
import re
import sys
from database import add_scraped_users, add_token

def extract_invite_code(link: str) -> str:
    """Extracts invite code from a discord invite link."""
    # Pattern to match discord.gg/abc, discord.com/invite/abc, etc.
    match = re.search(r"(?:discord\.gg/|discord\.com/invite/|discordapp\.com/invite/)([a-zA-Z0-9\-]+)", link)
    if match:
        return match.group(1)
    return link # Return original if it doesn't match, maybe it's already just the code

class ScrapeClient(discord.Client):
    def __init__(self, target_input: str, *args, **kwargs):
        super().__init__(chunk_guilds_at_startup=True, *args, **kwargs)
        self.target_input = target_input
        self.scraped_users = []
        self.error_message = None

    async def on_ready(self):
        print(f"[{self.user}] Подключение успешно установлено.")
        
        # Save token details to DB
        add_token(self.http.token, str(self.user))
        
        target_guild = None
        invite_code = extract_invite_code(self.target_input)
        
        # Check if target is a guild ID or invite
        if invite_code.isdigit():
            guild_id = int(invite_code)
            target_guild = self.get_guild(guild_id)
            if not target_guild:
                print(f"Бот не состоит в сервере с ID {guild_id}. Попытка присоединиться невозможна без инвайта.")
                self.error_message = "Not in guild and no invite link provided"
                await self.close()
                return
        else:
            print(f"Попытка использовать код приглашения: {invite_code}")
            try:
                # In discord.py-self, users can accept invites
                invite = await self.fetch_invite(invite_code)
                target_guild = self.get_guild(invite.guild.id)
                if not target_guild:
                    print(f"Вступаем на сервер: {invite.guild.name}")
                    # Accept invite
                    await invite.accept()
                    # Wait a bit to ensure Discord registers membership
                    await asyncio.sleep(3)
                    target_guild = self.get_guild(invite.guild.id)
            except Exception as e:
                print(f"Ошибка при обработке инвайта: {e}")
                self.error_message = f"Failed to process invite: {e}"
                await self.close()
                return
        
        if not target_guild:
            self.error_message = "Could not find or join the guild."
            await self.close()
            return
            
        print(f"Сервер найден: {target_guild.name} (ID: {target_guild.id})")

        # Stealthy member collection: Subscribe to a channel first (Lazy Loading simulation)
        for channel in target_guild.text_channels:
            if channel.permissions_for(target_guild.me).read_messages:
                print(f"Имитация просмотра канала: {channel.name}")
                try:
                    await channel.subscribe()
                    await asyncio.sleep(2)
                    break
                except:
                    continue

        print("Запрашиваем участников через поиск (имитация действий человека)...")
        
        # Comprehensive search: letters, numbers, and common symbols
        import string
        import random
        search_queries = list(string.ascii_lowercase + string.digits + " ._!@#$%^&*()+-=")
        random.shuffle(search_queries) # Shuffle to look less predictable

        for q in search_queries:
            try:
                print(f"Поиск участников на '{q}'...")
                await target_guild.query_members(query=q, limit=100, cache=True)
                # Wait between queries to look human
                await asyncio.sleep(random.uniform(1.0, 2.5))
                if len(target_guild.members) > 1000: # Stop if we have enough
                    break
            except Exception as e:
                print(f"Ошибка при поиске '{q}': {e}")
                continue

        members = target_guild.members
        print(f"Всего обнаружено участников в кэше: {len(members)}")
        
        scraped_data = []
        for m in members:
            # Skip bots and self
            if m.bot or m.id == self.user.id:
                continue
                
            # Check for administrator/moderator permissions to skip them
            is_admin = 0
            if m.guild_permissions.administrator:
                is_admin = 1
            elif m.guild_permissions.kick_members or m.guild_permissions.ban_members or m.guild_permissions.manage_guild:
                is_admin = 1
                
            scraped_data.append({
                'user_id': str(m.id),
                'username': m.name,
                'display_name': m.display_name,
                'is_admin': is_admin,
                'guild_id': str(target_guild.id),
                'guild_name': target_guild.name
            })
            
        if scraped_data:
            added = add_scraped_users(scraped_data)
            print(f"Успешно сохранено/обновлено {added} обычных пользователей в базу данных.")
            self.scraped_users = scraped_data
        else:
            print("Не удалось собрать участников. Возможно, у аккаунта нет прав на просмотр списка участников.")
            self.error_message = "No members scraped"
            
        await self.close()

def run_scraper(token: str, target: str):
    """Runs the scraper for a single token and target server."""
    client = ScrapeClient(target_input=target)
    try:
        # Use asyncio to run the client
        asyncio.run(client.start(token))
    except Exception as e:
        print(f"Ошибка при запуске селф-бота: {e}")
        return False, str(e)
    return len(client.scraped_users) > 0, client.error_message

if __name__ == "__main__":
    if len(sys.argv) < 3:
        print("Использование: python scraper.py <TOKEN> <INVITE_LINK_OR_GUILD_ID>")
        sys.exit(1)
        
    token = sys.argv[1]
    target = sys.argv[2]
    
    success, err = run_scraper(token, target)
    if success:
        print("Сбор участников успешно завершен.")
    else:
        print(f"Сбор участников завершился ошибкой: {err}")
