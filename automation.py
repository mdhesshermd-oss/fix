import pyautogui
import time
import random
import os

# Настройки безопасности
pyautogui.FAILSAFE = True  # Резкий увод мыши в угол остановит скрипт

def human_move(x, y):
    """Перемещение мыши по кривой, имитируя человеческую руку."""
    # Установка случайной скорости
    duration = random.uniform(0.5, 1.5)
    # Использование более естественной анимации (Ease-In-Out)
    pyautogui.moveTo(x, y, duration=duration, tween=pyautogui.easeInOutQuad)

def human_click(x, y):
    """Клик с небольшой задержкой нажатия."""
    human_move(x, y)
    time.sleep(random.uniform(0.1, 0.3))
    pyautogui.mouseDown()
    time.sleep(random.uniform(0.05, 0.15))
    pyautogui.mouseUp()

def check_for_captcha(template_path="captcha_icon.png"):
    """
    Пытается найти окно капчи на экране.
    Требуется скриншот иконки капчи для сравнения.
    """
    try:
        location = pyautogui.locateOnScreen(template_path, confidence=0.8)
        if location:
            print("[AUTOMATION] Обнаружено окно капчи!")
            return location
    except Exception as e:
        # print(f"[DEBUG] Поиск капчи: {e}")
        pass
    return None

def solve_interaction():
    """Пример автоматизированного действия для прохождения простой проверки."""
    # Логика зависит от типа окна Discord
    # Здесь мы имитируем клик по центру экрана или в зону 'Я человек'
    screen_width, screen_height = pyautogui.size()
    target_x = screen_width // 2
    target_y = screen_height // 2

    print("[AUTOMATION] Попытка визуального взаимодействия...")
    human_click(target_x, target_y + 100) # Клик чуть ниже центра
    time.sleep(random.uniform(2.0, 4.0))

if __name__ == "__main__":
    print("Тестирование модуля автоматизации через 5 секунд...")
    time.sleep(5)
    solve_interaction()
