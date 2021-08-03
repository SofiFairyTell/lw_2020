using System;
using System.IO;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Text.RegularExpressions;
using System.Collections;

namespace LSA_with_cluster
{
    class Porter
    {
        static char[] vowels = { 'а', 'о', 'у', 'ы', 'э', 'я', 'е', 'ю', 'и', 'А', 'О', 'У', 'Ы', 'Э', 'Я', 'Е', 'Ю', 'И' };
        static char[] consonants = { 'б', 'в', 'г', 'д', 'ж', 'з', 'й', 'к', 'л', 'м', 'н', 'п', 'р', 'с', 'т', 'ф', 'х', 'ц', 'ч', 'ш', 'щ', 'ъ', 'ь' };

        static Regex PERFECTIVEGERUND = new Regex("(ав|авши|авшись|яв|явши|явшись|ив|ивши|ившись|ыв|ывши|ывшись)$");
        static Regex REFLEXIVE = new Regex("(ся|сь)$");
        static Regex NOUN = new Regex("(а|ев|ов|ие|ье|е|иями|ями|ами|еи|ии|и|ией|ей|ой|ий|й|иям|ям|ием|ем|ам|ом|о|у|ах|иях|ях|ы|ь|ию|ью|ю|ия|ья|я)$");
        static Regex VERB = new Regex("(ила|ла|ена|ейте|уйте|ите|или|ыли|ей|уй|ил|ыл|им|ым|ен|ило|ыло|ено|ят|ует|уют|ит|ыт|ены|ить|ыть|ишь|ую|ю|ала|ана|аете|айте|али|ай|ал|аем|ан|ало|ано|ает|ают|аны|ать|аешь|анно|яла|яна|яете|яйте|яли|яй|ял|яем|ян|яло|яно|яет|яют|яны|ять|яешь|янно)$");
        static Regex ADJECTIVAL = new Regex("(ее|ие|ые|ое|ими|ыми|ей|ий|ый|ой|ем|им|ым|ом|его|ого|ему|ому|их|ых|ую|юю|ая|яя|ою|ею|ившее|ившие|ившые|ившое|ившими|ившыми|ившей|ивший|ившый|ившой|ившем|ившим|ившым|ившом|ившего|ившого|ившему|ившому|ивших|ившых|ившую|ившюю|ившая|ившяя|ившою|ившею|ывшее|ывшие|ывшые|ывшое|ывшими|ывшыми|ывшей|ывший|ывшый|ывшой|ывшем|ывшим|ывшым|ывшом|ывшего|ывшого|ывшему|ывшому|ывших|ывшых|ывшую|ывшюю|ывшая|ывшяя|ывшою|ывшею|ующее|ующие|ующые|ующое|ующими|ующыми|ующей|ующий|ующый|ующой|ующем|ующим|ующым|ующом|ующего|ующого|ующему|ующому|ующих|ующых|ующую|ующюю|ующая|ующяя|ующою|ующею|аемее|аемие|аемые|аемое|аемими|аемыми|аемей|аемий|аемый|аемой|аемем|аемим|аемым|аемом|аемего|аемого|аемему|аемому|аемих|аемых|аемую|аемюю|аемая|аемяя|аемою|аемею|аннее|анние|анные|анное|анними|анными|анней|анний|анный|анной|аннем|анним|анным|анном|аннего|анного|аннему|анному|анних|анных|анную|аннюю|анная|анняя|анною|аннею|авшее|авшие|авшые|авшое|авшими|авшыми|авшей|авший|авшый|авшой|авшем|авшим|авшым|авшом|авшего|авшого|авшему|авшому|авших|авшых|авшую|авшюю|авшая|авшяя|авшою|авшею|ающее|ающие|ающые|ающое|ающими|ающыми|ающей|ающий|ающый|ающой|ающем|ающим|ающым|ающом|ающего|ающого|ающему|ающому|ающих|ающых|ающую|ающюю|ающая|ающяя|ающою|ающею|ащее|ащие|ащые|ащое|ащими|ащыми|ащей|ащий|ащый|ащой|ащем|ащим|ащым|ащом|ащего|ащого|ащему|ащому|ащих|ащых|ащую|ащюю|ащая|ащяя|ащою|ащею|яемее|яемие|яемые|яемое|яемими|яемыми|яемей|яемий|яемый|яемой|яемем|яемим|яемым|яемом|яемего|яемого|яемему|яемому|яемих|яемых|яемую|яемюю|яемая|яемяя|яемою|яемею|яннее|янние|янные|янное|янними|янными|янней|янний|янный|янной|яннем|янним|янным|янном|яннего|янного|яннему|янному|янних|янных|янную|яннюю|янная|янняя|янною|яннею|явшее|явшие|явшые|явшое|явшими|явшыми|явшей|явший|явшый|явшой|явшем|явшим|явшым|явшом|явшего|явшого|явшему|явшому|явших|явшых|явшую|явшюю|явшая|явшяя|явшою|явшею|яющее|яющие|яющые|яющое|яющими|яющыми|яющей|яющий|яющый|яющой|яющем|яющим|яющым|яющом|яющего|яющого|яющему|яющому|яющих|яющых|яющую|яющюю|яющая|яющяя|яющою|яющею|ящее|ящие|ящые|ящое|ящими|ящыми|ящей|ящий|ящый|ящой|ящем|ящим|ящым|ящом|ящего|ящого|ящему|ящому|ящих|ящых|ящую|ящюю|ящая|ящяя|ящою|ящею)$");
        static Regex DERIVATIONAL = new Regex("(ост|ость)$");
        static Regex SUPERLATIVE = new Regex("(ейш|ейше)$");
        static Regex NN = new Regex("(нн)$");
        static Regex SOFTSIGN = new Regex("(ь)$");
        static Regex AND = new Regex("(и)$");

