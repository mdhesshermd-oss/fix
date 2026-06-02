from google import genai
import json
import os

CONFIG_PATH = os.path.join(os.path.dirname(os.path.abspath(__file__)), "config.json")

if not os.path.exists(CONFIG_PATH):
    print("config.json not found")
    exit(1)

with open(CONFIG_PATH, "r", encoding="utf-8") as f:
    config = json.load(f)

api_key = config.get("gemini_api_key", "")
print(f"Using API Key: {api_key[:8]}...{api_key[-4:] if len(api_key) > 4 else ''}")

try:
    # Initialize the new Google GenAI client
    client = genai.Client(api_key=api_key)
    
    print("\n--- Testing Model gemini-2.5-flash ---")
    response = client.models.generate_content(
        model='gemini-2.5-flash',
        contents='Hello! Respond with one word.'
    )
    print(f"SUCCESS! Response: {response.text.strip()}")
    
except Exception as e:
    print(f"FAILED: {e}")
