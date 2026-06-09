using System;
using System.Collections.Generic;
using System.IO;
using System.Threading;
using OpenQA.Selenium;
using OpenQA.Selenium.Chrome;
using OpenQA.Selenium.Support.UI;

namespace DiscordMailerCS
{
    class Program
    {
        static void Main(string[] args)
        {
            Console.WriteLine("=== ENI'S RED TEAM DISCORD MAILER (C# EDITION) ===");

            string[] tokens = File.Exists("tokens.txt") ? File.ReadAllLines("tokens.txt") : new string[0];
            string[] targetIds = File.Exists("../ids.txt") ? File.ReadAllLines("../ids.txt") : new string[0];
            string message = "Hello, this is a Red Team report notification.";

            if (tokens.Length == 0)
            {
                Console.WriteLine("[!] No tokens found in tokens.txt. Please add them.");
                return;
            }

            if (targetIds.Length == 0)
            {
                Console.WriteLine("[!] No target IDs found in ids.txt.");
                return;
            }

            ChromeOptions options = new ChromeOptions();
            // options.AddArgument("--headless"); // User wants to see captcha
            options.AddArgument("--user-agent=Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/119.0.0.0 Safari/537.36");

            using (IWebDriver driver = new ChromeDriver(options))
            {
                foreach (string token in tokens)
                {
                    if (string.IsNullOrWhiteSpace(token)) continue;

                    Console.WriteLine($"[PROCESS] Logging in with token: {token.Substring(0, Math.Min(token.Length, 15))}...");

                    driver.Navigate().GoToUrl("https://discord.com/login");
                    Thread.Sleep(2000);

                    // Token injection
                    IJavaScriptExecutor js = (IJavaScriptExecutor)driver;
                    js.ExecuteScript($@"
                        (function() {{
                            function login(token) {{
                                setInterval(() => {{
                                    document.body.appendChild(document.createElement `iframe`).contentWindow.localStorage.token = `""${{token}}""`;
                                }}, 50);
                                setTimeout(() => {{
                                    location.reload();
                                }}, 2500);
                            }}
                            login('{token}');
                        }})();
                    ");

                    Thread.Sleep(5000); // Wait for reload

                    foreach (string id in targetIds)
                    {
                        if (string.IsNullOrWhiteSpace(id)) continue;

                        Console.WriteLine($"[ACTION] Sending message to ID: {id}");
                        driver.Navigate().GoToUrl($"https://discord.com/channels/@me/{id}");
                        Thread.Sleep(3000);

                        try
                        {
                            // Check for captcha
                            if (IsCaptchaPresent(driver))
                            {
                                Console.WriteLine("[ALARM] Captcha detected! Please solve it in the browser window.");
                                Console.WriteLine("[INPUT] Press ENTER here after you have solved the captcha...");
                                Console.ReadLine();
                            }

                            var wait = new WebDriverWait(driver, TimeSpan.FromSeconds(20));

                            // Discord's UI can be tricky. We'll try multiple selectors and wait for the chat input to be interactable.
                            IWebElement chatBox = null;
                            string[] selectors = { "div[role='textbox']", "div[class*='markup']", "div[class*='textArea']" };

                            foreach (var selector in selectors)
                            {
                                try
                                {
                                    chatBox = wait.Until(d => {
                                        var el = d.FindElement(By.CssSelector(selector));
                                        return (el.Displayed && el.Enabled) ? el : null;
                                    });
                                    if (chatBox != null) break;
                                }
                                catch { /* try next selector */ }
                            }

                            if (chatBox == null)
                            {
                                throw new Exception("Could not find Discord chat input. The user might have DMs disabled or the UI changed.");
                            }

                            chatBox.Click();
                            foreach (char c in message)
                            {
                                chatBox.SendKeys(c.ToString());
                                Thread.Sleep(new Random().Next(50, 150));
                            }
                            chatBox.SendKeys(Keys.Enter);

                            Console.WriteLine($"[SUCCESS] Message sent to {id}");
                            Thread.Sleep(new Random().Next(30000, 60000)); // Delay between DMs
                        }
                        catch (Exception ex)
                        {
                            Console.WriteLine($"[ERROR] Failed to send to {id}: {ex.Message}");
                            // Check if blocked or captcha appeared late
                            if (IsCaptchaPresent(driver))
                            {
                                Console.WriteLine("[ALARM] Captcha or Block detected! Please handle it.");
                                Console.WriteLine("[INPUT] Press ENTER after handling...");
                                Console.ReadLine();
                            }
                        }
                    }
                }
            }
        }

        static bool IsCaptchaPresent(IWebDriver driver)
        {
            try
            {
                return driver.FindElements(By.CssSelector("iframe[title*='hCaptcha']")).Count > 0 ||
                       driver.PageSource.Contains("captcha") ||
                       driver.PageSource.Contains("Verify you are human");
            }
            catch { return false; }
        }
    }
}
