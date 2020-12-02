#include <Windows.h>
#include <tchar.h>
#include <strsafe.h>
#include <stdio.h>
#include <locale.h>
#include <NTSecAPI.h>//for las_handle
#include <winnt.h>
#include <sddl.h>//for ConvertSidToStringSidW
#include <LMCons.h>
#include <cwchar>
#include <iostream>
//#include <>

int _tmain();

/*Функции и переменные*/
LSA_HANDLE OpenLocalPolicy(ACCESS_MASK AccessType);//открытие дескриптора политики безопасности локального пк

BOOL GetAccountSID_W(LPCWSTR AccountName, PSID *ppsid);//узнаем SID ПК
BOOL GetAccountName_W(PSID psid, LPWSTR* AccountName);//определеим имя аккаунта по его SID
BOOL GetWellKnowSID(WELL_KNOWN_SID_TYPE WellKnowSidType, PSID sidDomain, PSID *sidWellKnow);//определение хорошо известного SID

void ListOfPrivilegesAndRights_User(LSA_HANDLE PolicyHandle, PSID sidUser);//вывод списка привелегий пользователя
void CoutSID(PSID psid);//вывод списков на экран
//UNICODE проект
int _tmain()
{
	_tsetlocale(LC_ALL, TEXT(" "));

	/*Pointer Security ID*/
	PSID sidDomain = NULL; //security ID для локального ПК
	PSID sidUser = NULL; //security ID для локального пользователя
	PSID sidWellKnow;//о
	LSA_HANDLE PolicyHandle = OpenLocalPolicy(POLICY_LOOKUP_NAMES);

	/*Вспомогательные переменные*/
	DWORD count_char;//для определения размеров
	BOOL RetRes = FALSE; //для результатов
	DWORD dwError = NULL; //для ошибки

	/*Переменные для вывода имен ПК и пользователя*/
	WCHAR ComputerName[MAX_COMPUTERNAME_LENGTH + 1] = TEXT("");
	WCHAR UserName[UNLEN + 1] = L"";
	
	/*Начало*/
	count_char = _countof(ComputerName);

	RetRes = GetComputerNameW(ComputerName, &count_char);//узнаем имя ПК

	if (RetRes != FALSE)
	{
		RetRes = GetAccountSID_W(ComputerName, &sidDomain);
		
		if ((RetRes != FALSE) && (sidDomain != NULL))
		{
			CoutSID(sidDomain);
		}
	}
	else
	{
		SetLastError(dwError);//ловим ошибку?
		return 0; //завершаем работу программы
	}

	count_char = _countof(UserName);

	RetRes = GetUserName(UserName, &count_char);//узнаем имя пользователя
	if (RetRes != FALSE)
	{
		RetRes = GetAccountSID_W(UserName, &sidUser);

		if ((RetRes != FALSE) && (sidUser != NULL))
		{
			CoutSID(sidUser);
			//вывод списка привелегий

			if (PolicyHandle != NULL)
			{
				ListOfPrivilegesAndRights_User(PolicyHandle, sidUser);
			//	LocalFree(sidUser);//повреждение кучи?
			}

			
		}
	}

	constexpr WELL_KNOWN_SID_TYPE WellKnownType[] =
	{
		WinConsoleLogonSid,
		WinAuthenticatedUserSid,
		WinLocalSystemSid,
		WinAccountAdministratorSid,
		WinBuiltinIUsersSid,
		WinBuiltinNetworkConfigurationOperatorsSid
	};

	for (int i = 0; i < _countof(WellKnownType); ++i)
	{
		RetRes = GetWellKnowSID(WellKnownType[i], sidDomain, &sidWellKnow);
		
		if ((RetRes != FALSE) && (sidWellKnow != NULL))
		{
			CoutSID(sidDomain);
			//вывод списка привелегий

			if (PolicyHandle != NULL)
			{
				ListOfPrivilegesAndRights_User(PolicyHandle, sidWellKnow);

			}

			//LocalFree(sidWellKnow);
		}
	}

  FreeSid(sidDomain);
  FreeSid(sidUser);
  FreeSid(sidWellKnow);
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
		LocalFree(psid);//освбодждаем память
	}
}

if (RefDomainName != NULL)
{
	LocalFree(RefDomainName);//освбодждаем память
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
	DWORD cb = (cch + cchRefDomainName) * sizeof(WCHAR);
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
				std::wcscpy(Name, Name + 1);//копирование для вовзрата в основую функциию
			}
		}
		*AccountName = Name; //вернем полученнное имя в программу
	}
	else
	{
		ConvertSidToStringSidW(psid, AccountName);//если не получилось получить имя, то вернем SID
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
	WCHAR DisplayName[256];//дружественное имя

	/*Вспомогательные счетчики*/
	DWORD count_char;//подсчет количества символов в строке
	DWORD lpLanguageID;//ID языка?
	ULONG count_list;//элементы в списке прав

	BOOL RetRes = FALSE;

	NTSTATUS ntstatus = LsaEnumerateAccountRights(PolicyHandle, sidUser, &List_Rights, &count_list);
	if (LSA_SUCCESS(ntstatus))
	{
		std::cout << "Список прав учетных записей" << std::endl;
		for (ULONG i = 0; i < count_list; ++i)
		{
			LPCWSTR UserRight = List_Rights[i].Buffer;
			std::cout << "\t" << i + 1 << "  " << UserRight;

			count_char = _countof(DisplayName);//определеим количество символов в строке

			RetRes = LookupPrivilegeDisplayName(NULL, UserRight, DisplayName, &count_char, &lpLanguageID);

			if (RetRes != FALSE)
			{
				std::cout << DisplayName << std::endl;
			}
			else
			{
				/*Константы прав учетных записей*/
				constexpr LPCWSTR Right_array[20] =
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

				for (int j = 0; j < _countof(Right_array); j+=2)
				{
					if ((wcscmp(Right_array[j], UserRight) == 0))
					{
						std::cout<<Right_array[j+1];
					}
				}
			}
			std::cout << std::endl;
		}
		std::cout << std::endl;
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
		std::wcout << AccountName << "  " << psid << std::endl;
	}
	/*Очистка памяти?*/
	FreeSid(psid);
	//delete AccountName;
}

