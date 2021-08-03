using System;
using System.IO;
using System.Text;

namespace ShilglesLab
{
    class Program
    {
        static void Main(string[] args)
        {
            TextComparisonEngine engine = new TextComparisonEngine();
            
            while (true)
            {
                Console.WriteLine("Enter names of local text files to compare:");
                Console.Write("File #1: ");
                string textA = File.ReadAllText(Console.ReadLine(), System.Text.Encoding.GetEncoding(1251));
                Console.Write("File #2: ");
                string textB = File.ReadAllText(Console.ReadLine(), System.Text.Encoding.GetEncoding(1251));

                engine.Compare(textA, textB);
                engine.PrintResults();
            }
        }
    }
}