        public void Step1(string rv, ref string word)
        {
            if (PERFECTIVEGERUND.IsMatch(rv))
            {
                word = PERFECTIVEGERUND.Replace(word, "");
            }
            else
            {
                word = REFLEXIVE.Replace(word, "");
                if (ADJECTIVAL.IsMatch(rv))
                {
                    word = ADJECTIVAL.Replace(word, "");
                }
                else
                {
                    if (VERB.IsMatch(rv))
                    {
                        word = VERB.Replace(word, "");
                    }
                    else
                    {
                        if (NOUN.IsMatch(rv))
                        {
                            word = NOUN.Replace(word, "");
                        }
                    }
                }
            }
        }

        public void Step2(string rv, ref string word)
        {
            word = AND.Replace(word, "");
        }

        public void Step3(ref string word, string r2)
        {
            if (DERIVATIONAL.IsMatch(r2))
            {
                word = DERIVATIONAL.Replace(word, "");
            }
        }

        public void Step4(string rv, ref string word)
        {
            if (NN.IsMatch(rv))
            {
                word = NN.Replace(word, "н");

            }
            else
            {
                if (SUPERLATIVE.IsMatch(rv))
                {
                    word = SUPERLATIVE.Replace(word, "");
                    if (NN.IsMatch(rv))
                    {
                        word = NN.Replace(word, "н");

                    }
                }
                else
                {
                    if (SOFTSIGN.IsMatch(rv))
                    {
                        word = SOFTSIGN.Replace(word, "");

                    }
                }
            }
        }




    }



    class LSA_clusters
    {
        #region variable
        static double[,] frequencyMatrix;
        static int m, n, clustersCount, rowsCount;
        static double[] w;
        static double[,] u, vt, VT;
        static string inputFileName;
        static ArrayList[] clusters;
        static ArrayList HEADERS = new ArrayList();
        static double eps;
        #endregion

