#include <Windows.h>
#include <tchar.h>
#include <stdio.h>
#include <conio.h>
#include <locale.h>
#include <time.h>
#include <process.h>
#include <iostream>
// ������� ������ ��� ������ ������� (������������ ����������� ������)
unsigned __stdcall ThreadFuncWithCriticalSection(void *lpParameter);

int rows, cols, value; // ������, ������, ��������

long total = 0; // ���������� ���������� �����
int max = 0, min = 999; // ������������ � ����������� ��������

CRITICAL_SECTION cs; // ����������� ������

void matrix()
{
	do
	{
		std::cout << "������� ���������� �����: ";
		std::cin >> rows;
		if (rows < 2)
		{
			std::cout << "������� ����!" << "\n";
		}
	} while (rows < 2);
	do
	{
		std::cout << "������� ���������� �������� : ";
		std::cin >> cols;
		if (cols < 2)
		{
			std::cout << "������� ����!" << "\n";
		}
	} while (cols < 2);

	/*�������� ������� � ������� �� 0 �� 999*/
	srand(time(NULL)); // �������������� ��������� ��������� �����. 
	int** array2D = new int*[rows];
	for (int i = 0; i < rows; ++i)
	{
		array2D[i] = new int[cols];
		for (int j = 0; j < cols; ++j)
		{
			array2D[i][j] = rand() % 1000; // ���������� ��������� ����� �� 0 �� 999;
			std::cout << array2D[i][j] << "\t";
		}
		std::cout << std::endl;
	}
	std::cout << std::endl;

	// ������ ������ ������� ��� ����� �������
	HANDLE* threads = new HANDLE[rows];
	InitializeCriticalSection(&cs); // �������������� ����������� ������
	for (int i = 0; i < rows; ++i)
	{
		//_beginthreadex ������ ��� CreateThread and ExitThread �� ��������� ������������� ��������� ������� 
		threads[i] = (HANDLE)_beginthreadex(NULL, 0, ThreadFuncWithCriticalSection, (void *)array2D[i], 0, NULL);
	}

	WaitForMultipleObjects(rows, threads, TRUE, INFINITE);// �������� ���������� ���� ��������� �������


	std::cout << "���������� ���������� �����: " << total << std::endl
		<< "������������ �������: " << max << std::endl
		<< "����������� �������: " << min << std::endl;


	for (int i = 0; i < rows; ++i)
	{
		delete[] array2D[i];			// ������� ������ �������	
		CloseHandle(threads[i]);// ��������� ���������� ���������� ������
	}

	delete[] threads;// ������� ������ �������
	delete[] array2D;// ������� �������
	DeleteCriticalSection(&cs); // ������� ����������� ������
}
unsigned __stdcall ThreadFuncWithCriticalSection(void *lpParameter)
{
	const int *row = (const int *)lpParameter; // ��������� �� ������ �������

	unsigned int count = 0; // ���������� ���������� ����� � ������ �������
	int Max = 0, Min = 0; // ������� ������������� � ������������ �������� � ������ �������

	for (int j = 0; j < cols; ++j)
	{
		if (row[j] > row[Max])
		{	Max = j;	}// �������� ������ ������������� ��������
		else 
			if (row[j] < row[Min])
			{	Min = j;	} // �������� ������ ������������ ��������
		if (row[j] > 9 && row[j] < 100) // ���������� �����
		{
			++count;// �������� �� ������� ���������� ���������� �����
		}

	}
	
	//if (TryEnterCriticalSection(&cs) != FALSE)
	//{
		EnterCriticalSection(&cs);
		total += count; // ����������� ���������� ���������� �����
		if (row[Max] > max) max = row[Max]; // ���������� ������������ �������
		if (row[Min] < min) min = row[Min]; // ���������� ����������� �������
		LeaveCriticalSection(&cs); // �������� ����������� ������

	//}
	return 0;
} 

int _tmain()
{
	int answer;
	setlocale(LC_ALL, "");
	do
	{
		std::cout << "��� ���������� ������ ��������� ������� 0 ����� 1\n";
		std::cin >> answer;
		if (answer == 1)
			matrix();
	} while (answer != 0);
	
}

