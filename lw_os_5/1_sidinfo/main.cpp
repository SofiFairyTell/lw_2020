#include <Windows.h>
#include <tchar.h>
#include <strsafe.h>
#include <stdio.h>
#include <locale.h>
#include <NTSecAPI.h>//for las_handle
#include <sddl.h>//for ConvertSidToStringSidW
#include <LMCons.h>

/*Функции и переменные*/
LSA_HANDLE OpenLocalPolicy(ACCESS_MASK AccessType);//открытие дескриптора политики безопасности локального пк
BOOL GetAccountSID_W(LPCWSTR AccountName, PSID *ppsid);//узнаем SID ПК

//UNICODE проект
int _tmain()
{
	_tsetlocale(LC_ALL, TEXT(" "));
	PSID sidDomain = NULL; //security ID для локального ПК
	LSA_HANDLE PolicyHandle = OpenLocalPolicy(POLICY_LOOKUP_NAMES);

	/*Переменные для вывода имени ПК*/
	WCHAR ComputerName[MAX_COMPUTERNAME_LENGTH + 1] = TEXT("");
	DWORD count_char = _countof(ComputerName);

	BOOL RetRes = GetComputerNameW(ComputerName, &count_char);//узнаем имя ПК

	if (RetRes != FALSE)
	{
		RetRes = GetAccountSID_W(ComputerName, &sidDomain);
		
		if ((RetRes != FALSE) && (sidDomain != NULL))
		{
			CoutSID(sidDomain);
		}
	}

}


LSA_HANDLE OpenLocalPolicy(ACCESS_MASK AccessType)
{
	//POLICY_LOOKUP_NAMES - this access type is needed to translate between names and SIDs.
	LSA_HANDLE PolicyHandle;
	LSA_OBJECT_ATTRIBUTES ObjAtr;
	
	ZeroMemory(&ObjAtr, sizeof(ObjAtr));//Инициализация структуры LSA_OBJECT_ATTRIBUTES
	
	NTSTATUS ntstatus = LsaOpenPolicy(NULL, &ObjAtr, AccessType, &PolicyHandle);//получение дескриптора политики безопасности

	SetLastError(LsaNtStatusToWinError(ntstatus)); //для ошибок

	return LSA_SUCCESS(ntstatus) ? PolicyHandle : NULL; //если завершено успешно, то вернуть PolicyHandle

}

BOOL GetAccountSID_W(LPCWSTR AccountName, PSID *ppsid)
{
	BOOL RetRes = FALSE;

	/*Переменные для определения имени и SID*/
	LPWSTR RefDomainName = NULL;
	PSID psid = NULL;
	DWORD cbSID = 0, cchRefDomainName = 0;

	LookupAccountNameW(NULL, AccountName, NULL, &cbSID, NULL, &cchRefDomainName, NULL);//определение размеров буфера под имена

	if ((cbSID > 0)&&(cchRefDomainName>0))
	{
		psid = (PSID)LocalAlloc(LMEM_FIXED, cbSID); //выделение памяти из локальной кучи процесса
		RefDomainName = (LPWSTR)LocalAlloc(LMEM_FIXED, cchRefDomainName * sizeof(WCHAR));// -||- для имени домена
	}


	if ((psid != NULL) && (RefDomainName != NULL))
	{
		SID_NAME_USE SidType;//переменная перечисляемого типа, сюда сохраним определенный тип SID
		RetRes = LookupAccountNameW(NULL, AccountName, psid, &cbSID, RefDomainName, &cchRefDomainName, &SidType);
	}

	if (RetRes != FALSE)
	{
		*ppsid = psid;
	}
	else
	{
		if (psid != NULL)
		{
			LocalFree(psid);
		}
	}

	if (RefDomainName != NULL)
	{
		LocalFree(RefDomainName);
	}

	return RetRes;
}
BOOL GetAccountName_W(PSID psid, LPWSTR AccountName)
{
	/*Переменные для вывода*/
	LPWSTR Name = NULL;
	DWORD cch = 0, cchRefDomainName = 0;

	if (IsValidSid(psid) == FALSE)
	{
		return FALSE;
	}

	LookupAccountSid(NULL, psid, NULL, &cch, NULL, &cchRefDomainName, NULL);//определим размеры буферов



}



void CoutSID(PSID psid)
{
	LPWSTR lpSID = NULL, AccountName = NULL;//переменные для вывода
	ConvertSidToStringSidW(psid, &lpSID);
	GetAccountName_W(psid, &AccountName);
}

