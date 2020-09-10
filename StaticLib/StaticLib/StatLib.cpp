#include <Windows.h>
#include <stdio.h>
#include <tchar.h>
#include <lmcons.h> //для функций GetUserName и GetUserNameEx
#include <secext.h> //для функции GetUserNameEx
#include "StatLib.h"

BOOL PrintCompName()
{
	TCHAR szbuffer[MAX_COMPUTERNAME_LENGTH + 1];
	DWORD cchar = _countof(szbuffer); //считаем размер выделенного под строку буфера символах
	BOOL ret = GetComputerName(szbuffer, &cchar); //если имя компьюетра определено вернет TRUE?
	if (FALSE != ret)
	{
		wprintf(TEXT("Имя компьюютера в локальной сети:%s\n"), szbuffer);
	}
	return ret;
}

BOOL PrintDNSName()
{
	//Для определения размера буфера установим NULL и 0
	LPWSTR szbuffer = NULL;
	DWORD cchar = 0; 
	GetComputerNameEx(ComputerNameDnsHostname, szbuffer, &cchar); //определение размера буфера в cchar
	szbuffer = new TCHAR[cchar];//выделение вычисленной памяти из ОС
	BOOL ret = GetComputerNameEx(ComputerNameDnsHostname, szbuffer, &cchar); //если имя компьюетра определено вернет TRUE?
	if (FALSE != ret)
	{
		wprintf(TEXT("Доменное имя локального компьютера:%s\n"), szbuffer);
	}
	delete[] szbuffer;
	return ret;
}

BOOL PrintUserName()
{
	TCHAR szbuffer[UNLEN + 1];
	DWORD cchar = _countof(szbuffer); //считаем размер выделенного под строку буфера символах
	BOOL ret = GetUserName(szbuffer, &cchar); //извелечение имени пользователя в текущем сеансе
	if (FALSE != ret)
	{
		wprintf(TEXT("Имя пользователя в текущем сеансе:%s\n"), szbuffer);
	}
	return ret;
}

BOOL PrintUserNameExtended()
{
	//Для определения размера буфера установим NULL и 0
	TCHAR szbuffer[UNLEN + 1];
	ULONG cchar = _countof(szbuffer);

	BOOLEAN ret = GetUserNameEx(NameSamCompatible, szbuffer, &cchar); //если имя компьюетра определено вернет TRUE?
	if (FALSE != ret)
	{
		wprintf(TEXT("Доменное имя локального компьютера:%s\n"), szbuffer);
	}
	delete[] szbuffer;
	return ret;
}