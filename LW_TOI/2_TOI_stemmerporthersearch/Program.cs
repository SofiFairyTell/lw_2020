using System.IO;
using System.Text;
using System;
using Stemmer.RU;
using System.Text.RegularExpressions;

namespace Stemmer.RU
{
    class Porter
    {
        private const string VOWEL = "аеиоуыэюя";

        private const string PERFECTIVEGROUND = "((ив|ивши|ившись|ыв|ывши|ывшись)|((?<=[ая])(в|вши|вшись)))$";

        private const string REFLEXIVE = "(с[яь])$";

        private const string ADJECTIVE = "(ее|ие|ые|ое|ими|ыми|ей|ий|ый|ой|ем|им|ым|ом|его|ого|еых|ую|юю|ая|яя|ою|ею)$";

        private const string PARTICIPLE = "((ивш|ывш|ующ)|((?<=[ая])(ем|нн|вш|ющ|щ)))$";

        private const string VERB = "((ила|ыла|ена|ейте|уйте|ите|или|ыли|ей|уй|ил|ыл|им|ым|ены|ить|ыть|ишь|ую|ю)|((?<=[ая])(ла|на|ете|йте|ли|й|л|ем|н|ло|но|ет|ют|ны|ть|ешь|нно)))$";

        private const string NOUN = "(а|ев|ов|ие|ье|е|иями|ями|ами|еи|ии|и|ией|ей|ой|ий|й|и|ы|ь|ию|ью|ю|ия|ья|я)$";

        private const string RVRE = "^(.*?[аеиоуыэюя])(.*)$";

        private const string DERIVATIONAL = "(ост|ость)?";

        private const string SUPERLATIVE = "(ейше|ейш)?";

        public string Stemm(string word)
        {
            word = word.ToLower();
            word = word.Replace("ё", "е");
            if (IsMatch(word, RVRE))
            {
                if (!Replace(ref word, PERFECTIVEGROUND, ""))
                {
                    Replace(ref word, REFLEXIVE, "");
                    if (Replace(ref word, ADJECTIVE, ""))
                    {
                        Replace(ref word, PARTICIPLE, "");
                    }
                    else
                    {
                        if (!Replace(ref word, VERB, ""))
                        {
                            Replace(ref word, NOUN, "");
                        }

                    }
                }

                Replace(ref word, "и$", "");

                if (IsMatch(word, DERIVATIONAL))
                {
                    Replace(ref word, DERIVATIONAL, "");
                }

                if (!Replace(ref word, "ь$", ""))
                {
                    Replace(ref word, SUPERLATIVE, "");
                    Replace(ref word, "нн$", "н");
                }

            }

            return word;
        }

        private bool IsMatch(string word, string matchingPattern)
        {
            return new Regex(matchingPattern).IsMatch(word);
        }

        private bool Replace(ref string replace, string cleaningPattern, string by)
        {
            string original = replace;
            replace = new Regex(cleaningPattern,
                        RegexOptions.ExplicitCapture |
                        RegexOptions.Singleline
                        ).Replace(replace, by);
            return original != replace;
        }
    }
}

namespace Stemmer
{
    class Program
    {
        static void Main(string[] args)
        {
            //string path = @"C:\Users\Risha\Desktop\text.txt";
            
            string a = "";
            string word = "";
            Porter p = new Porter();
            Console.WriteLine("Введите адрес исходного файла: ");
            string path = Console.ReadLine();
            while (true)
            {
                Console.WriteLine("Введите слово:");

                string input = Console.ReadLine();

                if (input != "выйти")
                {
                    Console.WriteLine("Основа слова " + "input " + ": " + p.Stemm(input));
                }
                else
                {
                    break;
                }

                word = p.Stemm(input);
                FileStream stream = new FileStream(@"C:\test\output.txt", FileMode.Create);
                StreamWriter writer = new StreamWriter(stream);

                try
                {
                    Console.WriteLine();
                    Console.WriteLine("Однокоренные слова в файле:");
                    using (StreamReader sr = new StreamReader(path, Encoding.Default))
                    {
                        string line;
                        while ((line = sr.ReadLine()) != null)
                        {

                            a = line;
                            if (a.IndexOf(word) > -1)
                            {
                                Console.WriteLine(a);
                                writer.WriteLine(a);
                            }
                        }
                    }
                    Console.WriteLine();
                }
                catch (Exception e)
                {
                    Console.WriteLine(e.Message);
                }
                writer.Close();
            }
        }
    }
}

