#include <Windows.h>
#include <tchar.h>
#include <stdio.h>
#include <conio.h>
#include <locale.h>
#include <time.h>
#include <process.h>
#include <iostream>
// функци€ потока дл€ строки матрицы (используютс€ критические секции)
unsigned __stdcall ThreadFuncWithCriticalSection(void *lpParameter);

int rows, cols, value; // строки, строки, значение

long total = 0; // количество двузначных чисел
int max = 0, min = 999; // максимальное и минимальное значени€

CRITICAL_SECTION cs; // критическа€ секци€

void matrix()
{
	do
	{
		std::cout << "¬ведите количество строк: ";
		std::cin >> rows;
		if (rows < 2)
		{
			std::cout << "—лишком мало!" << "\n";
		}
	} while (rows < 2);
	do
	{
		std::cout << "¬ведите количество столбцов : ";
		std::cin >> cols;
		if (cols < 2)
		{
			std::cout << "—лишком мало!" << "\n";
		}
	} while (cols < 2);

	/*—оздание матрицы с числами от 0 до 999*/
	srand(time(NULL)); // »нициализируем генератор случайных чисел. 
	int** array2D = new int*[rows];
	for (int i = 0; i < rows; ++i)
	{
		array2D[i] = new int[cols];
		for (int j = 0; j < cols; ++j)
		{
			array2D[i][j] = rand() % 1000; // генерируем случайное число от 0 до 999;
			std::cout << array2D[i][j] << "\t";
		}
		std::cout << std::endl;
	}
	std::cout << std::endl;

	// создаЄм массив потоков дл€ строк матрицы
	HANDLE* threads = new HANDLE[rows];
	InitializeCriticalSection(&cs); // инициализируем критическию секцию
	for (int i = 0; i < rows; ++i)
	{
		//_beginthreadex потому что CreateThread and ExitThread не учитывают возникновение некоторых проблем 
		threads[i] = (HANDLE)_beginthreadex(NULL, 0, ThreadFuncWithCriticalSection, (void *)array2D[i], 0, NULL);
	}

	WaitForMultipleObjects(rows, threads, TRUE, INFINITE);// ожидание завершени€ всех созданных потоков


	std::cout << " оличество двузначных чисел: " << total << std::endl
		<< "ћаксимальный элемент: " << max << std::endl
		<< "ћинимальный элемент: " << min << std::endl;


	for (int i = 0; i < rows; ++i)
	{
		delete[] array2D[i];			// удал€ем строку матрицы	
		CloseHandle(threads[i]);// закрываем дискриптор созданного потока
	}

	delete[] threads;// удал€ем массив потоков
	delete[] array2D;// удал€ем матрицу
	DeleteCriticalSection(&cs); // удал€ем критическию секцию
}
unsigned __stdcall ThreadFuncWithCriticalSection(void *lpParameter)
{
	const int *row = (const int *)lpParameter; // указатель на строку матрицы

	unsigned int count = 0; // количество двузначных чисел в строке матрицы
	int Max = 0, Min = 0; // индексы максимального и минимального элемента в строке матрицы

	for (int j = 0; j < cols; ++j)
	{
		if (row[j] > row[Max])
		{	Max = j;	}// запомним индекс максимального значени€
		else 
			if (row[j] < row[Min])
			{	Min = j;	} // запомним индекс минимального значени€
		if (row[j] > 9 && row[j] < 100) // двузначное число
		{
			++count;// увеличим на единицу количество двузначных чисел
		}

	}
	
	//if (TryEnterCriticalSection(&cs) != FALSE)
	//{
		EnterCriticalSection(&cs);
		total += count; // увеличиваем количество двузначных чисел
		if (row[Max] > max) max = row[Max]; // определ€ем максимальный элемент
		if (row[Min] < min) min = row[Min]; // определ€ем минимальный элемент
		LeaveCriticalSection(&cs); // покидаем критическую секцию

	//}
	return 0;
} 

int _tmain()
{
	int answer;
	setlocale(LC_ALL, "");
	do
	{
		std::cout << "ƒл€ завершени€ работы программы нажмите 0 иначе 1\n";
		std::cin >> answer;
		if (answer == 1)
			matrix();
	} while (answer != 0);
	
}

