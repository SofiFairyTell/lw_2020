#include <Windows.h>
#include <wchar.h>
#include <locale.h>
#include <stdio.h>
#include <delayimp.h> //��� ������ ������� __FUnloadDelayLoadedDLL2(LPCSTR szDLL)
#include <ShlObj.h>
#include <STATILIBR.h>
#include <DYNLIB1.h>
#include <DYNLIB2.h>
#include <DYNLIB3.h>

#pragma comment (lib,"STATILIBR.lib")
#pragma comment (lib,"DYNLIB1.lib") // ������� ����������� � �����.����������

int wmain()
{
	_wsetlocale(LC_ALL, TEXT(""));
	wprintf(TEXT("\n1. ����� ������ ����������� ����������\n"));
	PrintCompName();
	PrintDNSName();
	PrintUserName();
	PrintUserNameExtended();
	wprintf(TEXT("\n\n2. ����� ������ ������������ ����������\n"));
	wprintf(TEXT("\n���� � ��������� ���������\n"));
	const long csdir[] =
	{
		CSIDL_APPDATA,
		CSIDL_COMMON_APPDATA,
		CSIDL_COMMON_DOCUMENTS,
		CSIDL_HISTORY,
		CSIDL_INTERNET_CACHE,
		CSIDL_LOCAL_APPDATA,
		CSIDL_PERSONAL,
		CSIDL_PROGRAMS,
		CSIDL_PROGRAM_FILES,
		CSIDL_PROGRAM_FILES_COMMON,
		CSIDL_SYSTEM,
		CSIDL_WINDOWS,
		CSIDL_DESKTOP,
		CSIDL_STARTUP	
	};//������ ����� ��������������� ��������� �����
	PrintSysDir(csdir, _countof(csdir)); 
	wprintf(TEXT("\n������ ������������ �������\n\n"));
	PrintOSinfo();
	wprintf(TEXT("\n������� ���� � �����\n\n"));
	TimeDateInfo(LOCALE_NAME_INVARIANT, TIME_NOTIMEMARKER, L"\tdd-MM-yyyy", L"\thh:mm:ss tt");

	HMODULE Hdll = LoadLibrary(TEXT("DYNLIB2.dll"));//����� ����������� � ����������

	if (NULL != Hdll)
	{
		PRINT_SYSMETR_PROC PrintSYSmetr = (PRINT_SYSMETR_PROC)GetProcAddress(Hdll, "PrintSYSmetr");
		if (NULL != PrintSYSmetr)
		{
			PrintSYSmetr(L"SM_CXEDGE", SM_CXEDGE);
			PrintSYSmetr(L"SM_CYEDGE", SM_CYEDGE);
			PrintSYSmetr(L"SM_CXMINSPACING", SM_CXMINSPACING);
			PrintSYSmetr(L"SM_CYMINSPACING", SM_CYMINSPACING);
			PrintSYSmetr(L"SM_SHOWSOUNDS", SM_SHOWSOUNDS);
		}
		else
		{
			wprintf(TEXT("������� �� ������� %d"),GetLastError());
		}
		FreeLibrary(Hdll);//��� �������� ���������� �� ��������� ������������
	}
	else
	{
		wprintf(TEXT("������� �� ������� %d \n"), GetLastError());
	}
	__try
	{
		/*��������� ��������� ���� ��� ����� �������� ������.
		��������� ������������� ���������� ������������ �� ����
		�������� ������ � ����������, �������������� �� ��������
		���� ��� � ���������� ����������� ��� �������� ����*/
		PrintSYSparamInfo(L"\nSPI_GETMOUSEWHEELROUTING", SPI_GETMOUSEWHEELROUTING);
		PrintSYSparamInfo(L"\nSPI_GETWORKAREA", SPI_GETWORKAREA); //����������� ������� �������� �����
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		switch (GetLastError())
		{
		case ERROR_MOD_NOT_FOUND:
			wprintf(TEXT(":%d DYNLIB3.dll"), ERROR_MOD_NOT_FOUND);
			break;
		case ERROR_PROC_NOT_FOUND:
			wprintf(TEXT(":%d PrintSYSparamInfo"), ERROR_PROC_NOT_FOUND);
			break;
		}
	}
	__FUnloadDelayLoadedDLL2("DYNLIB3.dll"); //��� �������� ��������� ������������ DLL
	system("pause");
}
