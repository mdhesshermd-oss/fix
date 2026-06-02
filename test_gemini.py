import google.generativeai as genai
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
    genai.configure(api_key=api_key)
    
    print("\n--- Listing Models ---")
    models = genai.list_models()
    for m in models:
        print(f"Model name: {m.name}, supported methods: {m.supported_generation_methods}")
except Exception as e:
    print(f"Error listing models: {e}")

gemini_models = ["gemini-1.5-flash", "gemini-pro", "gemini-1.5-pro"]
for m_name in gemini_models:
    try:
        print(f"\n--- Testing Model {m_name} ---")
        model = genai.GenerativeModel(m_name)
        response = model.generate_content("Hello! Respond with one word.")
        print(f"SUCCESS! Response: {response.text.strip()}")
    except Exception as e:
        print(f"FAILED {m_name}: {e}")
