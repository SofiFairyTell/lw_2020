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
		// значение дескриптора объекта ядра и идентификатор родительского процесса
		// будем передавать в дочерний процесс как аргументы в командной строке

		hObject = CreateSemaphoreEx(NULL, 1, 1, NULL, 0, SEMAPHORE_ALL_ACCESS);
		if (hObject != NULL)
		{
			// формируем командную строку для создания дочерних процессов
			StringCchPrintf(szCmdLine, _countof(szCmdLine), TEXT("%s semaphore-duplicate %d %p"), argv[0], (int)GetCurrentProcessId(), hObject);
		} // if

		if (hObject != NULL)
		{
			// создаем массив процессов
			HANDLE* processes = new HANDLE[n_processes];

			// порождаем дочерние процессы...

			STARTUPINFO si = { sizeof(STARTUPINFO) };
			PROCESS_INFORMATION pi = { 0 };

			for (int i = 0; i < n_processes; ++i)
			{
				// порождаем новый процесс
				BOOL bRet = CreateProcess(NULL, szCmdLine, NULL, NULL, TRUE, CREATE_NEW_CONSOLE, NULL, NULL, &si, &pi);

				if (FALSE != bRet)
				{
					// записываем дескриптор нового процесса в массив
					processes[i] = pi.hProcess;
					// закрывает дескриптор потока нового процесса
					CloseHandle(pi.hThread);
				} // if
				else
				{
					processes[i] = NULL;
				} // else
			} // for

			// ожидаем завершения всех дочерних процессов
			WaitForMultipleObjects(n_processes, processes, TRUE, INFINITE);

			// закрываем дескрипторы дочерних процессов
			for (int i = 0; i < n_processes; ++i) CloseHandle(processes[i]);
		
			delete[] processes;	// удаляем массив процессов
		
			CloseHandle(hObject);// закрываем дескриптор
		} // if
	} // if
	else if (argc > 1) // (!) ветка дочернего процесса
	{
		std::cout << "Аргументы командной строки:" << std::endl;
		if(_tcsnicmp(argv[1], TEXT("semaphore-"), 10) == 0)
		{
			HANDLE hSemaphore = NULL; // дескриптор семафора
			if ((4 == argc) && (_tcsicmp(argv[1], TEXT("semaphore-duplicate")) == 0))
			{
				// получаем из командной строки идентификатор родительского процесса
				DWORD dwProcessId = (DWORD)_ttoi(argv[2]);
				// получаем из командной строки дескриптор семафора
				HANDLE hObject = (HANDLE)_tcstoui64(argv[3], NULL, 16);

				// дублируем полученный дескриптор семафора
				DuplicateHandle(dwProcessId, hObject, &hSemaphore, SEMAPHORE_ALL_ACCESS);
			} 

			if (NULL != hSemaphore)
			{
				_tprintf(TEXT("> \n"));
				_tprintf(TEXT("> Счет (семафор):\n"));

				for (int i = 0; i < 3; ++i)
				{
					// ожидаем освобождения семафора,
					// а затем захватываем его
					WaitForSingleObject(hSemaphore, INFINITE);

					for (int j = 0; j < 10; ++j)
					{
						_tprintf(TEXT("> %d\n"), j + 1);
						Sleep(500);
					} // for

					ReleaseSemaphore(hSemaphore, 1, NULL);// Поток увеличивает значение счетчика текущего числа ресурсов
				} // for

				// закрываем дескриптор семафора
				CloseHandle(hSemaphore);
			} // if
		} // if
	}
	
	} // _tmain
 
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
