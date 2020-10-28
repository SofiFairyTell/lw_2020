/*Порождает несколько своих экзмпляров. 
Ждет завершение их работы. 
Экзепляры считают до 10. Завершить приложение, когда 3 раза по 10
Объект ядра для синхронизации - Семафор (дублирование)*/
#include <Windows.h>
#include <tchar.h>
#include <strsafe.h>
#include <stdio.h>
#include <conio.h>
#include <locale.h>
#include <iostream>

// фунrция, дублирующая дескриптор
BOOL DuplicateHandle(DWORD dwProcessId, HANDLE hSourceHandle, LPHANDLE lpTargetHandle, DWORD dwDesiredAccess);

// ------------------------------------------------------------------------------------------------
int _tmain(int argc, LPCTSTR argv[])
{
	setlocale(LC_ALL, "");

	if (1 == argc) // (!) ветка родительского процесса
	{
		int n_processes;
		std::cout << "Введите количество процессов: ";
		std::cin >> n_processes;
		std::cout << std::endl;
		HANDLE hObject = NULL;

		TCHAR szCmdLine[MAX_PATH + 40];
		/*дескриптор объекта ядра и идентификатор родительского процесса перадются в дочерний процесс как аргументы в командной строке*/
	
		hObject = CreateSemaphore(NULL, 1, 2, TEXT("Semaphor"));

		if (hObject != NULL)
		{
			// формируем командную строку для создания дочерних процессов
			StringCchPrintf(szCmdLine, _countof(szCmdLine), TEXT("%s semaphore-duplicate %d %p"), argv[0], (int)GetCurrentProcessId(), hObject);
		} // if

		if (hObject != NULL)
		{
			// создаем массив процессов
			HANDLE* ProcArray = new HANDLE[n_processes];

			STARTUPINFO si = { sizeof(STARTUPINFO) };
			PROCESS_INFORMATION procinfo = { 0 };

			for (int i = 0; i < n_processes; ++i)
			{
				// порождаем новый процесс
				BOOL bRet = CreateProcess(NULL, szCmdLine, NULL, NULL, TRUE, CREATE_NEW_CONSOLE, NULL, NULL, &si, &procinfo);

				if (FALSE != bRet)
				{
					/*записать дескриптор в массив и закрыть дескриптор потока*/
					ProcArray[i] = procinfo.hProcess;
					CloseHandle(procinfo.hThread);
				} 
				else
				{
					ProcArray[i] = NULL;
				} 
			} 
			
			WaitForMultipleObjects(n_processes, ProcArray, TRUE, INFINITE);// ожидаем завершения всех дочерних процессов

			for (int i = 0; i < n_processes; ++i) 
				CloseHandle(ProcArray[i]);
		
			delete[] ProcArray;
		
			CloseHandle(hObject);// закрываем дескриптор
		} 
	} 
	else if (argc > 1) // (!) ветка дочернего процесса
	{
		std::cout << "Аргументы командной строки:" << std::endl;
		HANDLE hSemaphore = NULL; // дескриптор семафора
		if ((4 == argc) && (_tcsicmp(argv[1], TEXT("semaphore-duplicate")) == 0))
			{
				// получаем из командной строки идентификатор родительского процесса
				UINT parentPID = (UINT)_ttoi(argv[2]);
				
				HANDLE hObject = (HANDLE)_tcstoui64(argv[3], NULL, 16);//значение дескриптора семафора в 16-м формате
			
				DuplicateHandle(parentPID, hObject, &hSemaphore, SEMAPHORE_ALL_ACCESS);// дублируем полученный дескриптор семафора
			} 

			if (NULL != hSemaphore)
			{
				std::cout<<"> Счет (семафор):"<<std::endl;

				for (int i = 0; i < 3; ++i)
				{
					WaitForSingleObject(hSemaphore, INFINITE);//жждать пока освободится семафор
					int j;
					for (j = 0; j < 10; ++j)
					{
						std::cout << j + 1;
						Sleep(500);	//задержка вывода цифр			
					} 
					if (j == 10) std::cout << std::endl;
					ReleaseSemaphore(hSemaphore, 1, NULL);// Поток увеличивает значение счетчика текущего числа ресурсов 
				} 
				
				CloseHandle(hSemaphore);// закрываем дескриптор семафора
			} // if
	}
} 
 
// ------------------------------------------------------------------------------------------------
BOOL DuplicateHandle(DWORD dwProcessId, HANDLE hSourceHandle, LPHANDLE lpTargetHandle, DWORD dwDesiredAccess)
{
	BOOL bRet = FALSE;

	// открываем процесс
	HANDLE hProcess = OpenProcess(PROCESS_DUP_HANDLE, FALSE, dwProcessId);

	if (NULL != hProcess)
	{
		// дублируем полученный дескриптор
		bRet = DuplicateHandle(hProcess, hSourceHandle, GetCurrentProcess(), lpTargetHandle, dwDesiredAccess, FALSE, 0);
		// закрываем дескриптор процесса
		CloseHandle(hProcess);
	} // if

	return bRet;
} // DuplicateHandle
