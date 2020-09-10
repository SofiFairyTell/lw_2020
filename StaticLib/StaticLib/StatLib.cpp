#include <Windows.h>
#include <stdio.h>
#include <tchar.h>
#include "StatLib.h"

BOOL PrintCompName()
{
	TCHAR szbuffer[MAX_COMPUTERNAME_LENGTH + 1];
	DWORD ch = _countof(szbuffer); //считаем размер выделенного под строку буфера
	BOOL ret = GetComputerName(szbuffer,)

}

BOOL PrintDNSName()
{
	return;
}

BOOL PrintUserName()
{
	return ;
}
