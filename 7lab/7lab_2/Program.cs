using System;
using System.Diagnostics;
using System.Net.Http;
using System.Threading.Tasks;

class Program
{
    static async Task Main()
    {
        Console.WriteLine("Асинхронное выполнение");
        await ExecuteAsync();

        Console.WriteLine("\nСинхронное выполнение");
        ExecuteSync();

        Console.ReadLine();
    }

    static async Task ExecuteAsync()
    {
        var stopwatch = new Stopwatch();
        stopwatch.Start();

        var tasks = new Task[10];
        for (int i = 0; i < tasks.Length; i++)
        {
            tasks[i] = FetchAndPrintDogImageAsync(i + 1);
        }

        await Task.WhenAll(tasks);

        stopwatch.Stop();
        Console.WriteLine($"Время выполнения асинхронного запроса: {stopwatch.ElapsedMilliseconds} мс");
    }

    static void ExecuteSync()
    {
        var stopwatch = new Stopwatch();
        stopwatch.Start();

        for (int i = 0; i < 10; i++)
        {
            FetchAndPrintDogImageSync(i + 1).Wait();
        }

        stopwatch.Stop();
        Console.WriteLine($"Время выполнения синхронного запроса: {stopwatch.ElapsedMilliseconds} мс");
    }

    static async Task FetchAndPrintDogImageAsync(int index)
    {
        using (var httpClient = new HttpClient())
        {
            try
            {
                var response = await httpClient.GetStringAsync("https://dog.ceo/api/breeds/image/random");
                Console.WriteLine($"Асинхронный запрос {index}: {response}");
            }
            catch (HttpRequestException ex)
            {
                Console.WriteLine($"Ошибка при выполнении асинхронного запроса {index}: {ex.Message}");
            }
        }
    }

    static async Task FetchAndPrintDogImageSync(int index)
    {
        using (var httpClient = new HttpClient())
        {
            try
            {
                var response = await httpClient.GetStringAsync("https://dog.ceo/api/breeds/image/random");
                Console.WriteLine($"Синхронный запрос {index}: {response}");
            }
            catch (HttpRequestException ex)
            {
                Console.WriteLine($"Ошибка при выполнении синхронного запроса {index}: {ex.Message}");
            }
        }
    }
}
