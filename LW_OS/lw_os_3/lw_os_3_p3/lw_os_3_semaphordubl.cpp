/*��������� ��������� ����� ����������. 
���� ���������� �� ������. 
��������� ������� �� 10. ��������� ����������, ����� 3 ���� �� 10
������ ���� ��� ������������� - ������� (������������)*/
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

	if (1 == argc) // (!) ����� ������������� ��������
	{
		int n_processes;
		std::cout << "������� ���������� ���������: ";
		std::cin >> n_processes;
		std::cout << std::endl;
		HANDLE hObject = NULL;

		TCHAR commandline[MAX_PATH + 40];
		LPCTSTR  sinchtype = TEXT("%s semaphore-duplicate %d %p");
		/*���������� ������� ���� � ������������� ������������� �������� ��������� � �������� ������� ��� ��������� � ��������� ������*/
	
		hObject = CreateSemaphore(NULL, 2, 2, TEXT("Semaphor"));

		if (hObject != NULL)
		{
			/*��������� ��������� ������ ��� �������� �������� ���������*/
			//std::wstring commandline = sinchtype + *argv[0] + (int)GetCurrentProcessId() + hObject;
			StringCchPrintf(commandline, _countof(commandline), sinchtype, argv[0], (int)GetCurrentProcessId(), hObject);
		} 

		if (hObject != NULL)
		{
			// ������� ������ ���������
			HANDLE* ProcArray = new HANDLE[n_processes];

			STARTUPINFO si = { sizeof(STARTUPINFO) };
			PROCESS_INFORMATION procinfo = { 0 };

			for (int i = 0; i < n_processes; ++i)
			{
				// ��������� ����� �������
				BOOL bRet = CreateProcess(NULL, commandline, NULL, NULL, TRUE, CREATE_NEW_CONSOLE, NULL, NULL, &si, &procinfo);
				std::cout << "PID: argc"<<argc << argv << std::endl;
				if (FALSE != bRet)
				{				
					ProcArray[i] = procinfo.hProcess;/*�������� ���������� � ������ � ������� ���������� ������*/
					std::cout << "PID: argc argv" << argc << argv << std::endl;
					CloseHandle(procinfo.hThread);
				} 
				else
				{
					ProcArray[i] = NULL;
				} 
			} 

			WaitForMultipleObjects(n_processes, ProcArray, TRUE, INFINITE);// ������� ���������� ���� �������� ���������

			for (int i = 0; i < n_processes; ++i) 
				CloseHandle(ProcArray[i]);
		
			delete[] ProcArray;
		
			CloseHandle(hObject);// ��������� ����������
		} 
	} 
	else if (argc > 1) // ����� ��������� ��������
	{
		std::cout << "��������� ��������� ������:" << std::endl;
		std::cout << "PID: argc" << argc <<" "<< argv[1] << std::endl;
		HANDLE hSemaphore = NULL; // ���������� ��������
		if ((4 == argc) && (lstrcmpi(argv[1], TEXT("semaphore-duplicate")) == 0))
			{
				UINT parentPID = (UINT)_ttoi(argv[2]);
				
				HANDLE hObject = (HANDLE)_tcstoui64(argv[3], NULL, 16);//�������� ����������� �������� � 16-� �������

				HANDLE hProcessChild = OpenProcess(PROCESS_DUP_HANDLE, FALSE, parentPID);// ��������� �������
				if (NULL != hProcessChild)
				{
					DuplicateHandle(hProcessChild, hObject, GetCurrentProcess(), &hSemaphore, SEMAPHORE_ALL_ACCESS, FALSE, 0);// ��������� ���������� ����������
					CloseHandle(hProcessChild);// ��������� ���������� ��������
				}			
			} 

			if (NULL != hSemaphore)
			{
				std::cout<<"> ������� ��� ��������:"<<std::endl;

				for (int i = 0; i < 3; ++i)
				{
					WaitForSingleObject(hSemaphore, INFINITE);//������ ���� ����������� �������
					int j;
					for (j = 0; j < 10; ++j)
					{
						std::cout << j + 1;
						Sleep(500);	//�������� ������ ����			
					} 
					if (j == 10) std::cout << std::endl;
					ReleaseSemaphore(hSemaphore, 1, NULL);// ����� ����������� �������� �������� �������� ����� �������� 
				} 
				
				CloseHandle(hSemaphore);// ��������� ���������� ��������
			} 
	}
} 
 

