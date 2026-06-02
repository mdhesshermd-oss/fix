import requests
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

# URL without the key parameter
url = "https://generativelanguage.googleapis.com/v1beta/models/gemini-1.5-flash:generateContent"

# Pass the API key in the custom 'x-goog-api-key' header
headers = {
    "x-goog-api-key": api_key,
    "Content-Type": "application/json"
}

body = {
    "contents": [
        {"parts": [{"text": "Hello! Respond with exactly one word."}]}
    ]
}

try:
    print("Sending REST request with 'x-goog-api-key' header...")
    response = requests.post(url, headers=headers, json=body)
    print("HTTP Status Code:", response.status_code)
    print("Response Body:")
    print(response.text)
except Exception as e:
    print("REST Request failed:", e)
