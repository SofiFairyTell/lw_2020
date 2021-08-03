
#include <Windows.h>
#include <stdio.h>
#include <tchar.h>
#include <lmcons.h> //��� ������� GetUserName � GetUserNameEx
//#include <secext.h> //��� ������� GetUserNameEx

//#include "StatLib.h"

#pragma comment(lib,"Secur32.lib") 
#define SECURITY_WIN32
#include <security.h>//��� ������� GetUserNameEx

BOOL PrintCompName()
{
	TCHAR szbuffer[MAX_COMPUTERNAME_LENGTH + 1];
	DWORD cchar = _countof(szbuffer); //������� ������ ����������� ��� ������ ������ ��������
	BOOL ret = GetComputerName(szbuffer, &cchar); //���� ��� ���������� ���������� ������ TRUE?
	if (FALSE != ret)
	{
		wprintf(TEXT("��� ���������� � ��������� ����:%s\n"), szbuffer);
	}
	return ret;

}

BOOL PrintDNSName()
{
	//��� ����������� ������� ������ ��������� NULL � 0
	LPWSTR szbuffer = NULL;
	DWORD cchar = 0;
	GetComputerNameEx(ComputerNameDnsHostname, szbuffer, &cchar); //����������� ������� ������ � cchar
	szbuffer = new TCHAR[cchar];//��������� ����������� ������ �� ��
	BOOL ret = GetComputerNameEx(ComputerNameDnsHostname, szbuffer, &cchar); //���� ��� ���������� ���������� ������ TRUE?
	if (FALSE != ret)
	{
		wprintf(TEXT("�������� ��� ���������� ����������:%s\n"), szbuffer);
	}
	delete[] szbuffer;
	return ret;
}

BOOL PrintUserName()
{
	TCHAR szbuffer[UNLEN + 1];
	DWORD cchar = _countof(szbuffer); //������� ������ ����������� ��� ������ ������ ��������
	BOOL ret = GetUserName(szbuffer, &cchar); //����������� ����� ������������ � ������� ������
	if (FALSE != ret)
	{
		wprintf(TEXT("��� ������������ � ������� ������:%s\n"), szbuffer);
	}
	return ret;
}

BOOL PrintUserNameExtended()
{
	//��� ����������� ������� ������ ��������� NULL � 0
	TCHAR szbuffer[UNLEN + 1];
	ULONG cchar = _countof(szbuffer);
	BOOLEAN ret = GetUserNameEx(NameSamCompatible, szbuffer, &cchar); //���� ��� ������������ ���������� ������ TRUE
	if (FALSE != ret)
	{
		wprintf(TEXT("�������� ��� ���������� ����������:%s\n"), szbuffer);
	}
	return ret;
}