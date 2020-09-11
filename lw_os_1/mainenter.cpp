#include <Windows.h>
#include <wchar.h>
#include <locale.h>
#include <stdio.h>
#include <ShlObj.h>
#include <STATILIBR.h>
#include <DYNLIB1.h>

#pragma comment (lib,"STATILIBR.lib")
#pragma comment (lib,"DYNLIB1.lib") // не€вное подключение к динам.библиотеке

int wmain()
{
	_wsetlocale(LC_ALL, TEXT(""));
	wprintf(TEXT("¬ывод данных статической библиотеки\n"));
	PrintCompName();
	PrintDNSName();
	PrintUserName();
	PrintUserNameExtended();
	wprintf(TEXT("ѕути к системным каталогам\n"));
	const long csdir[] =
	{
		CSIDL_DESKTOP,
		CSIDL_INTERNET,
		CSIDL_PROGRAMS,
		CSIDL_PERSONAL


	};//виртуальный каталог рабочего стола
	PrintSysDir(csdir, _countof(csdir)); 

}
