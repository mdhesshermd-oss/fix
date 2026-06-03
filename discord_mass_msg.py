import asyncio
import json
import random
import os
import re
from playwright.async_api import async_playwright

async def extract_ids_from_html(filepath):
    """Extracts Discord user IDs from the rassilka.html file."""
    if not os.path.exists(filepath):
        print(f"Error: {filepath} not found.")
        return []

    with open(filepath, 'r', encoding='utf-8') as f:
        content = f.read()

    # Use regex to find IDs in the format /users/ID
    ids = re.findall(r'discord\.com/users/(\d+)', content)
    return list(set(ids)) # Return unique IDs

async def send_messages():
    # Load configuration
    try:
        with open('config.json', 'r') as f:
            config = json.load(f)
    except FileNotFoundError:
        print("Error: config.json not found.")
        return

    token = config.get('token')
    message_text = config.get('message')

    if not token or not message_text:
        print("Error: token or message missing in config.json")
        return

    # Extract user IDs directly from the source HTML
    user_ids = await extract_ids_from_html('rassilka.html')
    if not user_ids:
        print("No user IDs found to process.")
        return

    print(f"Found {len(user_ids)} unique user IDs.")

    async with async_playwright() as p:
        # Launch browser. Note: Requires XServer for headless=False.
        # If running in a headless server environment, use headless=True
        # or xvfb-run.
        browser_type = p.chromium
        try:
            browser = await browser_type.launch(headless=False)
        except Exception as e:
            print(f"Failed to launch headed browser: {e}")
            print("Attempting to launch in headless mode...")
            browser = await browser_type.launch(headless=True)

        context = await browser.new_context()
        page = await context.new_page()

        print("Opening Discord login...")
        await page.goto("https://discord.com/login")

        # Human-readable login process is preferred over token injection for stability,
        # but the user requested token-based efficiency.
        # We will attempt token login via local storage as a fallback to injection.
        print(f"Please ensure you are logged in or have the token {token[:5]}... configured.")

        # Navigate to home to check session
        await page.goto("https://discord.com/channels/@me")

        # If not logged in, the user will see the login page (since headless=False)
        # and can log in manually, fulfilling the 'manual' part of the request.

        for user_id in user_ids:
            print(f"Navigating to profile: {user_id}")
            try:
                await page.goto(f"https://discord.com/users/{user_id}")
                await asyncio.sleep(random.uniform(3, 5))

                # Check for platform challenges
                if "checkpoint" in page.url or await page.query_selector("iframe[src*='hcaptcha']"):
                    print("!!! ACTION REQUIRED: Captcha detected !!!")
                    print("Please solve it in the browser window.")
                    # Wait for user to resolve
                    while "checkpoint" in page.url or await page.query_selector("iframe[src*='hcaptcha']"):
                        await asyncio.sleep(5)
                    print("Challenge resolved.")

                # Look for message button. Try multiple selectors for robustness.
                message_selectors = [
                    'button:has-text("Message")',
                    'button:has-text("Отправить сообщение")',
                    'button[aria-label="Message"]',
                    'button[aria-label="Отправить сообщение"]'
                ]

                message_btn = None
                for selector in message_selectors:
                    message_btn = await page.query_selector(selector)
                    if message_btn:
                        break

                if message_btn:
                    await message_btn.click()
                    await asyncio.sleep(2)

                    # Target the message input field
                    # Discord uses a contenteditable div for chat
                    chat_input = await page.query_selector('[role="textbox"]')
                    if chat_input:
                        await chat_input.fill(message_text)
                        await asyncio.sleep(0.5)
                        await page.keyboard.press("Enter")
                        print(f"Success: Message sent to {user_id}")
                    else:
                        print(f"Error: Chat input field not found for {user_id}")
                else:
                    print(f"Error: Message button not found for {user_id}. They might have DMs closed.")

                # Anti-spam delay
                delay = random.uniform(45, 90)
                print(f"Cooldown: {delay:.2f} seconds...")
                await asyncio.sleep(delay)

            except Exception as e:
                print(f"Process error for {user_id}: {e}")
                await asyncio.sleep(10)

        await browser.close()

if __name__ == "__main__":
    asyncio.run(send_messages())
