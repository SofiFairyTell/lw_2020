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
/*������� � ����������*/
LSA_HANDLE OpenLocalPolicy(ACCESS_MASK AccessType);//�������� ����������� �������� ������������ ���������� ��

BOOL GetAccountName_W(PSID psid, LPWSTR* AccountName);//���������� ��� �������� �� ��� SID
BOOL GetWellKnowSID(WELL_KNOWN_SID_TYPE WellKnowSidType, PSID sidDomain, PSID *sidWellKnow);//����������� ������ ���������� SID
BOOL GetAccountSID_W(LPCWSTR AccountName, PSID *ppsid); //������ SID �� ��� USER

void CoutSID(PSID psid);//����� ������� �� �����
void ListOfPrivilegesAndRights_User(LSA_HANDLE PolicyHandle, PSID sidUser);//����� ������ ���������� ������������


int _tmain()
{
	_tsetlocale(LC_ALL, TEXT(""));

	/*Pointer Security ID*/
	PSID sidDomain = NULL;//security ID ��� ���������� ��
	PSID sidUser = NULL;//security ID ��� ���������� ������������
	PSID sidWellKnow;//��� ������ � ������ ���������� SID


	//��������������� ����������
	DWORD count_char;//��� ����������� ��������
	BOOL RetRes = FALSE; //��� �����������

	//���������� ��� ������ ���� �� � ������������*
	WCHAR ComputerName[MAX_COMPUTERNAME_LENGTH + 1] = TEXT("");
	WCHAR UserName[UNLEN + 1] = TEXT("");

	
	count_char = _countof(ComputerName);

	RetRes = GetComputerName(ComputerName, &count_char);//������ ��� ��
	
	if (RetRes != FALSE)
	{
		RetRes = GetAccountSID_W(ComputerName, &sidDomain);//��������� SID ��

		if ((RetRes != FALSE) && (sidDomain != NULL))
		{
			CoutSID(sidDomain);//������� SID �� �� �����
		}
	}

	/*---------------------------------------------------------------------------------------*/
	count_char = _countof(UserName);
	RetRes = GetUserName(UserName, &count_char);//������ ��� ������������

	LSA_HANDLE PolicyHandle = OpenLocalPolicy(POLICY_LOOKUP_NAMES);

	if (RetRes != FALSE)
	{
		RetRes = GetAccountSID_W(UserName, &sidUser);

		if ((RetRes != FALSE) && (sidUser != NULL))
		{
			CoutSID(sidUser);
			//����� ������ ����������
			if (PolicyHandle != NULL)
				ListOfPrivilegesAndRights_User(PolicyHandle, sidUser);//����, ��� � ������ ����. 

			LocalFree(sidUser);//����������� ����?
		}
	}
	/*---------------������ � ������ ���������� SID-----------------------------*/
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
			//����� ������ ����������

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

	ZeroMemory(&ObjAtr, sizeof(ObjAtr));//������������� ��������� LSA_OBJECT_ATTRIBUTES

	NTSTATUS ntstatus = LsaOpenPolicy(NULL, &ObjAtr, AccessType, &PolicyHandle);//��������� ����������� �������� ������������

	SetLastError(LsaNtStatusToWinError(ntstatus)); //��� ������

	return LSA_SUCCESS(ntstatus) ? PolicyHandle : NULL; //���� ��������� �������, �� ������� PolicyHandle

}

/*����������� ������� ������������ SID*/
BOOL GetAccountSID_W(LPCWSTR AccountName, PSID *ppsid)
{
	BOOL RetRes = FALSE;
	SID_NAME_USE SidType;//���������� �������������� ����, ���� �������� ������������ ��� SID

	/*���������� ��� ����������� ����� � SID*/
	LPWSTR RefDomainName = NULL;
	PSID psid = NULL;
	DWORD cbSID = 0, cchRefDomainName = 0;

	LookupAccountNameW(NULL, AccountName, NULL, &cbSID, NULL, &cchRefDomainName, NULL);//����������� �������� ������ ��� �����

	if ((cbSID > 0) && (cchRefDomainName > 0))
	{
		psid = (PSID)LocalAlloc(LMEM_FIXED, cbSID); //��������� ������ �� ��������� ���� ��������
		RefDomainName = (LPWSTR)LocalAlloc(LMEM_FIXED, cchRefDomainName * sizeof(WCHAR));// -||- ��� ����� ������
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
			LocalFree(psid);//����������� ������
		}
	}

	if (RefDomainName != NULL)
	{
		LocalFree(RefDomainName);//����������� ������
	}

	return RetRes;
}


