import os
import json
import asyncio
import sys
from database import init_db, add_token, get_stats, get_active_tokens
from scraper import run_scraper
from sender import start_all_senders

CONFIG_PATH = os.path.join(os.path.dirname(os.path.abspath(__file__)), "config.json")

def load_config():
    """Loads config.json safely."""
    if not os.path.exists(CONFIG_PATH):
        default_config = {
            "gemini_api_key": "YOUR_GEMINI_API_KEY_HERE",
            "delay_friend_request_min_seconds": 120,
            "delay_friend_request_max_seconds": 300,
            "delay_message_min_seconds": 60,
            "delay_message_max_seconds": 180,
            "base_message": "Привет! Увидел тебя на сервере. Хотел поделиться классным материалом: https://example.com"
        }
        save_config(default_config)
        return default_config
    try:
        with open(CONFIG_PATH, "r", encoding="utf-8") as f:
            return json.load(f)
    except Exception as e:
        print(f"Ошибка загрузки конфигурации: {e}")
        return {}

def save_config(config):
    """Saves configuration to config.json."""
    try:
        with open(CONFIG_PATH, "w", encoding="utf-8") as f:
            json.dump(config, f, indent=2, ensure_ascii=False)
        print("Конфигурация успешно сохранена.")
    except Exception as e:
        print(f"Ошибка сохранения конфигурации: {e}")

def show_stats_menu():
    """Displays current DB statistics."""
    stats = get_stats()
    print("\n================ СТАТИСТИКА ================")
    print(f"Всего добавленных токенов:   {stats['total_tokens']}")
    print(f"Активных (рабочих) токенов:  {stats['active_tokens']}")
    print(f"Всего спарсенных участников: {stats['total_users']}")
    print(f"  - Ожидают заявку в друзья: {stats['friend_none']}")
    print(f"  - Заявка в друзья отправлена: {stats['friend_sent']}")
    print(f"  - Заявка принята (друзья):   {stats['friend_accepted']}")
    print(f"  - Ошибка отправки заявки:    {stats['friend_failed']}")
    print(f"Всего отправлено ЛС:        {stats['messages_sent']}")
    print(f"Ошибок отправки ЛС:         {stats['messages_failed']}")
    print("=============================================")

def add_tokens_menu():
    """Menu to add Discord tokens."""
    print("\n=== ДОБАВЛЕНИЕ DISCORD ТОКЕНОВ ===")
    print("Вы можете ввести токен вручную или указать путь к файлу с токенами.")
    print("[1] Ввести токен вручную")
    print("[2] Загрузить из файла tokens.txt (каждый токен с новой строки или 'token|proxy')")
    print("[3] Вернуться назад")
    
    choice = input("\nВыберите действие: ").strip()
    
    if choice == "1":
        token = input("Введите Discord токен: ").strip()
        proxy = input("Введите прокси (http://user:pass@ip:port) или оставьте пустым: ").strip()
        if not proxy: proxy = None

        if token:
            if add_token(token, proxy=proxy):
                print("Токен успешно добавлен!")
            else:
                print("Этот токен уже существует или произошла ошибка.")
    elif choice == "2":
        filepath = input("Укажите путь к файлу (по умолчанию tokens.txt): ").strip()
        if not filepath:
            filepath = "tokens.txt"
            
        if not os.path.exists(filepath):
            # Create sample tokens.txt
            with open(filepath, "w", encoding="utf-8") as f:
                f.write("# Вставьте ваши Discord токены сюда, каждый с новой строки\n")
            print(f"Файл {filepath} не найден. Мы создали пустой шаблон. Вставьте туда токены и повторите попытку.")
            return
            
        added = 0
        try:
            with open(filepath, "r", encoding="utf-8") as f:
                for line in f:
                    line = line.strip()
                    if line and not line.startswith("#"):
                        proxy = None
                        token = line
                        if "|" in line:
                            parts = line.split("|")
                            token = parts[0].strip()
                            proxy = parts[1].strip()

                        if add_token(token, proxy=proxy):
                            added += 1
            print(f"Успешно импортировано {added} токенов.")
        except Exception as e:
            print(f"Ошибка при чтении файла: {e}")

