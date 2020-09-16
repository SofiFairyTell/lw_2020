
#include <Windows.h>
#include <stdio.h>
#include <wchar.h>
#include <locale.h>
#include "DYNLIB2.h"
#define DYNLIB2_PROTOTYPES

//точка входа в dll при загрузке в адресное пространство запущенного приложения
BOOL WINAPI DllMain(HINSTANCE hInstDll, DWORD dwReason, LPVOID LpvResearved)
{
	return TRUE;
}

DYNLIB2_API void PrintSYSmetr(LPCWSTR LpDisplayNam, int index)
{
	int value = GetSystemMetrics(index);
	wprintf(L"Определеяет ширину трехмерной границы");
	switch (index)
	{
	case SM_CXEDGE:
		wprintf(L"Определеяет ширину трехмерной границы");
		wprintf(L"%s:%d px\n", LpDisplayNam, value);
		break;
	case SM_CYEDGE:
		wprintf(L"Определеяет ширину трехмерной границы");
		wprintf(L"%s:%d px\n", LpDisplayNam, value);
		break;
	case SM_CXMINSPACING:
	case SM_CYMINSPACING:
		wprintf(L"Определеяет размеры ячейки сетки для свернутых окон");
		wprintf(L"%s:%d px\n", LpDisplayNam, value);
		break;
	case SM_SHOWSOUNDS:
		wprintf(L"Ненулевое значение в случае вызова приложения с информацией представленной только в звуковом формате");
		wprintf(L"%s:%d px\n",LpDisplayNam,value);
				break;
	}
		
}
