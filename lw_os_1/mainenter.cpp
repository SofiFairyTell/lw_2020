#include <Windows.h>
#include <wchar.h>
#include <locale.h>
#include <stdio.h>
#include <delayimp.h>
#include <ShlObj.h>
#include <STATILIBR.h>
#include <DYNLIB1.h>
#include <DYNLIB2.h>
#include <DYNLIB3.h>

#pragma comment (lib,"STATILIBR.lib")
#pragma comment (lib,"DYNLIB1.lib") // неявное подключение к динам.библиотеке

int wmain()
{
	_wsetlocale(LC_ALL, TEXT(""));
	wprintf(TEXT("\n1. Вывод данных статической библиотеки\n"));
	PrintCompName();
	PrintDNSName();
	PrintUserName();
	PrintUserNameExtended();
	wprintf(TEXT("\n\n2. Вывод данных динамической библиотеки\n"));
	wprintf(TEXT("\nПути к системным каталогам\n"));
	const long csdir[] =
	{
		CSIDL_DESKTOP,
		CSIDL_PROGRAMS,
		CSIDL_PERSONAL,
		CSIDL_STARTUP,
		CSIDL_SYSTEM,
		CSIDL_HISTORY
	};//список части идентификаторов системных папок
	PrintSysDir(csdir, _countof(csdir)); 
	wprintf(TEXT("\nВерсия операционной системы\n\n"));
	PrintOSinfo();
	wprintf(TEXT("\nТекущая дата и время\n\n"));
	TimeDateInfo(LOCALE_NAME_INVARIANT, TIME_NOTIMEMARKER, L"\tdd-MM-yyyy", L"\thh:mm:ss tt");

	HMODULE Hdll = LoadLibrary(TEXT("DYNLIB2.dll"));

	if (NULL != Hdll)
	{
		PRINT_SYSMETR_PROC PrintSYSmetr = (PRINT_SYSMETR_PROC)GetProcAddress(Hdll, "PrintSYSmetr");
		if (NULL != PrintSYSmetr)
		{
			PrintSYSmetr(L"SM_CXEDGE", SM_CXEDGE);
		}
		else
		{
			wprintf(TEXT("Функция не найдена %d"),GetLastError());
		}
		FreeLibrary(Hdll);
	}
	else
	{
		wprintf(TEXT("Функция не найдена %d \n"), GetLastError());
	}
	__try
	{
		PrintSYSparamInfo(L"\nSPI_GETMOUSEWHEELROUTING", SPI_GETMOUSEWHEELROUTING);
		PrintSYSparamInfo(L"\nSPI_GETWORKAREA", SPI_GETWORKAREA); //определение размера рабочего стола
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
	__FUnloadDelayLoadedDLL2("DYNLIB3.dll");
	system("pause");
}
