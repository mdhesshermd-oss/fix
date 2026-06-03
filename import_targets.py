import sqlite3
import os
from database import init_db

def import_ids_to_db(filepath):
    init_db()
    if not os.path.exists(filepath):
        print(f"File {filepath} not found.")
        return

    conn = sqlite3.connect("sender_data.db")
    cursor = conn.cursor()

    with open(filepath, 'r') as f:
        ids = [line.strip() for line in f if line.strip()]

    print(f"Importing {len(ids)} IDs to database...")

    added = 0
    for user_id in ids:
        try:
            # We don't have usernames for all, so we use a placeholder
            cursor.execute("""
            INSERT OR IGNORE INTO users (user_id, username, guild_id)
            VALUES (?, ?, ?)
            """, (user_id, f"Target_{user_id}", "EXTRACTED_LIST"))
            if cursor.rowcount > 0:
                added += 1
        except Exception as e:
            # print(f"Error adding {user_id}: {e}")
            pass

    conn.commit()
    conn.close()
    print(f"Successfully added {added} new targets to the database.")

if __name__ == "__main__":
    import_ids_to_db("ids.txt")
