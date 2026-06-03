import json
import time
import random
import sys
import asyncio
from playwright.async_api import async_playwright

def load_config():
    """Loads configuration from config.json."""
    try:
        with open("config.json", "r", encoding="utf-8") as f:
            return json.load(f)
    except FileNotFoundError:
        print("[ERROR] config.json not found.")
        sys.exit(1)
    except json.JSONDecodeError:
        print("[ERROR] Failed to decode config.json.")
        sys.exit(1)

def load_ids(filepath="ids.txt"):
    """Loads user IDs from a file."""
    try:
        with open(filepath, "r") as f:
            return [line.strip() for line in f if line.strip()]
    except FileNotFoundError:
        print(f"[ERROR] {filepath} not found.")
        sys.exit(1)

async def main():
    config = load_config()
    ids = load_ids("ids.txt") # Use full list

    print(f"[SYSTEM] Configuration loaded. Targets: {len(ids)}")
    print(f"[SYSTEM] Base Message: {config.get('base_message')}")

    async with async_playwright() as p:
        # Launch browser in visible mode
        print("[SYSTEM] Launching browser...")
        try:
            browser = await p.chromium.launch(headless=False)
        except Exception as e:
            print(f"[ERROR] Could not launch headed browser: {e}")
            print("[INFO] If you are running on a server without a GUI, use 'xvfb-run python3 browser_sender.py' or set headless=True (but you won't see captchas).")
            return
        context = await browser.new_context()
        page = await context.new_page()

        print("[SYSTEM] Browser initialized in visible mode.")

        # Navigate to Discord login
        await page.goto("https://discord.com/login")
        print("[USER] Please log in to Discord in the browser window.")

        # Wait for user to be logged in (detecting transition to /channels/@me or presence of sidebar)
        while True:
            if "channels/@me" in page.url or await page.query_selector(".guilds-2J_vCC"):
                print("[SUCCESS] Login detected. Starting messaging process...")
                break
            await asyncio.sleep(2)

        # Initial wait for UI to stabilize
        await asyncio.sleep(5)

        base_message = config.get("base_message", "Hello!")

        for user_id in ids:
            print(f"[PROCESS] Target: {user_id}")

            # Navigate to user profile
            try:
                await page.goto(f"https://discord.com/users/{user_id}")
                # Wait for the profile or message button to appear
                # On Discord web, /users/id redirects to a DM if it exists or shows profile
                await asyncio.sleep(random.uniform(3.0, 5.0))

                # Check for "Send Message" button or presence in DM
                # This is a bit tricky as Discord's UI is complex.
                # Usually, it opens the DM directly if you navigate to /users/id

                # Check for "Hold Up!" or Captcha
                if "Sift" in await page.content() or "captcha" in page.url:
                    print("[ALERT] CAPTCHA DETECTED! Script paused.")
                    input("Solve the captcha in the browser and press Enter here to continue...")

                # Improved Typing and sending logic
                # Discord profile might be a modal, try to find the message button first

                # Check for "Send Message" button in profile modal
                message_btn = await page.query_selector('button:has-text("Message"), button:has-text("Отправить сообщение")')
                if message_btn:
                    await message_btn.click()
                    await asyncio.sleep(random.uniform(1.5, 3.0))

                # Now check for textbox
                textbox = await page.query_selector('[role="textbox"]')
                if not textbox:
                    # Try to wait a bit longer
                    await asyncio.sleep(2)
                    textbox = await page.query_selector('[role="textbox"]')

                if textbox:
                    print(f"[ACTION] Typing message to {user_id}...")
                    # Human-like typing with variation
                    await textbox.click()
                    for char in base_message:
                        await page.keyboard.type(char)
                        await asyncio.sleep(random.uniform(0.02, 0.10))

                    await asyncio.sleep(random.uniform(0.5, 1.5))
                    await page.keyboard.press("Enter")
                    print(f"[SUCCESS] Message sent to {user_id}")
                else:
                    print(f"[WARNING] No message path found for {user_id}. Skipping.")

                # Random delay between targets
                wait_time = random.randint(
                    config.get("delay_friend_request_min_seconds", 30),
                    config.get("delay_friend_request_max_seconds", 60)
                )
                print(f"[SLEEP] Waiting {wait_time}s before next target...")
                await asyncio.sleep(wait_time)

            except Exception as e:
                print(f"[ERROR] Failed to process {user_id}: {e}")

        print("[FINISH] All targets processed.")
        await browser.close()

if __name__ == "__main__":
    asyncio.run(main())
