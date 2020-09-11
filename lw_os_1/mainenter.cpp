#include <Windows.h>
#include <wchar.h>
#include <locale.h>
#include <stdio.h>
#include <ShlObj.h>
#include <STATILIBR.h>
#include <DYNLIB1.h>

#pragma comment (lib,"STATILIBR.lib")
#pragma comment (lib,"DYNLIB1.lib") // неявное подключение к динам.библиотеке

int wmain()
{
	_wsetlocale(LC_ALL, TEXT(""));
	wprintf(TEXT("\nВывод данных статической библиотеки\n\n"));
	PrintCompName();
	PrintDNSName();
	PrintUserName();
	PrintUserNameExtended();
	wprintf(TEXT("\nПути к системным каталогам\n\n"));
	const long csdir[] =
	{
		CSIDL_DESKTOP,
		CSIDL_PROGRAMS,
		CSIDL_PERSONAL,
		CSIDL_STARTUP,
		CSIDL_SYSTEM,
		CSIDL_HISTORY
	};//список идентификаторов системных папок
	PrintSysDir(csdir, _countof(csdir)); 
	wprintf(TEXT("\nВерсия операционной системы\n\n"));
	PrintOSinfo();
}
