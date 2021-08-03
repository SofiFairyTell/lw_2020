
#include <Windows.h>
#include <stdio.h>
#include <wchar.h>
#include <locale.h>
#include "DYNLIB2.h"
#define DYNLIB2_PROTOTYPES

//����� ����� � dll ��� �������� � �������� ������������ ����������� ����������
BOOL WINAPI DllMain(HINSTANCE hInstDll, DWORD dwReason, LPVOID LpvResearved)
{
	return TRUE;
}

DYNLIB2_API void PrintSYSmetr(LPCWSTR LpDisplayNam, int index)
{
	int value = GetSystemMetrics(index);
	switch (index)
	{
	case SM_CXEDGE:
		//wprintf(L"����������� ������ ���������� �������");
		wprintf(L"%s:%d px\n", LpDisplayNam, value);
		break;
	case SM_CYEDGE:
		//wprintf(L"���������� ������ ���������� �������");
		wprintf(L"%s:%d px\n", LpDisplayNam, value);
		break;
	case SM_CXMINSPACING:
		//wprintf(L"����������� ������� ������ ����� ��� ��������� ����");
		wprintf(L"%s:%d px\n", LpDisplayNam, value);
	case SM_CYMINSPACING:
		//wprintf(L"����������� ������� ������ ����� ��� ��������� ����");
		wprintf(L"%s:%d px\n", LpDisplayNam, value);
		break;
	case SM_SHOWSOUNDS:
		//wprintf(L"��������� �������� � ������ ������ ���������� � ����������� �������������� ������ � �������� �������");
		wprintf(L"%s:%d px\n",LpDisplayNam,value);
				break;
	}
		
}
