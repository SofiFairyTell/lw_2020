#include <Windows.h>
#include <wchar.h>
#include <locale.h>
#include <stdio.h>

int wmain()
{
	_wsetlocale(LC_ALL, TEXT(""));
	wprintf(TEXT("¬ывод данных статической библиотеки"));
	//_wsetlocale(LC_ALL, );
}
