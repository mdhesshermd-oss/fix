import json
import os
import time
import google.generativeai as genai

CONFIG_PATH = os.path.join(os.path.dirname(os.path.abspath(__file__)), "config.json")

def load_config():
    """Loads config.json safely."""
    try:
        if os.path.exists(CONFIG_PATH):
            with open(CONFIG_PATH, "r", encoding="utf-8") as f:
                return json.load(f)
    except Exception as e:
        print(f"[Config Error] Failed to load config: {e}")
    return {}

import random

def add_random_invisible_chars(text: str) -> str:
    """Adds random zero-width characters to the end of the text to ensure uniqueness."""
    chars = ["\u200b", "\u200c", "\u200d"]
    suffix = "".join(random.choice(chars) for _ in range(random.randint(3, 10)))
    return text + suffix

def paraphrase_free(base_message: str) -> str:
    """
    Attempts to paraphrase the base message using free GPT4Free (g4f) providers.
    Does not require any API key, is unlimited and includes auto-retry on rate limits.
    """
    try:
        import g4f
    except ImportError:
        print("[AI Error] Библиотека g4f не установлена. Используем оригинал с невидимыми символами.")
        return add_random_invisible_chars(base_message)

    tones = [
        "дружелюбный и разговорный",
        "вежливый и профессиональный",
        "воодушевленный и открытый",
        "неформальный и приветливый",
        "простой и естественный"
    ]
    selected_tone = random.choice(tones)
    random_salt = random.randint(1000, 99999)

    prompt = (
        "Ты — профессиональный копирайтер и редактор игровых текстов. Твоя задача — немного изменить/перефразировать предложенный текст, "
        "чтобы он стал уникальным для обхода спам-фильтров Discord, но при этом КРАЙНЕ строго соблюдай следующие правила:\n\n"
        "1. КРАЙНЕ ВАЖНО: Ни в коем случае не меняй ключевые игровые термины, цифры и названия!\n"
        "   - Слово \"клан\" (clans) НЕЛЬЗЯ заменять на \"клон\" (clones) или другие слова! Всегда оставляй только \"клан\".\n"
        "   - Название игры (DayZ) и IP-адрес (212.22.85.57:2302) должны остаться ровно такими же.\n"
        "   - Названия машин (BMW M5/X5, Mercedes AMG G63, Hummer, Dodge Hellcat, Mustang Shelby, Tacoma, \"Буханка\") должны остаться абсолютно без изменений.\n"
        "   - Названия предметов (C4, кодлоки, плоскогубцы, топор, проволока, гвозди, шкаф, стойка для оружия) не должны меняться.\n"
        "   - Все ссылки (URL) и контакты должны остаться нетронутыми.\n"
        "2. СТРОГО СОХРАНЯЙ СТРУКТУРУ И ОФОРМЛЕНИЕ:\n"
        "   - Сохраняй абсолютно все эмодзи (🦊, 🔥, 🎁, 1️⃣, 2️⃣, 3️⃣) ровно на их местах.\n"
        "   - Сохраняй всю разметку Discord (жирный текст **, списки, переносы строк).\n"
        "   - Количество строк, абзацев и списков должно остаться точно таким же. Не объединяй абзацы и не придумывай новые разделы.\n"
        "3. ЧТО МОЖНО МЕНЯТЬ:\n"
        "   - Меняй только общие связующие фразы, прилагательные, глаголы и приветствия на подходящие синонимы, чтобы сделать предложения уникальными.\n"
        f"   - Стиль написания: {selected_tone}.\n"
        f"   - Уникальный идентификатор сессии: {random_salt}.\n"
        "4. Выведи ТОЛЬКО готовый перефразированный текст, без каких-либо кавычек в начале и конце, без вводных слов и пояснений.\n\n"
        f"Оригинальный текст:\n{base_message}"
    )

    # We prioritize PollinationsAI as it was tested successfully, and use others as fallback
    attempts = [
        {"provider": g4f.Provider.PollinationsAI, "model": "openai"},
        {"provider": g4f.Provider.BlackboxPro, "model": "gpt-4o"},
    ]

    for attempt in attempts:
        provider = attempt["provider"]
        model = attempt["model"]
        
        # Try up to 3 times for each provider if we hit rate limits (429 / Queue Full)
        for retry in range(3):
            try:
                print(f"[AI] Попытка перефразировать через {provider.__name__} (попытка {retry + 1})...")
                
                response = g4f.ChatCompletion.create(
                    model=model,
                    provider=provider,
                    messages=[{"role": "user", "content": prompt}],
                    timeout=12
                )
                
                if response and len(str(response).strip()) > 10:
                    cleaned_text = str(response).strip()
                    # Remove markdown quotes if LLM added them
                    if cleaned_text.startswith('"') and cleaned_text.endswith('"'):
                        cleaned_text = cleaned_text[1:-1].strip()
                    if cleaned_text.startswith("'''") and cleaned_text.endswith("'''"):
                        cleaned_text = cleaned_text[3:-3].strip()
                    if cleaned_text.startswith("```") and cleaned_text.endswith("```"):
                        cleaned_text = cleaned_text[3:-3].strip()
                    print(f"[AI] Успешно перефразировано через {provider.__name__}!")
                    return add_random_invisible_chars(cleaned_text)
                    
            except Exception as e:
                err_str = str(e).lower()
                print(f"[AI Warning] Ошибка провайдера {provider.__name__}: {e}")
                
                # If we get rate limited (429) or queue is full, wait and retry
                if "429" in err_str or "queue" in err_str or "limit" in err_str or "too many" in err_str:
                    wait_time = 4 + retry * 2 # Incremental backoff: 4s, 6s, 8s
                    print(f"[AI] Обнаружен лимит запросов. Ожидание {wait_time} секунд...")
                    time.sleep(wait_time)
                    continue
                else:
                    # Non-rate-limit error (e.g. 404 or connection issues), switch provider immediately
                    break

    print("[AI Warning] Все бесплатные провайдеры вернули ошибку. Используем оригинал с невидимыми символами.")
    return add_random_invisible_chars(base_message)

