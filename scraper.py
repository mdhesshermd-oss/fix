import discord
import asyncio
import re
import sys
import string
import random
from database import add_scraped_users, add_token

def extract_invite_code(link: str) -> str:
    """Извлекает код приглашения из ссылки Discord."""
    match = re.search(r"(?:discord\.gg/|discord\.com/invite/|discordapp\.com/invite/)([a-zA-Z0-9\-]+)", link)
    if match:
        return match.group(1)
    return link

class ScrapeClient(discord.Client):
    def __init__(self, target_input: str, *args, **kwargs):
        # Настройка чанков для селф-ботов
        super().__init__(chunk_guilds_at_startup=True, *args, **kwargs)
        self.target_input = target_input
        self.scraped_users = []
        self.error_message = None

    async def on_ready(self):
        print(f"\n[АВТОРИЗАЦИЯ] Вход выполнен как: {self.user}")

        # Сохраняем/обновляем данные токена в БД
        add_token(self.http.token, str(self.user))

        target_guild = None
        invite_code = extract_invite_code(self.target_input)

        # Проверка: ID сервера или инвайт-ссылка
        if invite_code.isdigit():
            guild_id = int(invite_code)
            target_guild = self.get_guild(guild_id)
            if not target_guild:
                self.error_message = f"Аккаунт не состоит в сервере с ID {guild_id}. Нужна инвайт-ссылка."
                await self.close()
                return
        else:
            print(f"[ИНФО] Обработка инвайта: {invite_code}")
            try:
                invite = await self.fetch_invite(invite_code)
                target_guild = self.get_guild(invite.guild.id)
                if not target_guild:
                    print(f"[ВСТУПЛЕНИЕ] Заходим на сервер: {invite.guild.name}")
                    await invite.accept()
                    await asyncio.sleep(5) # Пауза после входа
                    target_guild = self.get_guild(invite.guild.id)
            except Exception as e:
                self.error_message = f"Ошибка инвайта: {e}"
                await self.close()
                return

        if not target_guild:
            self.error_message = "Не удалось найти или вступить на сервер."
            await self.close()
            return

        print(f"[УСПЕХ] Цель: {target_guild.name} (Участников в API: {target_guild.member_count})")

        # Имитация активности: "просмотр" канала перед парсингом
        for channel in target_guild.text_channels:
            if channel.permissions_for(target_guild.me).read_messages:
                try:
                    await channel.subscribe()
                    print(f"[STEALTH] Имитация просмотра канала #{channel.name}")
                    await asyncio.sleep(3)
                    break
                except:
                    continue

        print("\n[ПРОЦЕСС] Начинаем ГЛУБОКИЙ сбор участников (Deep Scan 2-char)...")
        print("[!] Это может занять 15-30 минут для серверов >10к участников.")

        # Алгоритм глубокого поиска: комбинации из 2 символов (aa, ab... z9)
        # Это обходит лимит Discord в 100 результатов на один запрос поиска
        chars = string.ascii_lowercase + string.digits
        search_queries = [a + b for a in chars for b in chars]
        random.shuffle(search_queries) # Перемешиваем запросы для скрытности

        total_queries = len(search_queries)

        for idx, q in enumerate(search_queries):
            try:
                # Каждые 50 запросов выводим статус
                if idx % 50 == 0:
                    current_count = len(target_guild.members)
                    print(f" -> Прогресс: {idx}/{total_queries} запросов | Собрано в кэш: {current_count}")

                # Запрашиваем участников через поиск
                await target_guild.query_members(query=q, limit=100, cache=True)

                # Случайная пауза между запросами (крайне важно для защиты от бана)
                await asyncio.sleep(random.uniform(0.8, 1.5))

                # Длинная пауза каждые 150 запросов
                if idx % 150 == 0 and idx > 0:
                    print("[STEALTH] Длинная пауза для имитации поведения человека...")
                    await asyncio.sleep(random.uniform(10, 20))

            except discord.HTTPException as e:
                if e.status == 429:
                    print("[WARNING] Rate Limit (429)! Ожидание 60 секунд...")
                    await asyncio.sleep(60)
                else:
                    print(f"[!] Ошибка API на запросе '{q}': {e}")
            except Exception as e:
                print(f"[!] Ошибка: {e}")
                continue

        # Обработка собранных данных
        all_members = target_guild.members
        print(f"\n[ЗАВЕРШЕНО] Сбор окончен. Всего уникальных участников в кэше: {len(all_members)}")

        scraped_data = []
        for m in all_members:
            if m.bot or m.id == self.user.id:
                continue

            # Проверка на админа/модератора (чтобы не слать им спам)
            is_admin = 0
            if m.guild_permissions.administrator or m.guild_permissions.kick_members or m.guild_permissions.manage_guild:
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
            print(f"[БАЗА ДАННЫХ] Успешно сохранено/обновлено: {added} пользователей.")
            self.scraped_users = scraped_data
        else:
            self.error_message = "Не удалось извлечь данные участников из кэша."

        await self.close()

def run_scraper(token: str, target: str):
    """Запускает процесс парсинга."""
    client = ScrapeClient(target_input=target)
    try:
        asyncio.run(client.start(token))
    except Exception as e:
        print(f"[КРИТИЧЕСКАЯ ОШИБКА] {e}")
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
        print("\nПроцесс парсинга полностью завершен.")
    else:
        print(f"\nПарсинг не удался: {err}")