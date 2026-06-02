import sqlite3
import os

DB_PATH = os.path.join(os.path.dirname(os.path.abspath(__file__)), "sender_data.db")

def init_db():
    """Initializes the SQLite database tables."""
    conn = sqlite3.connect(DB_PATH)
    cursor = conn.cursor()
    
    # Create tokens table
    cursor.execute("""
    CREATE TABLE IF NOT EXISTS tokens (
        id INTEGER PRIMARY KEY AUTOINCREMENT,
        token TEXT UNIQUE NOT NULL,
        username TEXT,
        status TEXT DEFAULT 'active',
        last_used TIMESTAMP
    )
    """)
    
    # Create users table
    cursor.execute("""
    CREATE TABLE IF NOT EXISTS users (
        id INTEGER PRIMARY KEY AUTOINCREMENT,
        user_id TEXT UNIQUE NOT NULL,
        username TEXT NOT NULL,
        display_name TEXT,
        is_admin INTEGER DEFAULT 0,
        guild_id TEXT NOT NULL,
        guild_name TEXT,
        friend_status TEXT DEFAULT 'none', -- none, request_sent, accepted, failed
        message_status TEXT DEFAULT 'none', -- none, sent, failed
        updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
    )
    """)
    
    conn.commit()
    conn.close()

def add_token(token: str, username: str = None) -> bool:
    """Adds a new discord user token."""
    conn = sqlite3.connect(DB_PATH)
    cursor = conn.cursor()
    try:
        cursor.execute(
            "INSERT OR IGNORE INTO tokens (token, username) VALUES (?, ?)",
            (token, username)
        )
        conn.commit()
        return cursor.rowcount > 0
    except Exception as e:
        print(f"[DB Error] Cannot add token: {e}")
        return False
    finally:
        conn.close()

def get_active_tokens():
    """Retrieves list of active tokens."""
    conn = sqlite3.connect(DB_PATH)
    cursor = conn.cursor()
    cursor.execute("SELECT id, token, username, status FROM tokens WHERE status = 'active'")
    rows = cursor.fetchall()
    conn.close()
    return [{"id": r[0], "token": r[1], "username": r[2], "status": r[3]} for r in rows]

def update_token_status(token_id: int, status: str):
    """Updates token status (e.g. invalid, rate_limited)."""
    conn = sqlite3.connect(DB_PATH)
    cursor = conn.cursor()
    cursor.execute("UPDATE tokens SET status = ?, last_used = CURRENT_TIMESTAMP WHERE id = ?", (status, token_id))
    conn.commit()
    conn.close()

def add_scraped_users(users_list: list):
    """
    Inserts or updates scraped users.
    users_list is a list of dicts:
    [{'user_id': '...', 'username': '...', 'display_name': '...', 'is_admin': 0, 'guild_id': '...', 'guild_name': '...'}]
    """
    conn = sqlite3.connect(DB_PATH)
    cursor = conn.cursor()
    added_count = 0
    for u in users_list:
        try:
            cursor.execute("""
            INSERT INTO users (user_id, username, display_name, is_admin, guild_id, guild_name)
            VALUES (?, ?, ?, ?, ?, ?)
            ON CONFLICT(user_id) DO UPDATE SET
                username = excluded.username,
                display_name = excluded.display_name,
                is_admin = excluded.is_admin,
                guild_id = excluded.guild_id,
                guild_name = excluded.guild_name
            """, (u['user_id'], u['username'], u['display_name'], u['is_admin'], u['guild_id'], u['guild_name']))
            if cursor.rowcount > 0:
                added_count += 1
        except Exception as e:
            print(f"[DB Error] Failed to insert user {u['username']}: {e}")
    conn.commit()
    conn.close()
    return added_count

def get_users_to_friend(limit=10):
    """Gets users with friend_status = 'none' and who are NOT admin."""
    conn = sqlite3.connect(DB_PATH)
    cursor = conn.cursor()
    cursor.execute("""
        SELECT user_id, username, display_name, guild_id 
        FROM users 
        WHERE friend_status = 'none' AND is_admin = 0 
        LIMIT ?
    """, (limit,))
    rows = cursor.fetchall()
    conn.close()
    return [{"user_id": r[0], "username": r[1], "display_name": r[2], "guild_id": r[3]} for r in rows]

