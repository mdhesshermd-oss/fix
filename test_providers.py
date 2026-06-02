import g4f

providers_to_test = [
    ("BlackboxPro", g4f.Provider.BlackboxPro, "gpt-4o"),
    ("PollinationsAI", g4f.Provider.PollinationsAI, "openai"),
    ("CablyAI", g4f.Provider.CablyAI, "gpt-4o-mini"),
    ("DeepInfra", g4f.Provider.DeepInfra, "meta-llama/Meta-Llama-3-70B-Instruct-Turbo"),
    ("HuggingChat", g4f.Provider.HuggingChat, "meta-llama/Meta-Llama-3.1-70B-Instruct"),
    ("EasyChat", g4f.Provider.EasyChat, "gpt-4o-mini"),
    ("OpenRouterFree", g4f.Provider.OpenRouterFree, "meta-llama/llama-3-8b-instruct:free"),
]

prompt = "Hello! Tell me a 1-word greeting."

for name, provider, model in providers_to_test:
    try:
        print(f"Testing {name} with model {model}...", end="", flush=True)
        response = g4f.ChatCompletion.create(
            model=model,
            provider=provider,
            messages=[{"role": "user", "content": prompt}],
            timeout=10
        )
        if response and len(str(response).strip()) > 0:
            print(f" SUCCESS! Response: {str(response).strip()}")
        else:
            print(" FAILED (empty response)")
    except Exception as e:
        print(f" FAILED: {e}")
