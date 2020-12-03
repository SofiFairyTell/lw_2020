using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.IO;

namespace invertindexsearch
{
    class Library
    {
        public string name { get; set; }
        public string path { get; set; }
        public double score { get; set; }
        public int flagall { get; set; }
        public int flagprase { get; set; }
        public Library()
        {
            name = "";
            path = "";
            score = 0;
            flagall = 0;
            flagprase = 0;
        }
        public Library(string n, string p)
        {
            name = n;
            path = p;
        }
        public Library(string n, string p, double s, int fa, int fp)
        {
            name = n;
            path = p;
            score = s;
            flagall = fa;
            flagprase = fp;
        }
    }

    class Program
    {
        static void Find(ref List<Library> libraries, string request)
        {
            string[] requestword = request.Split(' ');
            int[] flag = new int[requestword.Length];
            foreach (Library l in libraries)
            {
                double Score = 0;
                double Wsingle = 0;
                double Wpair = 0;
                double Wall = 0;
                double Wphrase = 0;
                int i = 0;
                for (i = 0; i < flag.Length; i++)
                    flag[i] = 0;
                string text = "";
                string[] textword;
                text = File.ReadAllText(String.Concat(l.path, l.name));//чтение текста из документа
                textword = text.Split(' ');
                for (i = 0; i < textword.Length; i++)
                {
                    for (int j = 0; j < requestword.Length; j++)
                    {
                        /*сравнение текста из поискового запроса с найденным текстом документа*/
                        if (String.Compare(textword[i], requestword[j]) == 0)
                        {
                            Wsingle++;
                            flag[j] = 1;
                            if (i + 1 < textword.Length && j + 1 < requestword.Length && String.Compare(textword[i + 1], requestword[j + 1]) == 0)
                            {
                                Wpair++;
                            }
                        }
                    }
                }
                i = 0;
                while (i < flag.Length && flag[i] == 1)
                    i++;
                if (i == flag.Length)
                    Wall = 1;
                if (text.IndexOf(request) != -1)
                    Wphrase = 1;
                Score = Wsingle + Wpair + Wall + Wphrase / 350;
                l.score = Score;
                l.flagall = (int)Wall;
                l.flagprase = (int)Wphrase;
            }
        }

        static void Ranging(List<Library> libraries)
        {
            var t = libraries.OrderByDescending(a => a.score);
            bool f1 = false;
            bool f2 = false;
            foreach (var k in t)
            {
                f1 = false;
                f2 = false;
                if (k.flagall == 1)
                    f1 = true;
                if (k.flagprase == 1)
                    f2 = true;
                Console.WriteLine(String.Format("Документ {0}:", k.name));
                Console.WriteLine(String.Format("Значение score: {0:f4}", k.score));
                Console.WriteLine(String.Format("Встречаются ли все слова: {0}", f1.ToString()));
                Console.WriteLine(String.Format("Слова в нужном порядке: {0}", f2.ToString()));
            }
        }

        static void Main(string[] args)
        {
            char ch;
            string path = @"C:\test\libr\";
            string name = ".txt";
            List<Library> libraries = new List<Library>();
            for (int i = 0; i < 5; i++)
            {
                libraries.Add(new Library(String.Concat(i + 1, name), path));
            }
            do
            {
                Console.WriteLine("Введите запрос");
                string request = Console.ReadLine();
                Find(ref libraries, request);
                Ranging(libraries);
                ch = Console.ReadKey().KeyChar;
            } while (ch != 8);
                    
        }
    }
}
