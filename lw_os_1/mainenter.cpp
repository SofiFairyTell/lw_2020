//#pragma comment (lib,"STATILIBR.lib")
#include <Windows.h>
#include <wchar.h>
#include <locale.h>
#include <stdio.h>
#include <STATILIBR.h>

int wmain()
{
	_wsetlocale(LC_ALL, TEXT(""));
	wprintf(TEXT("����� ������ ����������� ����������\n"));
	PrintCompName();
	PrintDNSName();
	PrintUserName();
	PrintUserNameExtended();
}
