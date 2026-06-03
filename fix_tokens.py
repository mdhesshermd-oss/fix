import sqlite3
import os

# Путь к вашей базе данных
DB_PATH = os.path.join(os.path.dirname(os.path.abspath(__file__)), "sender_data.db")

def reset_tokens():
    if not os.path.exists(DB_PATH):
        print("Файл базы данных не найден!")
        return

    conn = sqlite3.connect(DB_PATH)
    cursor = conn.cursor()
    
    # Обновляем все токены, ставя им статус 'active'
    cursor.execute("UPDATE tokens SET status = 'active'")
    
    changes = conn.total_changes
    conn.commit()
    conn.close()
    
    print(f"Готово! Статус обновлен для {changes} токенов. Теперь они снова активны.")

if __name__ == "__main__":
    reset_tokens()