import re
import os

def extract_ids(filepath):
    if not os.path.exists(filepath):
        print(f"File {filepath} not found.")
        return []

    with open(filepath, 'r', encoding='utf-8') as f:
        content = f.read()

    # Regex to find discord.com/users/(\d+)
    ids = re.findall(r'discord\.com/users/(\d+)', content)
    return list(set(ids))

if __name__ == "__main__":
    ids = extract_ids('rassilka.html')
    with open('ids.txt', 'w') as f:
        for user_id in ids:
            f.write(f"{user_id}\n")
    print(f"Extracted {len(ids)} unique IDs to ids.txt")
