#include <Windows.h>
#include <stdio.h>
#include <wchar.h>
#include <ShlObj.h>
#include "DYNLIB1.h"

#pragma warning(disable : 4996) //отключает Ошибку deprecate. Возникает, когда используется устаревшая функция

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
DYNLIB1_API void PrintOSinfo()
{
	OSVERSIONINFO osver;
	osver.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	BOOL ret = GetVersionEx(&osver);
	if (ret != FALSE)
	{
		wprintf(TEXT("Version:%d.%d.%d\n", osver.dwBuildNumber, osver.dwMajorVersion, osver.dwMinorVersion));
		switch (osver.dwMajorVersion)
		{
		case 5:
		{
			switch (osver.dwMinorVersion)
			{
			case 1:
				wprintf(TEXT("Windows XP"));
				break;
			case 2:
				wprintf(TEXT("Windows 2003"));
				break;
			}
		}
		break;
		case 6:
		{
			switch (osver.dwMinorVersion)
			{
			case 0:
				wprintf(TEXT("Windows Vista"));
				break;
			case 1:
				wprintf(TEXT("Windows 7"));
				break;
			case 2:
				wprintf(TEXT("Windows 8"));
				break;
			case 3:
				wprintf(TEXT("Windows 8.1"));
				break;
			}
		}
			break;
		case 10:
			switch (osver.dwMinorVersion)
			{
			case 0:
				{
				wprintf(TEXT("WINDOWS 10"));
				}
				break;	 
			}
		
		default:
			wprintf(TEXT("OS undefined"));
			break;
		}

	}
}