def run_scraper_menu():
    """Menu to run the member scraper."""
    print("\n=== СБОР УЧАСТНИКОВ СЕРВЕРА (ПАРСИНГ) ===")
    tokens = get_active_tokens()
    if not tokens:
        print("Ошибка: Нет активных токенов в БД. Сначала добавьте токены!")
        return
        
    print(f"Доступно активных аккаунтов для сбора: {len(tokens)}")
    print("Выберите аккаунт для парсинга:")
    for idx, t in enumerate(tokens, 1):
        proxy_info = f" | Прокси: {t['proxy']}" if t['proxy'] else " | Без прокси"
        print(f"[{idx}] {t['username'] or 'Без имени'} ({t['token'][:15]}...){proxy_info}")
        
    try:
        token_choice = int(input("\nВыберите номер аккаунта: ").strip()) - 1
        if token_choice < 0 or token_choice >= len(tokens):
            print("Неверный номер.")
            return
    except ValueError:
        print("Неверный ввод.")
        return
        
    selected_token = tokens[token_choice]["token"]
    
    target = input("Введите инвайт-ссылку на сервер (например, https://discord.gg/abc) или ID сервера: ").strip()
    if not target:
        print("Поле не может быть пустым.")
        return
        
    print("\nЗапуск сбора участников... Пожалуйста, подождите...")
    success, err = run_scraper(selected_token, target)
    if success:
        print("\n[УСПЕХ] Участники сервера успешно собраны и сохранены в базу данных.")
    else:
        print(f"\n[ОШИБКА] Сбор участников завершился ошибкой: {err}")

def settings_menu():
    """Menu to configure delay and message settings."""
    config = load_config()
    while True:
        print("\n=== НАСТРОЙКИ СИСТЕМЫ ===")
        print(f"[1] Текст сообщения: {config.get('base_message')}")
        print(f"[2] Ключ Gemini API: {'Установлен' if config.get('gemini_api_key') != 'YOUR_GEMINI_API_KEY_HERE' and config.get('gemini_api_key') else 'НЕ УСТАНОВЛЕН'}")
        print(f"[3] Задержка отправки заявок в друзья: {config.get('delay_friend_request_min_seconds')} - {config.get('delay_friend_request_max_seconds')} сек.")
        print(f"[4] Задержка отправки сообщений после добавления: {config.get('delay_message_min_seconds')} - {config.get('delay_message_max_seconds')} сек.")
        print(f"[5] Движок ИИ для перефразирования: {'БЕСПЛАТНЫЙ (g4f)' if config.get('ai_engine', 'free') == 'free' else 'Google Gemini (нужен ключ)'}")
        print(f"[6] Ключ 2Captcha (для авто-капчи): {'Установлен' if config.get('2captcha_api_key') else 'НЕ УСТАНОВЛЕН'}")
        print("[7] Сохранить и вернуться в главное меню")
        
        choice = input("\nВыберите параметр для изменения: ").strip()
        
        if choice == "1":
            print("\nВведите текст сообщения. Нажмите Enter, затем Ctrl+Z (Windows) или Ctrl+D (Mac/Linux) и Enter, чтобы сохранить:")
            lines = sys.stdin.read()
            if lines.strip():
                config["base_message"] = lines.strip()
        elif choice == "2":
            key = input("Введите ваш Gemini API Key: ").strip()
            if key:
                config["gemini_api_key"] = key
        elif choice == "3":
            try:
                min_sec = int(input("Минимальная задержка (сек): ").strip())
                max_sec = int(input("Максимальная задержка (сек): ").strip())
                if min_sec > 0 and max_sec >= min_sec:
                    config["delay_friend_request_min_seconds"] = min_sec
                    config["delay_friend_request_max_seconds"] = max_sec
                else:
                    print("Максимальная задержка должна быть больше или равна минимальной.")
            except ValueError:
                print("Некорректное число.")
        elif choice == "4":
            try:
                min_sec = int(input("Минимальная задержка (сек): ").strip())
                max_sec = int(input("Максимальная задержка (сек): ").strip())
                if min_sec > 0 and max_sec >= min_sec:
                    config["delay_message_min_seconds"] = min_sec
                    config["delay_message_max_seconds"] = max_sec
                else:
                    print("Максимальная задержка должна быть больше или равна минимальной.")
            except ValueError:
                print("Некорректное число.")
        elif choice == "5":
            print("\nВыберите движок ИИ:")
            print("[1] БЕСПЛАТНЫЙ (g4f — GPT-4o-mini / Llama 3, без ключа, без ограничений)")
            print("[2] Google Gemini (требует API ключ)")
            eng_choice = input("Ваш выбор: ").strip()
            if eng_choice == "1":
                config["ai_engine"] = "free"
                print("Установлен бесплатный движок.")
            elif eng_choice == "2":
                config["ai_engine"] = "gemini"
                print("Установлен Google Gemini.")
        elif choice == "6":
            key = input("Введите ваш 2Captcha API Key: ").strip()
            if key:
                config["2captcha_api_key"] = key
        elif choice == "7":
            save_config(config)
            break

