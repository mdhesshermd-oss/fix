import asyncio
import random
import time
from sender import MultiTokenSender
from database import get_active_tokens, init_db, get_users_to_friend
from paraphraser import load_config
try:
    from automation import check_for_captcha, solve_interaction
except ImportError:
    print("[!] Модуль GUI-автоматизации (pyautogui) не найден. Работаем в обычном режиме.")
    check_for_captcha = lambda: None
    solve_interaction = lambda: None

async def run_mass_mailing():
    """Главный координатор массовой рассылки."""
    init_db()
    tokens = get_active_tokens()
    if not tokens:
        print("[!] Нет активных токенов в базе.")
        return

    print(f"[СИСТЕМА] Запуск МАССОВОЙ рассылки для {len(tokens)} аккаунтов.")

    # Инициализация клиентов
    clients = [MultiTokenSender(t) for t in tokens]

    # Запуск всех клиентов
    tasks = [asyncio.create_task(cl.start(cl.token_str)) for cl in clients]

    # Мониторинг капчи и процесса
    while True:
        # 1. Визуальная проверка на капчу через экран (каждые 10 секунд)
        if check_for_captcha():
            print("[ALARM] ВНИМАНИЕ! Капча обнаружена визуально. Перехват управления...")
            solve_interaction()
            # Даем время на обновление
            await asyncio.sleep(10)

        await asyncio.sleep(10)

        # Проверяем, живы ли задачи
        if all(t.done() for t in tasks):
            break

if __name__ == "__main__":
    try:
        asyncio.run(run_mass_mailing())
    except KeyboardInterrupt:
        print("\n[!] Рассылка принудительно остановлена.")