        #region pattern 
        static Regex stopSymbolsStart = new Regex("^(еще |него |сказать |а |ж |нее |со |без |же |ней |совсем " +
            "|более |жизнь |нельзя |так |больше |за |нет |такой |будет |зачем |ни |там |будто |здесь |нибудь |тебя" +
            " |бы |и |никогда |тем |был |из |ним |теперь |была |из-за |них |то |были |или |ничего |тогда |было |им |но" +
            " |того |быть |иногда |ну |тоже |в |их |о |только |вам |к |об |том |вас |кажется |один |тот |вдруг |как |он " +
            "|три |ведь |какая |она |тут |во |какой |они |ты |вот |когда |опять |у |впрочем |конечно |от |уж |все " +
            "|которого |перед |уже |всегда |которые |по |хорошо |всего |кто |под |хоть |всех |куда |после |чего |всю |ли " +
            "|потом |человек |вы |лучше |потому |чем |г |между |почти |через |где |меня |при |что |говорил |мне |про " +
            "|чтоб |да |много |раз |чтобы |даже |может |разве |чуть |два |можно |с |эти |для |мой |сам |этого |до |моя " +
            "|свое |этой |другой |мы |свою |этом |его |на |себе |этот |ее |над |себя |эту |ей |надо |сегодня |я |ему " +
            "|наконец |сейчас |если |нас |сказал |есть |не |сказала )", RegexOptions.IgnoreCase);
        static Regex stopSymbolsMiddle = new Regex("( еще | него | сказать | а | ж | нее | со | без | же | ней " +
             "| совсем | более | жизнь | нельзя | так | больше | за | нет | такой | будет | зачем | ни | там | будто " +
             "| здесь | нибудь | тебя | бы | и | никогда | тем | был | из | ним | теперь | была | изза | них | то " +
             "| были | или | ничего | тогда | было | им | но | того | быть | иногда | ну | тоже | в | их | о | только " +
             "| вам | к | об | том | вас | кажется | один | тот | вдруг | как | он | три | ведь | какая | она | тут" +
             " | во | какой | они | ты | вот | когда | опять | у | впрочем | конечно | от | уж | все | которого | перед " +
             "| уже | всегда | которые | по | хорошо | всего | кто | под | хоть | всех | куда | после | чего | всю | ли " +
             "| потом | человек | вы | лучше | потому | чем | г | между | почти | через | где | меня | при | что " +
             "| говорил | мне | про | чтоб | да | много | раз | чтобы | даже | может | разве | чуть | два | можно " +
             "| с | эти | для | мой | сам | этого | до | моя | свое | этой | другой | мы | свою | этом | его | на | себе" +
             " | этот | ее | над | себя | эту | ей | надо | сегодня | я | ему | наконец | сейчас | если | нас | сказал " +
             "| есть | не | сказала )", RegexOptions.IgnoreCase);

        static Regex figures = new Regex("(\\d)");

        static Regex punctuationSymbols = new Regex("(\\W)");

        static char[] vowels = { 'а', 'о', 'у', 'ы', 'э', 'я', 'е', 'ю', 'и', 'А', 'О', 'У', 'Ы', 'Э', 'Я', 'Е', 'Ю', 'И' };
        static char[] consonants = { 'б', 'в', 'г', 'д', 'ж', 'з', 'й', 'к', 'л', 'м', 'н', 'п', 'р', 'с', 'т', 'ф', 'х', 'ц', 'ч', 'ш', 'щ', 'ъ', 'ь' };

        #endregion



