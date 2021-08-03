using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using ShilglesLab;

namespace ShilglesLab.Comparers
{
    class ShinglesComparer : ITextComparer
    {
        public double Result { get; private set; }
        public List<int> HashesA { get; private set; }
        public List<int> HashesB { get; private set; }

        public int HashCount { get; private set; }
        public int ShingleSize { get; private set; }

        public ShinglesComparer(int HashCount, int ShingleSize)
        {
            this.HashCount = HashCount;
            this.ShingleSize = ShingleSize;
        }

        public double ProcessTexts(string textA, string textB)
        {
            Console.WriteLine("Обработка первого текста: ");
            HashesA = CalcMinHashesFromText(textA, HashCount, ShingleSize);
            Console.WriteLine("Обработка второго текста: ");
            HashesB = CalcMinHashesFromText(textB, HashCount, ShingleSize);
            return EstimateSimilarity(HashCount);
        }

/*------------------------Метод для сравнения сумм в хеш-таблицах двух текстов-----------------*/
        static public int CompareHashes(List<int> hashes1, List<int> hashes2)
        {
            int sim = 0;
            for (int i = 0; i < hashes1.Count; i++)
            {
                if (hashes1[i] == hashes2[i])
                {
                    sim += 1;
                }
            }
            return sim;
        }
/*-----------------------------------------------------------------------------------------*/

        private double EstimateSimilarity(int hashCount) 
        {
            double sim = CompareHashes(HashesA, HashesB);
            sim /= (double)hashCount;
            Result = sim;
            return sim;
        }


        private List<int> CalcMinHashesFromText(string text,int hashCount = 36, int shingleSize = 10)
        {
            return GetMinHashes(CalcHashes(GetShingles(Canonisation(text),shingleSize),hashCount));
        }

/*-------------------Метод удаления стоп-символов и стоп-слов из текста-------------------------*/
        private List<string> Canonisation(string text)
        {
            var resText = new List<string>();
            foreach (var word in text.ToLower().Split(Constants.StopSymbols))//стоп-символы например: , . ! 
            {
                if (!Constants.StopWords.Contains(word))//если слова нет в списке стоп-слова
                {
                    resText.Add(word);//добавим его в результат
                }
            }
            return resText;
        }
/*-----------------------------------------------------------------------------------------*/
/*----------------------------Метод получения шиинглов------------------------------------*/      
     private List<string> GetShingles(List<string> words, int count = 10)
        {
            List<string> shingles = new List<string>();//список содержит строку из 10 слов
                                                       //без пробелов
            for (int i = 0; i < words.Count - count + 1; i++) //после каждой итерации сдвиг
            {                                                 //на одно слово
                string shingle = "";
                foreach(var w in words.Skip(i).Take(count))
                {
                    shingle += w;
                }
                shingles.Add(shingle);
            }
            return shingles;
        }
        /*Example: 
        count = 2;
        TEXT: сборник задач физике старшей школы
        i = 1: shingle = сборникзадач
        i = 2: shingle = задачфизике ... etc*/
/*-----------------------------------------------------------------------------------------*/
/*-------------------------------Вычисление хеш-значений для каждой строки----------------*/
        private List<List<int>> CalcHashes(List<string> shingles, int countHashes = 36)
        {
            List<List<int>> hashes = new List<List<int>>();
            for (int i = 0; i < countHashes; i++) 
            { 
                hashes.Add(new List<int>());
                foreach(var shingle in shingles)
                {
                    hashes[i].Add(HashFunc(shingle, i));//возвращаемое хеш-значение добавим в список
                }
            }
            return hashes;
        }
/*-----------------------------------------------------------------------------------------*/
/*-------------------------Поиск строки с минимальным весом-------------------------------*/
        private List<int> GetMinHashes(List<List<int>> hashes)
        {
            List<int> minHashes = new List<int>();
            int hashNum = 0;
            foreach (var hash in hashes) 
            {
                var min = hash.First();
                int k = 0;
                int shinMin = 0;

                foreach (var sh in hash)
                {
                    if (min > sh)
                    {
                        min = sh;
                        shinMin = k;
                    }
                    k++;
                }
                Console.WriteLine("Hash #" + hashNum + " - Shingle #" + shinMin);
                hashNum++;
                minHashes.Add(min);
            }
            return minHashes;
        }
/*-----------------------------------------------------------------------------------------*/
/*---------------------Вычисление хеш-значения--------------------------------------------*/
        private int HashFunc(string shingle, int num) 
        {
            num += 84;
            return Hash(shingle, 2, Constants.SimpleNumbers[num]);
        }

        private int Hash(string shingle, int p, int mod)
        {
            int hash = (int)shingle[0];
            int m = 1;
            for (int i = 1; i < shingle.Length; i++, m*=p)
            {
                hash = (hash * p) % mod + (int)shingle[i]; //по схеме Горнера посчитаем значение в точке за линейное время от max степени
            }
            return hash % mod;//уменьшение по модулю?
        }
/*-----------------------------------------------------------------------------------------*/
    }
}