def update_friend_status(user_id: str, status: str):
    """Updates friend status of a user (e.g. request_sent, failed, accepted)."""
    conn = sqlite3.connect(DB_PATH)
    cursor = conn.cursor()
    cursor.execute("UPDATE users SET friend_status = ?, updated_at = CURRENT_TIMESTAMP WHERE user_id = ?", (status, user_id))
    conn.commit()
    conn.close()

def get_users_to_message(limit=10):
    """Gets users who accepted friend request but haven't received message yet."""
    conn = sqlite3.connect(DB_PATH)
    cursor = conn.cursor()
    cursor.execute("""
        SELECT user_id, username, display_name 
        FROM users 
        WHERE friend_status = 'accepted' AND message_status = 'none' 
        LIMIT ?
    """, (limit,))
    rows = cursor.fetchall()
    conn.close()
    return [{"user_id": r[0], "username": r[1], "display_name": r[2]} for r in rows]

def update_message_status(user_id: str, status: str):
    """Updates the message status (e.g. sent, failed)."""
    conn = sqlite3.connect(DB_PATH)
    cursor = conn.cursor()
    cursor.execute("UPDATE users SET message_status = ?, updated_at = CURRENT_TIMESTAMP WHERE user_id = ?", (status, user_id))
    conn.commit()
    conn.close()

def get_all_scraped_users():
    """Returns all scraped users in database."""
    conn = sqlite3.connect(DB_PATH)
    cursor = conn.cursor()
    cursor.execute("SELECT user_id, username, display_name, friend_status, message_status FROM users")
    rows = cursor.fetchall()
    conn.close()
    return [{"user_id": r[0], "username": r[1], "display_name": r[2], "friend_status": r[3], "message_status": r[4]} for r in rows]

def set_test_users(selected_user_ids: list):
    """Sets friend_status to 'skipped' for all users except those in the selected list."""
    conn = sqlite3.connect(DB_PATH)
    cursor = conn.cursor()
    # First, mark everything except selected as skipped (including admins)
    cursor.execute("UPDATE users SET friend_status = 'skipped'")
    # Then set selected users back to none (ready to friend), reset message_status, and set is_admin to 0 so they can be processed
    for u_id in selected_user_ids:
        cursor.execute("UPDATE users SET friend_status = 'none', message_status = 'none', is_admin = 0 WHERE user_id = ?", (u_id,))
    conn.commit()
    conn.close()

def reset_all_skipped_users():
    """Resets all 'skipped' users back to 'none'."""
    conn = sqlite3.connect(DB_PATH)
    cursor = conn.cursor()
    cursor.execute("UPDATE users SET friend_status = 'none' WHERE friend_status = 'skipped'")
    conn.commit()
    conn.close()

def get_stats():
    """Returns general database stats."""
    conn = sqlite3.connect(DB_PATH)
    cursor = conn.cursor()
    
    stats = {}
    cursor.execute("SELECT COUNT(*) FROM tokens")
    stats["total_tokens"] = cursor.fetchone()[0]
    
    cursor.execute("SELECT COUNT(*) FROM tokens WHERE status = 'active'")
    stats["active_tokens"] = cursor.fetchone()[0]
    
    cursor.execute("SELECT COUNT(*) FROM users")
    stats["total_users"] = cursor.fetchone()[0]
    
    cursor.execute("SELECT COUNT(*) FROM users WHERE friend_status = 'none'")
    stats["friend_none"] = cursor.fetchone()[0]
    
    cursor.execute("SELECT COUNT(*) FROM users WHERE friend_status = 'request_sent'")
    stats["friend_sent"] = cursor.fetchone()[0]
    
    cursor.execute("SELECT COUNT(*) FROM users WHERE friend_status = 'accepted'")
    stats["friend_accepted"] = cursor.fetchone()[0]
    
    cursor.execute("SELECT COUNT(*) FROM users WHERE friend_status = 'failed'")
    stats["friend_failed"] = cursor.fetchone()[0]
    
    cursor.execute("SELECT COUNT(*) FROM users WHERE message_status = 'sent'")
    stats["messages_sent"] = cursor.fetchone()[0]
    
    cursor.execute("SELECT COUNT(*) FROM users WHERE message_status = 'failed'")
    stats["messages_failed"] = cursor.fetchone()[0]
    
    conn.close()
    return stats
