#pragma once
#include <Windows.h>
#include <WindowsX.h>
#include <tchar.h>
#include <CommCtrl.h>
#include <strsafe.h>
#include <time.h>//для ctime
#include <shlobj.h>
#include <shlwapi.h>
#include <shobjidl.h>//for browserinfo
#include <fileapi.h>
#include <string>
#include "resource.h"
#include <AccCtrl.h>
#include <aclapi.h>//GetExplicitEntriesFromAclA function (aclapi.h)
#include <sddl.h>//for ConvertSidToStringSidW
#include  <Lmcons.h> //для UNLEN



#pragma warning(disable : 4996) //отключает Ошибку deprecate. Возникает, когда используется устаревшая функци
#pragma comment(linker,"\"/manifestdependency:type='win32' \
name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

#pragma comment(lib, "shlwapi.lib")
// оконная процедура главного окна
LRESULT CALLBACK MainWindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

/*Окно диалоговое*/
BOOL DialogAce_OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam);
void DialogAce_OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify);
INT_PTR CALLBACK DialogAceProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);

/*Обработчики сообщений WM_CREATE WM_DESTROY WM_SIZE WM_COMMAND */

BOOL OnCreate(HWND hwnd, LPCREATESTRUCT lpCreateStruct);
void OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify);

/*Константы*/
// массив возможных значений поля grfInheritance
constexpr DWORD dwInherit[7] = {
	NO_INHERITANCE,
	SUB_CONTAINERS_AND_OBJECTS_INHERIT,
	SUB_CONTAINERS_ONLY_INHERIT,
	SUB_OBJECTS_ONLY_INHERIT,
	INHERIT_ONLY | SUB_CONTAINERS_AND_OBJECTS_INHERIT,
	INHERIT_ONLY | SUB_CONTAINERS_ONLY_INHERIT,
	INHERIT_ONLY | SUB_OBJECTS_ONLY_INHERIT
};

// массив строк, содержащие описание для возможных значений поля grfInheritance
constexpr LPCTSTR szInheritText[7] = {
	TEXT("Только для этого каталога"),
	TEXT("Для этого каталога, его подкаталогов и файлов"),
	TEXT("Для этого каталога и его подкаталогов"),
	TEXT("Для этого каталога и его файлов"),
	TEXT("Только для подкаталогов и файлов"),
	TEXT("Только для подкаталогов"),
	TEXT("Только для файлов")
};

// массив разрешений для файла/каталога
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

// массив идентификаторов флажков для разрешений
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



/*Перевод в ListView*/
BOOL ListViewInit(LPTSTR path, HWND hwnd);
/*Var*/
RECT rect = { 0 }; // размер и положение окна

HANDLE hFile = INVALID_HANDLE_VALUE; // дескриптор редактируемого текстового файла
LPCTSTR lpszFileName = NULL; // указатель на имя файла/каталога
DWORD cchPath = 0; // длина пути к файлу/каталогу
HKEY hKey = NULL; // дескриптор ключа рееста

/**/
PSECURITY_DESCRIPTOR Sec_Descriptor;//дескриптор безопасности
TCHAR FileName[MAX_PATH] = TEXT(""); // путь до редактируемого файла/папки

/*Функции*/
BOOL GetFileSecurityDescriptor(LPCWSTR lpFileName, SECURITY_INFORMATION RequestedInformation, PSECURITY_DESCRIPTOR *ppSD);
BOOL GetItemFromDACL(PSECURITY_DESCRIPTOR Sec_Descriptor, PULONG pcCountOfEntries, PEXPLICIT_ACCESS *pListOfEntries);
BOOL GetAccountName_W(PSID psid, LPWSTR* AccountName);
BOOL GetAccountSID_W(LPCWSTR AccountName, PSID *ppsid); //узнаем SID ПК ИЛИ USER
BOOL GetOwnerName_W(PSECURITY_DESCRIPTOR Sec_Descriptor, LPWSTR *OwnerName);
BOOL SetFileSecurityInfo(LPCTSTR FileName, LPWSTR NewOwner, ULONG CountOfEntries, PEXPLICIT_ACCESS pListOfEntries, BOOL bMergeEntries);
BOOL DeleteEntryFromDalc(PSECURITY_DESCRIPTOR pSD, DWORD dwIndex);