        /*Iput from console fucntion*/
        /*Ввод данных в программу*/
        static void Input()
        {
            for (; ; )
            {
                Console.Write("Имя файла с заголовками:   ");//pattern <Disk>:\\<direct>\\<file>
                inputFileName = Console.ReadLine();
                FileInfo fileInf = new FileInfo(inputFileName);
                if (!fileInf.Exists)
                {
                    Console.WriteLine("Ошибка. Неверное имя!");
                    Console.WriteLine("************************************");
                }
                else
                {
                    break;
                }
            }
            Console.Write("Точность: ");
            string temp = Console.ReadLine();//for russian lang use <,> for split
            eps = Convert.ToDouble(temp);
            Console.Write("Количество строк для разложения (наибольшая точность при 2) : ");
            temp = Console.ReadLine();
            rowsCount = int.Parse(temp);//нужны для матрицы VT, write = 2 for VT
            Console.Write("Количество кластеров: ");
            temp = Console.ReadLine();
            clustersCount = int.Parse(temp);
        }

        /*Remouve stop Symbols. Result of it write in output file */
        /*Удаляем стоп-символы. Результат запишем в выходной файл. 
         Если этот файл уже существовал, то он перезапишется. */
        static void RemovingStopSymbols()
        {
            using (StreamReader InputHeaders = new StreamReader(inputFileName))// all line from input file
            {
                //  using (StreamWriter newHeadings = new StreamWriter("D:\\newHeadings.txt"))
                using (StreamWriter NewHeaders = new StreamWriter("C:\\test\\output.txt"))
                {
                    string line;
                    while ((line = InputHeaders.ReadLine()) != null)
                    {
                        HEADERS.Add(line);
                        line = stopSymbolsStart.Replace(line, "");
                        line = figures.Replace(line, "");
                        line = punctuationSymbols.Replace(line, " ");
                        line = stopSymbolsMiddle.Replace(line, " ");
                        NewHeaders.WriteLine(line);//here write new line in new file 
                    }
                }
            }
        }

        /*Check that word is word, not single character*/
        static string RVFinding(string word)
        {
            int position = -1;
            bool check = false;
            for (int i = 0; i < word.Length; i++)
            {
                int j = 0;
                foreach (char ch in vowels)
                {
                    if (word[i] == vowels[j])
                    {
                        position = i;
                        check = true;
                        break;
                    }
                    j++;
                }
                if (check)
                    break;
            }
            if ((position == -1) || (position == word.Length - 1))
                return "";
            else
            {
                string rv = "";
                for (int i = position + 1; i < word.Length; i++)
                    rv += word[i];
                return rv;
            }
        }

        static string R1Finding(string word)
        {
            string r1 = "";
            bool check = false;
            int pos = -1;
            if (word.Length < 2)
                return "";
            for (int i = 0; i < word.Length - 1; i++)
            {
                int f = 0;
                foreach (char c in vowels)
                {
                    if (word[i] == vowels[f])
                    {
                        check = true;
                        break;
                    }
                    f++;
                }
                if (check)
                {
                    f = 0;
                    foreach (char c in consonants)
                    {
                        if (word[i + 1] == consonants[f])
                        {
                            pos = i + 1;
                            break;
                        }
                        f++;
                    }
                }
                if (pos != -1)
                    break;
            }
            if (pos == -1)
                return "";
            for (int i = pos + 1; i < word.Length; i++)
                r1 += word[i];
            return r1;
        }


        /*Stemmer*/
        static void Stemming()
        {
            Porter p = new Porter();
            using (StreamReader newHeadings = new StreamReader("C:\\test\\output.txt"))
            {
                using (StreamWriter newHeadingsAfterStemming = new StreamWriter("C:\\test\\output2.txt"))
                {
                    string line;
                    while ((line = newHeadings.ReadLine()) != null)
                    {
                        int i = 0;
                        string word = "";
                        string rv, r1, r2;
                        while (i < line.Length)
                        {
                            if (line[i] != ' ')
                            {
                                word += line[i];
                            }
                            else
                            {
                                rv = RVFinding(word);
                                if (rv != "")
                                {
                                    r1 = R1Finding(word);
                                    r2 = R1Finding(r1);
                                    p.Step1(rv, ref word);
                                    p.Step2(rv, ref word);
                                    p.Step3(ref word, r2);
                                    p.Step4(rv, ref word);
                                }

                                newHeadingsAfterStemming.Write(word + " ");
                                word = "";
                            }
                            i++;
                        }
                        newHeadingsAfterStemming.WriteLine("");
                    }
                }
            }
        }