BOOL GetAccountName_W(PSID psid, LPWSTR* AccountName)
{
	BOOL RetRes = FALSE;
	SID_NAME_USE SidType;//���������� �������������� ����, ���� �������� ������������ ��� SID
	/*���������� ��� ������*/
	LPWSTR Name = NULL;
	DWORD cch = 0, cchRefDomainName = 0;

	if (IsValidSid(psid) == FALSE)
	{
		return FALSE;
	}

	LookupAccountSid(NULL, psid, NULL, &cch, NULL, &cchRefDomainName, NULL);//��������� ������� �������

	DWORD cb = (cch + cchRefDomainName) * sizeof(TCHAR);

	if (cb > 0)
	{

		Name = (LPWSTR)LocalAlloc(LMEM_FIXED, cb);//��������� ������ �� ��������� ���� ��������
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
				StringCbCopy(Name, cb, Name + 1);//����������� ��� �������� � ������� ��������
			}
		}
		*AccountName = Name; //������ ����������� ��� � ���������
	}
	else
	{
		ConvertSidToStringSid(psid, AccountName);//���� �� ���������� �������� ���, �� ������ SID
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
		return FALSE; //����������� SID �� �������. 
	}

	BOOL RetRes = CreateWellKnownSid(WellKnowSidType, sidDomain, localSID, &MaxSidSize);
	if (RetRes != FALSE)
	{
		*sidWellKnow = localSID;//��������� ��������� SID
	}
	else
	{
		LocalFree(localSID);
	}
	return RetRes;
}



void ListOfPrivilegesAndRights_User(LSA_HANDLE PolicyHandle, PSID sidUser)
{
	/*���������� ��� ������*/
	PLSA_UNICODE_STRING List_Rights; //������ ����
	ULONG count_list;//�������� � ������ ����

	/*��������������� ��������*/
	DWORD count_char, dwLangId;
	BOOL RetRes = FALSE;

	//Error: 0xc0000034 ���������� ����� ��� ������?
	NTSTATUS ntstatus = LsaEnumerateAccountRights(PolicyHandle, sidUser, &List_Rights, &count_list);

	if (LSA_SUCCESS(ntstatus))
	{
		wprintf(TEXT("\t������ ���� ������� ������:\n"));
		for (ULONG i = 0; i < count_list; ++i)
		{
			LPCWSTR UserRight = List_Rights[i].Buffer;

			wprintf(TEXT("\t  %2d. %s"), (i + 1), UserRight);

			TCHAR DisplayName[256];//������������� ���

			count_char = _countof(DisplayName);//���������� ���������� �������� � ������

			RetRes = LookupPrivilegeDisplayName(NULL, UserRight, DisplayName, &count_char, &dwLangId);

			if (RetRes != FALSE)
			{
				wprintf(TEXT(" (%s)"), DisplayName);
			}
			else
			{
				/*��������� ���� ������� �������*/
				constexpr LPCTSTR Right_array[20] =
				{
					SE_INTERACTIVE_LOGON_NAME, TEXT("��������� ���� � �������"),
					SE_DENY_INTERACTIVE_LOGON_NAME, TEXT("��������� ��������� ����"),
					SE_NETWORK_LOGON_NAME, TEXT("������ � ���������� �� ����"),
					SE_DENY_NETWORK_LOGON_NAME, TEXT("�������� � ������� � ����� ����������"),
					SE_BATCH_LOGON_NAME, TEXT("���� � �������� ��������� �������"),
					SE_DENY_BATCH_LOGON_NAME, TEXT("�������� �� ����� � �������� ��������� �������"),
					SE_SERVICE_LOGON_NAME, TEXT("���� � �������� ������"),
					SE_DENY_SERVICE_LOGON_NAME, TEXT("�������� �� ����� � �������� ������"),
					SE_REMOTE_INTERACTIVE_LOGON_NAME, TEXT("��������� ���� � ������� ����� ������ ��������� ������� ������"),
					SE_DENY_REMOTE_INTERACTIVE_LOGON_NAME, TEXT("��������� ���� � ������� ����� ������ ��������� ������� ������")
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
	LPWSTR lpSID = NULL, AccountName = NULL;//���������� ��� ������
	ConvertSidToStringSidW(psid, &lpSID);
	GetAccountName_W(psid, &AccountName);
	if ((AccountName != NULL) && (psid != NULL))
	{
		wprintf(TEXT("%s (%s)\n\n"), AccountName, lpSID);
	}
	/*������� ������?*/
	LOCAL_FREE(lpSID);
	LOCAL_FREE(AccountName);
}

