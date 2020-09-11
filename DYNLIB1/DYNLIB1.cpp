#include <Windows.h>
#include <stdio.h>
#include <wchar.h>
#include <ShlObj.h>
#include "DYNLIB1.h"

BOOL WINAPI DllMain(HINSTANCE hinstDll,DWORD idReason, LPVOID lpReserved )
{
	return TRUE;		
}
DYNLIB1_API void PrintSysDir(const long csidl[], unsigned long nCount)
{
	TCHAR szPath[MAX_PATH + 1];
	for (unsigned long i = 0; i < nCount; ++i)
	{
		HRESULT hr = SHGetFolderPath(NULL, csidl[i], NULL, SHGFP_TYPE_CURRENT, szPath);
		if (S_OK == hr)
			wprintf(TEXT("%d:%s\n"), i + 1, szPath);
	}
		return;
}
