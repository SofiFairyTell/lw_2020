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

		TCHAR commandline[MAX_PATH + 40];
		LPCTSTR  sinchtype = TEXT("%s semaphore-duplicate %d %p");
		/*дескриптор объекта ядра и идентификатор родительского процесса перадются в дочерний процесс как аргументы в командной строке*/
	
		hObject = CreateSemaphore(NULL, 1, 2, TEXT("Semaphor"));

		if (hObject != NULL)
		{
			/*формируем командную строку для создания дочерних процессов*/
			//std::wstring commandline = sinchtype + *argv[0] + (int)GetCurrentProcessId() + hObject;
			StringCchPrintf(commandline, _countof(commandline), sinchtype, argv[0], (int)GetCurrentProcessId(), hObject);
		} 

		if (hObject != NULL)
		{
			// создаем массив процессов
			HANDLE* ProcArray = new HANDLE[n_processes];

			STARTUPINFO si = { sizeof(STARTUPINFO) };
			PROCESS_INFORMATION procinfo = { 0 };

			for (int i = 0; i < n_processes; ++i)
			{
				// порождаем новый процесс
				BOOL bRet = CreateProcess(NULL, commandline, NULL, NULL, TRUE, CREATE_NEW_CONSOLE, NULL, NULL, &si, &procinfo);
				std::cout << "PID: argc"<<argc << argv << std::endl;
				if (FALSE != bRet)
				{				
					ProcArray[i] = procinfo.hProcess;/*записать дескриптор в массив и закрыть дескриптор потока*/
					std::cout << "PID: argc argv" << argc << argv << std::endl;
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
	else if (argc > 1) // ветка дочернего процесса
	{
		std::cout << "Аргументы командной строки:" << std::endl;
		std::cout << "PID: argc" << argc <<" "<< argv[1] << std::endl;
		HANDLE hSemaphore = NULL; // дескриптор семафора
		if ((4 == argc) && (lstrcmpi(argv[1], TEXT("semaphore-duplicate")) == 0))
			{
				UINT parentPID = (UINT)_ttoi(argv[2]);
				
				HANDLE hObject = (HANDLE)_tcstoui64(argv[3], NULL, 16);//значение дескриптора семафора в 16-м формате

				HANDLE hProcessChild = OpenProcess(PROCESS_DUP_HANDLE, FALSE, parentPID);// открываем процесс
				if (NULL != hProcessChild)
				{
					DuplicateHandle(hProcessChild, hObject, GetCurrentProcess(), &hSemaphore, SEMAPHORE_ALL_ACCESS, FALSE, 0);// дублируем полученный дескриптор
					CloseHandle(hProcessChild);// закрываем дескриптор процесса
				}			
			} 

			if (NULL != hSemaphore)
			{
				std::cout<<"> Счетчик при семафоре:"<<std::endl;

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
			} 
	}
} 
 

