import requests

# Вставьте сюда ваш прокси для проверки
# Формат: http://IP:PORT или http://USER:PASS@IP:PORT
proxy_to_test = "http://91.203.242.66:222" 

proxies = {
    "http": proxy_to_test,
    "https": proxy_to_test,
}

print(f"Проверка прокси: {proxy_to_test}...")

try:
    # Мы пытаемся зайти на API Дискорда, так как именно это будет делать бот
    response = requests.get("https://discord.com/api/v9/experiments", proxies=proxies, timeout=10)
    
    if response.status_code == 200:
        print("[УСПЕХ] Прокси работает! Discord ответил корректно.")
    elif response.status_code == 403:
        print("[ОШИБКА] Прокси работает, но DISCORD ЗАБЛОКИРОВАЛ этот IP (Cloudflare 403).")
    else:
        print(f"[ПРЕДУПРЕЖДЕНИЕ] Прокси ответил, но код ошибки: {response.status_code}")

except requests.exceptions.ProxyError:
    print("[ОШИБКА] Не удалось подключиться к прокси-серверу. Он либо выключен, либо адрес неверный.")
except requests.exceptions.ConnectTimeout:
    print("[ОШИБКА] Время ожидания истекло. Прокси слишком медленный.")
except Exception as e:
    print(f"[ОШИБКА] Произошла непредвиденная ошибка: {e}")