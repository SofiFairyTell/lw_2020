#include <Windows.h>
#include <wchar.h>
#include <locale.h>
#include <stdio.h>
#include <C:\Users\Kurbatova\source\LW2020\lw_os_1\STATILIBR\STATILIBR.h>

int wmain()
{
	_wsetlocale(LC_ALL, TEXT(""));
	wprintf(TEXT("¬ывод данных статической библиотеки"));
	//_wsetlocale(LC_ALL, );
	PrintCompName();
	PrintDNSName();
	PrintUserName();
	PrintUserNameExtended();
}