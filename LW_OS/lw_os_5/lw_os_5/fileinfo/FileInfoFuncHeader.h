#pragma once
#include <Windows.h>
#include <WindowsX.h>
#include <tchar.h>
#include <CommCtrl.h>
#include <strsafe.h>
#include <time.h>//��� ctime
#include <shlobj.h>
#include <shlwapi.h>
#include <shobjidl.h>//for browserinfo
#include <fileapi.h>
#include <string>
#include "resource.h"
#include <AccCtrl.h>
#include <aclapi.h>//GetExplicitEntriesFromAclA function (aclapi.h)
#include <sddl.h>//for ConvertSidToStringSidW
#include  <Lmcons.h> //��� UNLEN



#pragma warning(disable : 4996) //��������� ������ deprecate. ���������, ����� ������������ ���������� ������
#pragma comment(linker,"\"/manifestdependency:type='win32' \
name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

#pragma comment(lib, "shlwapi.lib")
// ������� ��������� �������� ����
LRESULT CALLBACK MainWindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

/*���� ����������*/
BOOL DialogAce_OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam);
void DialogAce_OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify);
INT_PTR CALLBACK DialogAceProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);

/*����������� ��������� WM_CREATE WM_DESTROY WM_SIZE WM_COMMAND */

BOOL OnCreate(HWND hwnd, LPCREATESTRUCT lpCreateStruct);
void OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify);

/*���������*/
// ������ ��������� �������� ���� grfInheritance
constexpr DWORD dwInherit[7] = {
	NO_INHERITANCE,
	SUB_CONTAINERS_AND_OBJECTS_INHERIT,
	SUB_CONTAINERS_ONLY_INHERIT,
	SUB_OBJECTS_ONLY_INHERIT,
	INHERIT_ONLY | SUB_CONTAINERS_AND_OBJECTS_INHERIT,
	INHERIT_ONLY | SUB_CONTAINERS_ONLY_INHERIT,
	INHERIT_ONLY | SUB_OBJECTS_ONLY_INHERIT
};

// ������ �����, ���������� �������� ��� ��������� �������� ���� grfInheritance
constexpr LPCTSTR szInheritText[7] = {
	TEXT("������ ��� ����� ��������"),
	TEXT("��� ����� ��������, ��� ������������ � ������"),
	TEXT("��� ����� �������� � ��� ������������"),
	TEXT("��� ����� �������� � ��� ������"),
	TEXT("������ ��� ������������ � ������"),
	TEXT("������ ��� ������������"),
	TEXT("������ ��� ������")
};

// ������ ���������� ��� �����/��������
constexpr DWORD dwPermissions[13] = {
	FILE_TRAVERSE,
	FILE_LIST_DIRECTORY,
	FILE_READ_ATTRIBUTES,
	FILE_READ_EA,
	FILE_ADD_FILE,
	FILE_ADD_SUBDIRECTORY,
	FILE_WRITE_ATTRIBUTES,
	FILE_WRITE_EA,
	FILE_DELETE_CHILD,
	DELETE,
	READ_CONTROL,
	WRITE_DAC,
	WRITE_OWNER
};

// ������ ��������������� ������� ��� ����������
constexpr WORD idcPermissions[13] = {
	IDC_FILE_TRAVERSE,
	IDC_FILE_LIST_DIRECTORY,
	IDC_FILE_READ_ATTRIBUTES,
	IDC_FILE_READ_EA,
	IDC_FILE_ADD_FILE,
	IDC_FILE_ADD_SUBDIRECTORY,
	IDC_FILE_WRITE_ATTRIBUTES,
	IDC_FILE_WRITE_EA,
	IDC_FILE_DELETE_CHILD,
	IDC_DELETE,
	IDC_READ_CONTROL,
	IDC_WRITE_DAC,
	IDC_WRITE_OWNER
};



/*������� � ListView*/
BOOL ListViewInit(LPTSTR path, HWND hwnd);
/*Var*/
RECT rect = { 0 }; // ������ � ��������� ����

HANDLE hFile = INVALID_HANDLE_VALUE; // ���������� �������������� ���������� �����
LPCTSTR lpszFileName = NULL; // ��������� �� ��� �����/��������
DWORD cchPath = 0; // ����� ���� � �����/��������
HKEY hKey = NULL; // ���������� ����� ������

/**/
PSECURITY_DESCRIPTOR Sec_Descriptor;//���������� ������������
TCHAR FileName[MAX_PATH] = TEXT(""); // ���� �� �������������� �����/�����

/*�������*/
BOOL GetFileSecurityDescriptor(LPCWSTR lpFileName, SECURITY_INFORMATION RequestedInformation, PSECURITY_DESCRIPTOR *ppSD);
BOOL GetItemFromDACL(PSECURITY_DESCRIPTOR Sec_Descriptor, PULONG pcCountOfEntries, PEXPLICIT_ACCESS *pListOfEntries);
BOOL GetAccountName_W(PSID psid, LPWSTR* AccountName);
BOOL GetAccountSID_W(LPCWSTR AccountName, PSID *ppsid); //������ SID �� ��� USER
BOOL GetOwnerName_W(PSECURITY_DESCRIPTOR Sec_Descriptor, LPWSTR *OwnerName);
BOOL SetFileSecurityInfo(LPCTSTR FileName, LPWSTR NewOwner, ULONG CountOfEntries, PEXPLICIT_ACCESS pListOfEntries, BOOL bMergeEntries);
BOOL DeleteEntryFromDalc(PSECURITY_DESCRIPTOR pSD, DWORD dwIndex);