import json
import os
from paraphraser import paraphrase_message

def test_uniqueness():
    base_msg = "Привет! Как дела?"
    results = []
    for i in range(3):
        print(f"Попытка {i+1}...")
        res = paraphrase_message(base_msg)
        results.append(res)
        print(f"Результат: '{res}'")

    # Check for ZWS
    for r in results:
        has_zws = any(c in r for c in ["\u200b", "\u200c", "\u200d"])
        print(f"Содержит ZWS: {has_zws}")

if __name__ == "__main__":
    test_uniqueness()
