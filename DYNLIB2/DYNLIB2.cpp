#include <WinUser.h>
#include <Windows.h>
#include <stdio.h>
#include <wchar.h>
#include <locale.h>

//точка входа в dll при загрузке в адресное пространство запущенного приложения
BOOL WINAPI DllMain(HINSTANCE hInstDll, DWORD dwReason, LPVOID LpvResearved)
{
	return TRUE;
}