def paraphrase_message(base_message: str) -> str:
    """
    Paraphrases the message using multiple LLM fallbacks for maximum reliability.
    Strictly keeps IP/Discord parameters unchanged.
    """
    config = load_config()
    
    # 1. First Attempt: Google Gemini (if configured)
    if config.get("ai_engine") == "gemini":
        api_key = config.get("gemini_api_key", "")
        if api_key and api_key != "YOUR_GEMINI_API_KEY_HERE":
            try:
                genai.configure(api_key=api_key)
                # Try multiple Gemini versions
                for model_name in ["gemini-1.5-flash", "gemini-pro"]:
                    try:
                        model = genai.GenerativeModel(model_name)

                        tones = ["дружелюбный", "вежливый", "воодушевленный", "неформальный"]
                        prompt = (
                            "Перефразируй текст для Discord. СОХРАНИ IP 212.22.85.57:2302, DayZ, "
                            "названия машин, предметов, все эмодзи и ссылки. Не меняй структуру. "
                            f"Стиль: {random.choice(tones)}. Выдай ТОЛЬКО текст.\n\n"
                            f"Текст:\n{base_message}"
                        )

                        response = model.generate_content(prompt)
                        if response.text:
                            print(f"[AI] Успешно через Gemini ({model_name})")
                            return add_random_invisible_chars(response.text.strip())
                    except Exception:
                        continue
            except Exception as e:
                print(f"[AI Error] Сбой Gemini: {e}")

    # 2. Second Attempt: Free LLM Providers (G4F)
    print("[AI] Пробуем бесплатные нейросети (G4F)...")
    res = paraphrase_free(base_message)

    # Check if paraphrase_free actually changed the message or just returned base
    if res.replace("\u200b", "").replace("\u200c", "").replace("\u200d", "") != base_message:
        return res

    # 3. Final Fallback: Manual Obfuscation if all LLMs fail
    print("[AI Warning] Все LLM вернули ошибку. Применяем ручную уникализацию.")
    return add_random_invisible_chars(base_message)

if __name__ == "__main__":
    # Test paraphrasing
    config = load_config()
    msg = config.get("base_message", "Привет! Посмотри мой проект: https://github.com")
    print("--- ОРИГИНАЛ ---")
    print(msg)
    print("\n--- ПЕРЕФРАЗИРОВАНО (БЕСПЛАТНЫЙ ИИ) ---")
    print(paraphrase_free(msg))
