#include <Windows.h>
#include <tchar.h>
#include <strsafe.h>
#include <stdio.h>
#include <locale.h>
#include <NTSecAPI.h>//for las_handle
#include <AclAPI.h>
#include <winnt.h>
#include <sddl.h>//for ConvertSidToStringSidW
#include <LMCons.h>
#include <cwchar>
#include <iostream>
#include <string>

#define LOCAL_FREE(ptr) if (NULL != (ptr)) LocalFree(ptr)
/*Функции и переменные*/
LSA_HANDLE OpenLocalPolicy(ACCESS_MASK AccessType);//открытие дескриптора политики безопасности локального пк

BOOL GetAccountName_W(PSID psid, LPWSTR* AccountName);//определеим имя аккаунта по его SID
BOOL GetWellKnowSID(WELL_KNOWN_SID_TYPE WellKnowSidType, PSID sidDomain, PSID *sidWellKnow);//определение хорошо известного SID
BOOL GetAccountSID_W(LPCWSTR AccountName, PSID *ppsid); //узнаем SID ПК ИЛИ USER

void CoutSID(PSID psid);//вывод списков на экран
void ListOfPrivilegesAndRights_User(LSA_HANDLE PolicyHandle, PSID sidUser);//вывод списка привелегий пользователя


int _tmain()
{
	_tsetlocale(LC_ALL, TEXT(""));

	/*Pointer Security ID*/
	PSID sidDomain = NULL;//security ID для локального ПК
	PSID sidUser = NULL;//security ID для локального пользователя
	PSID sidWellKnow;//для работы с хорошо известными SID


	//Вспомогательные переменные
	DWORD count_char;//для определения размеров
	BOOL RetRes = FALSE; //для результатов

	//Переменные для вывода имен ПК и пользователя*
	WCHAR ComputerName[MAX_COMPUTERNAME_LENGTH + 1] = TEXT("");
	WCHAR UserName[UNLEN + 1] = TEXT("");

	
	count_char = _countof(ComputerName);

	RetRes = GetComputerName(ComputerName, &count_char);//узнаем имя ПК
	
	if (RetRes != FALSE)
	{
		RetRes = GetAccountSID_W(ComputerName, &sidDomain);//определим SID ПК

		if ((RetRes != FALSE) && (sidDomain != NULL))
		{
			CoutSID(sidDomain);//выведем SID ПК на экран
		}
	}

	/*---------------------------------------------------------------------------------------*/
	count_char = _countof(UserName);
	RetRes = GetUserName(UserName, &count_char);//узнаем имя пользователя

	LSA_HANDLE PolicyHandle = OpenLocalPolicy(POLICY_LOOKUP_NAMES);

	if (RetRes != FALSE)
	{
		RetRes = GetAccountSID_W(UserName, &sidUser);

		if ((RetRes != FALSE) && (sidUser != NULL))
		{
			CoutSID(sidUser);
			//вывод списка привелегий
			if (PolicyHandle != NULL)
				ListOfPrivilegesAndRights_User(PolicyHandle, sidUser);//НОРМ, ТАК И ДОЛЖНО БЫТЬ. 

			LocalFree(sidUser);//повреждение кучи?
		}
	}
	/*---------------Работа с хорошо известными SID-----------------------------*/
	constexpr WELL_KNOWN_SID_TYPE WellKnownType[] =
	{
		WinConsoleLogonSid,
		WinAuthenticatedUserSid,
		WinLocalSystemSid,
		WinAccountAdministratorSid,
		WinBuiltinUsersSid,
		WinBuiltinNetworkConfigurationOperatorsSid
	};

	for (int i = 0; i < _countof(WellKnownType); ++i)
	{
		RetRes = GetWellKnowSID(WellKnownType[i], sidDomain, &sidWellKnow);

		if ((RetRes != FALSE) && (sidWellKnow != NULL))
		{
			CoutSID(sidWellKnow);
			//вывод списка привелегий

			if (PolicyHandle != NULL)
			{
				ListOfPrivilegesAndRights_User(PolicyHandle, sidWellKnow);
			}
			LocalFree(sidWellKnow);
		}
	}

	system("pause");
	LOCAL_FREE(sidDomain);

	LsaClose(PolicyHandle);

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

/*Определения функций определяющих SID*/
BOOL GetAccountSID_W(LPCWSTR AccountName, PSID *ppsid)
{
	BOOL RetRes = FALSE;
	SID_NAME_USE SidType;//переменная перечисляемого типа, сюда сохраним определенный тип SID

	/*Переменные для определения имени и SID*/
	LPWSTR RefDomainName = NULL;
	PSID psid = NULL;
	DWORD cbSID = 0, cchRefDomainName = 0;

	LookupAccountNameW(NULL, AccountName, NULL, &cbSID, NULL, &cchRefDomainName, NULL);//определение размеров буфера под имена

	if ((cbSID > 0) && (cchRefDomainName > 0))
	{
		psid = (PSID)LocalAlloc(LMEM_FIXED, cbSID); //выделение памяти из локальной кучи процесса
		RefDomainName = (LPWSTR)LocalAlloc(LMEM_FIXED, cchRefDomainName * sizeof(WCHAR));// -||- для имени домена
	}

	if ((psid != NULL) && (RefDomainName != NULL))
	{
		RetRes = LookupAccountName(NULL, AccountName, psid, &cbSID, RefDomainName, &cchRefDomainName, &SidType);
	}

	if (RetRes != FALSE)
	{
		*ppsid = psid;
	}
	else
	{
		if (psid != NULL)
		{
			LocalFree(psid);//освобождаем память
		}
	}

	if (RefDomainName != NULL)
	{
		LocalFree(RefDomainName);//освобождаем память
	}

	return RetRes;
}


BOOL GetAccountName_W(PSID psid, LPWSTR* AccountName)
{
	BOOL RetRes = FALSE;
	SID_NAME_USE SidType;//переменная перечисляемого типа, сюда сохраним определенный тип SID
	/*Переменные для вывода*/
	LPWSTR Name = NULL;
	DWORD cch = 0, cchRefDomainName = 0;

	if (IsValidSid(psid) == FALSE)
	{
		return FALSE;
	}

	LookupAccountSid(NULL, psid, NULL, &cch, NULL, &cchRefDomainName, NULL);//определим размеры буферов

	DWORD cb = (cch + cchRefDomainName) * sizeof(TCHAR);

	if (cb > 0)
	{

		Name = (LPWSTR)LocalAlloc(LMEM_FIXED, cb);//выделение памяти из локальной кучи процесса
	}

	if (Name != NULL)
	{
		RetRes = LookupAccountSid(NULL, psid, Name + cchRefDomainName, &cch, Name, &cchRefDomainName, &SidType);
	}

	if (RetRes != FALSE)
	{
		if (SidTypeDomain != SidType)
		{
			if (cchRefDomainName > 0)
			{
				Name[cchRefDomainName] = '\\';
			}
			else
			{
				StringCbCopy(Name, cb, Name + 1);//копирование для вовзрата в основую функциию
			}
		}
		*AccountName = Name; //вернем полученнное имя в программу
	}
	else
	{
		ConvertSidToStringSid(psid, AccountName);//если не получилось получить имя, то вернем SID
		if (Name != NULL)
		{
			LocalFree(Name);
		}
	}
	return RetRes;
}

BOOL GetWellKnowSID(WELL_KNOWN_SID_TYPE WellKnowSidType, PSID sidDomain, PSID *sidWellKnow)
{
	DWORD MaxSidSize = SECURITY_MAX_SID_SIZE;

	PSID localSID = (PSID)LocalAlloc(LMEM_FIXED, MaxSidSize);

	if (localSID == NULL)
	{
		return FALSE; //определеить SID не удалось. 
	}

	BOOL RetRes = CreateWellKnownSid(WellKnowSidType, sidDomain, localSID, &MaxSidSize);
	if (RetRes != FALSE)
	{
		*sidWellKnow = localSID;//возращаем созданный SID
	}
	else
	{
		LocalFree(localSID);
	}
	return RetRes;
}



void ListOfPrivilegesAndRights_User(LSA_HANDLE PolicyHandle, PSID sidUser)
{
	/*Переменные для вывода*/
	PLSA_UNICODE_STRING List_Rights; //список прав
	ULONG count_list;//элементы в списке прав

	/*Вспомогательные счетчики*/
	DWORD count_char, dwLangId;
	BOOL RetRes = FALSE;

	//Error: 0xc0000034 Определять права для группы?
	NTSTATUS ntstatus = LsaEnumerateAccountRights(PolicyHandle, sidUser, &List_Rights, &count_list);

	if (LSA_SUCCESS(ntstatus))
	{
		wprintf(TEXT("\tСписок прав учетной записи:\n"));
		for (ULONG i = 0; i < count_list; ++i)
		{
			LPCWSTR UserRight = List_Rights[i].Buffer;

			wprintf(TEXT("\t  %2d. %s"), (i + 1), UserRight);

			TCHAR DisplayName[256];//дружественное имя

			count_char = _countof(DisplayName);//определеим количество символов в строке

			RetRes = LookupPrivilegeDisplayName(NULL, UserRight, DisplayName, &count_char, &dwLangId);

			if (RetRes != FALSE)
			{
				wprintf(TEXT(" (%s)"), DisplayName);
			}
			else
			{
				/*Константы прав учетных записей*/
				constexpr LPCTSTR Right_array[20] =
				{
					SE_INTERACTIVE_LOGON_NAME, TEXT("Локальный вход в систему"),
					SE_DENY_INTERACTIVE_LOGON_NAME, TEXT("Запретить локальный вход"),
					SE_NETWORK_LOGON_NAME, TEXT("Доступ к компьютеру из сети"),
					SE_DENY_NETWORK_LOGON_NAME, TEXT("Отказать в доступе к этому компьютеру"),
					SE_BATCH_LOGON_NAME, TEXT("Вход в качестве пакетного задания"),
					SE_DENY_BATCH_LOGON_NAME, TEXT("Отказать во входе в качестве пакетного задания"),
					SE_SERVICE_LOGON_NAME, TEXT("Вход в качестве службы"),
					SE_DENY_SERVICE_LOGON_NAME, TEXT("Отказать во входе в качестве службы"),
					SE_REMOTE_INTERACTIVE_LOGON_NAME, TEXT("Разрешать вход в систему через службы удаленных рабочих столов"),
					SE_DENY_REMOTE_INTERACTIVE_LOGON_NAME, TEXT("Запретить вход в систему через службы удаленных рабочих столов")
				};

				for (int j = 0; j < _countof(Right_array); j += 2)
				{
					if ((_tcscmp(Right_array[j], UserRight) == 0))
					{
						wprintf(TEXT(" (%s)"), Right_array[j + 1]);
						break;
					}
				}
			}
			wprintf(TEXT("\n"));
		}
		wprintf(TEXT("\n"));
		LsaFreeMemory(List_Rights);
	}
}


void CoutSID(PSID psid)
{
	LPWSTR lpSID = NULL, AccountName = NULL;//переменные для вывода
	ConvertSidToStringSidW(psid, &lpSID);
	GetAccountName_W(psid, &AccountName);
	if ((AccountName != NULL) && (psid != NULL))
	{
		wprintf(TEXT("%s (%s)\n\n"), AccountName, lpSID);
	}
	/*Очистка памяти?*/
	LOCAL_FREE(lpSID);
	LOCAL_FREE(AccountName);
}