        static void CreatingFrequencyMatrix()
        {
            ArrayList multiplyWords = new ArrayList();
            ArrayList lines = new ArrayList();
            string line, tempWord;
            using (StreamReader WithoutStopWordHeaders = new StreamReader("C:\\test\\output2.txt"))
            {
                while ((line = WithoutStopWordHeaders.ReadLine()) != null)
                {
                    lines.Add(line);//добавим из файла все строки
                }
            }
            for (int i = 0; i < lines.Count; i++)
            {
                string temp = (string)lines[i];
                int j = 0;
                string word = "";
                while (j < temp.Length)
                {
                    if (temp[j] != ' ')
                    {
                        word += temp[j];
                    }
                    else
                    {
                        if ((word != " ") && (word != ""))
                        {
                            word += " ";
                            tempWord = word; //добавим найденное слово во временную строку
                            word = word.ToLower();
                            string savingLine = (string)lines[i];
                            temp = temp.ToLower();
                            temp = temp.Replace(word, "");
                            lines[i] = temp;
                            for (int k = 0; k < lines.Count; k++)
                            {
                                string searchString = (string)lines[k];
                                searchString = searchString.ToLower();
                                int pos = searchString.IndexOf(word);
                                if (pos != -1)
                                {
                                    bool check = false;
                                    for (int f = 0; f < multiplyWords.Count; f++)
                                    {
                                        string multiplyWordsToLower = (string)multiplyWords[f];
                                        multiplyWordsToLower = multiplyWordsToLower.ToLower();
                                        if (word == multiplyWordsToLower)
                                        {
                                            check = true;
                                            break;
                                        }
                                    }
                                    if (!check)
                                    {
                                        multiplyWords.Add(tempWord);//добавим слово, если ранее его не было в списке временных слов
                                    }
                                }
                            }
                            lines[i] = savingLine;
                            temp = savingLine;
                            word = "";
                        }
                    }
                    j++;
                }
            }
            multiplyWords.Sort();
            frequencyMatrix = new double[multiplyWords.Count, lines.Count];
            m = multiplyWords.Count; 
            n = lines.Count;
            /*После того, как во временном файле окажутся уникальные слова*/
            /*Выполним их подсчет для каждой строки */
            /*Заполним матрицу*/
            for (int i = 0; i < multiplyWords.Count; i++)
            {
                for (int j = 0; j < lines.Count; j++)
                {
                    frequencyMatrix[i, j] = 0;
                }
            }
            for (int i = 0; i < multiplyWords.Count; i++)
            {
                for (int j = 0; j < lines.Count; j++)
                {
                    tempWord = (string)multiplyWords[i];
                    tempWord = tempWord.ToLower();
                    line = (string)lines[j];
                    line = line.ToLower();
                    int pos = line.IndexOf(tempWord);//слово было найдено в строке
                    if (pos != -1)
                    {
                        frequencyMatrix[i, j]++;//добавим к частоте встречаемости
                    }
                }
            }
        }

        /*Use alglib.dll*/
        static void SVD()
        {
            alglib.rmatrixsvd(frequencyMatrix, m, n, 2, 2, 2, out w, out u, out vt);
            VT = new double[rowsCount, n];
            /*где rowsCount задается при старте программы, а n это количество столбцов*/
            for (int i = 0; i < rowsCount; i++)
            {
                for (int j = 0; j < n; j++)
                {
                    VT[i, j] = vt[i, j];//2demension matrix, rowsCount eneter when app start
                }
            }
        }
        /*Clusters*/
        /*Zero step*/
        static void InitializeCentroid(double[,] centroid)
        {
            for (int i = 0; i < rowsCount; i++)
            {
                for (int j = 0; j < clustersCount; j++)
                {
                    centroid[i, j] = VT[i, j];
                }
            }
        }

