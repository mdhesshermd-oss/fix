using System;
using PBOSharp;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using PBOSharp.Objects;
using PBOSharp.Enums;
using System.IO;

namespace PBOSharp_Example
{
    class Program
    {
        static void Main(string[] args)
        {
            // Создаем клиент
            PBOSharpClient client = new PBOSharpClient();

            // Подписываемся на события вывода логов
            client.onEvent += Client_onEvent;

            // Путь к вашему исходному PBO файлу
            string pboPath = Path.Combine(Environment.GetFolderPath(Environment.SpecialFolder.MyDocuments), "ALECarLock.pbo");

            // Анализируем PBO
            PBO pbo = client.AnalyzePBO(pboPath);

            if (pbo != null && pbo.PBOAnalysed)
            {
                // Жестко заданный целевой путь для распаковки
                string targetPath = @"C:\Users\md5\Desktop\Loadingscreen";

                // Запускаем выгрузку всех файлов в указанную папку
                client.ExtractAll(pbo, targetPath);
            }
            else
            {
                Console.WriteLine("Ошибка: Не удалось проанализировать PBO файл.");
            }

            Console.WriteLine("Процесс завершен. Нажмите любую клавишу для выхода...");
            Console.ReadKey();
        }
        private static void Client_onEvent(PBOSharpEventArgs args)
            => Console.WriteLine($"[{args.Type}] {args.Message}");
    }
}
