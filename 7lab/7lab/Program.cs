using System;
using System.IO;
using System.Text;
using System.Threading.Tasks;

class Program
{
    static async Task Main(string[] args)
    {
        Console.WriteLine("Введите путь к текстовому файлу:");
        string filePath = Console.ReadLine();

        Console.WriteLine("Введите список символов для удаления (без пробелов):");
        string charactersToRemove = Console.ReadLine();

        try
        {
            await ProcessFileAsync(filePath, charactersToRemove);
            Console.WriteLine("Операция завершена успешно.");
        }
        catch (Exception ex)
        {
            Console.WriteLine($"Произошла ошибка: {ex.Message}");
        }
    }

    static async Task ProcessFileAsync(string filePath, string charactersToRemove)
    {
        string text = await ReadFileAsync(filePath);
        string modifiedText = RemoveCharacters(text, charactersToRemove);
        await SaveFileAsync(filePath, modifiedText);
    }

    static async Task<string> ReadFileAsync(string filePath)
    {
        using (StreamReader reader = new StreamReader(filePath))
        {
            return await reader.ReadToEndAsync();
        }
    }

    static string RemoveCharacters(string text, string charactersToRemove)
    {
        foreach (char c in charactersToRemove)
        {
            text = text.Replace(c.ToString(), string.Empty);
        }
        return text;
    }

    static async Task SaveFileAsync(string filePath, string text)
    {
        using (StreamWriter writer = new StreamWriter(filePath, false, Encoding.UTF8))
        {
            await writer.WriteAsync(text);
        }
    }
}