        /*First step*/
        static void FindClusters(double[,] centroid1)
        {
            for (int i = 0; i < clustersCount; i++)
                clusters[i].Clear();
            for (int i = 0; i < n; i++)
            {
                double[] distances = new double[clustersCount];
                for (int j = 0; j < clustersCount; j++)
                {
                    distances[j] = 0;
                    for (int k = 0; k < rowsCount; k++)
                    {
                        distances[j] += Math.Pow(centroid1[k, j] - VT[k, i], 2);
                    }
                    distances[j] = Math.Sqrt(distances[j]);

                }
                double min = distances.Min();
                int minIndex = Array.IndexOf(distances, min);
                clusters[minIndex].Add(i);
            }
        }
        /*Second step*/
        static void CopyCentroids(double[,] centroid1, double[,] centroid2)
        {
            for (int i = 0; i < rowsCount; i++)
            {
                for (int j = 0; j < clustersCount; j++)
                {
                    centroid2[i, j] = centroid1[i, j];
                }
            }
        }

        /*Third step*/
        static void CreateNewCentroid(double[,] centroid1)
        {
            for (int i = 0; i < clustersCount; i++)
            {
                foreach (int c in clusters[i])
                {
                    for (var j = 0; j < rowsCount; j++)
                    {
                        centroid1[j, i] += VT[j, c];
                    }
                }
                for (var j = 0; j < rowsCount; j++)
                {
                    if (clusters[i].Count != 0)
                    {
                        centroid1[j, i] /= clusters[i].Count;//пересчет центра кластеров
                    }
                }
            }
        }
    /*If >eps*/
        static double MaxChanging(double[,] centroid1, double[,] centroid2)
            {
                double[] changing = new double[clustersCount];
                for (int i = 0; i < clustersCount; i++)
                {
                    for (int j = 0; j < rowsCount; j++)
                    {
                        changing[i] += Math.Pow(centroid1[j, i] - centroid2[j, i], 2);//определение меры близости между центрами
                    }

                    changing[i] = Math.Sqrt(changing[i]);//вычисление Евклидового расстояния
                }
                return changing.Max();
            }

        static void Clustering()
        {
            double[,] centroid1 = new double[rowsCount, clustersCount];
            double[,] centroid2 = new double[rowsCount, clustersCount];
            InitializeCentroid(centroid1);//инициализируем класстер значениями из VT
            clusters = new ArrayList[clustersCount];//количество кластеров задается при вводе
            for (int i = 0; i < clustersCount; i++)
            {
                clusters[i] = new ArrayList();
            }
            FindClusters(centroid1); 
            CopyCentroids(centroid1, centroid2);
            CreateNewCentroid(centroid1);
            while (MaxChanging(centroid1, centroid2) > eps) 
            {
                FindClusters(centroid1);
                CopyCentroids(centroid1, centroid2);
                CreateNewCentroid(centroid1);
            }
        }

        static void Output()
        {
            using (StreamWriter result = new StreamWriter("C:\\test\\result.txt"))
            {
                for (int i = 0; i < clustersCount; i++)
                {
                    result.WriteLine("Область  {0}", i + 1);
                    result.WriteLine("-----------------------", i + 1);
                    foreach (int doc in clusters[i])
                    {
                        result.WriteLine(HEADERS[doc]);//запись заголовка под номером
                    }
                    result.WriteLine("-----------------------");
                    result.WriteLine();
                }
            }
        }
     
        static void Main(string[] args)
        {
            Input();//enter filename, count of clusters, etc.
            RemovingStopSymbols();//delete stop symbols
            Stemming();
            CreatingFrequencyMatrix();
            SVD(); //singular var deprication
            Clustering();
            Output();
        }
       
    }
}