def select_test_users_menu():
    """Allows the user to select specific people for test mailing and skip others."""
    from database import get_all_scraped_users, set_test_users, reset_all_skipped_users
    
    users = get_all_scraped_users()
    if not users:
        print("\nОшибка: База данных пуста. Сначала спарсите участников!")
        return
        
    print("\n=== СПИСОК УЧАСТНИКОВ В БАЗЕ ДАННЫХ ===")
    print("Участники, доступные для тестирования:")
    for idx, u in enumerate(users, 1):
        status_info = f"[Дружба: {u['friend_status']}, ЛС: {u['message_status']}]"
        print(f"[{idx}] {u['username']} (Отображаемое имя: {u['display_name']}) {status_info}")
        
    print("\n[А] Выбрать конкретных людей для теста")
    print("[Б] Сбросить ограничения (включить рассылку для ВСЕХ участников)")
    print("[В] Назад")
    
    choice = input("\nВыберите действие: ").strip().lower()
    
    if choice in ("а", "a"):
        numbers_str = input("Введите номера участников через запятую (например, 1, 3): ").strip()
        try:
            indices = [int(num.strip()) - 1 for num in numbers_str.split(",") if num.strip()]
            selected_ids = []
            selected_names = []
            for i in indices:
                if 0 <= i < len(users):
                    selected_ids.append(users[i]["user_id"])
                    selected_names.append(users[i]["username"])
                    
            if selected_ids:
                set_test_users(selected_ids)
                print(f"\n[УСПЕХ] Выбраны для рассылки: {', '.join(selected_names)}.")
                print("Все остальные участники временно скрыты (помечены как 'skipped').")
            else:
                print("Никто не выбран.")
        except ValueError:
            print("Некорректный ввод номеров.")
            
    elif choice in ("б", "b"):
        reset_all_skipped_users()
        print("\n[УСПЕХ] Ограничения сброшены. Рассылка пойдет по всем участникам в базе.")

def main_menu():
    """Main execution CLI loop."""
    init_db()
    load_config()
    
    while True:
        print("\n=============================================")
        print("          DISCORD MAILER BOT SYSTEM          ")
        print("=============================================")
        print("[1] Показать статистику базы данных")
        print("[2] Управление токенами (добавить аккаунты)")
        print("[3] Спарсить участников сервера")
        print("[4] Запустить автоматическую рассылку (Друзья + ЛС)")
        print("[5] Настройки (сообщение, API ключ, задержки)")
        print("[6] Выбор конкретных пользователей для теста")
        print("[7] Выход")
        print("=============================================")
        
        choice = input("Выберите пункт меню: ").strip()
        
        if choice == "1":
            show_stats_menu()
        elif choice == "2":
            add_tokens_menu()
        elif choice == "3":
            run_scraper_menu()
        elif choice == "4":
            print("\nЗапуск рассылки... Нажмите Ctrl+C в любой момент, чтобы остановить процесс.")
            try:
                asyncio.run(start_all_senders())
            except KeyboardInterrupt:
                print("\n[ИНФО] Рассылка приостановлена пользователем.")
            except Exception as e:
                print(f"[ОШИБКА] Ошибка при выполнении рассылки: {e}")
        elif choice == "5":
            settings_menu()
        elif choice == "6":
            select_test_users_menu()
        elif choice == "7":
            print("Спасибо за использование Discord Mailer! До свидания.")
            sys.exit(0)
        else:
            print("Некорректный выбор. Попробуйте еще раз.")

if __name__ == "__main__":
    main_menu()
