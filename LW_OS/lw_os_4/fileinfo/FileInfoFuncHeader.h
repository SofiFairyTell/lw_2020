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

#define IDC_EDIT_TEXT        2001

#pragma warning(disable : 4996) //��������� ������ deprecate. ���������, ����� ������������ ���������� ������
#pragma comment(linker,"\"/manifestdependency:type='win32' \
name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

#pragma comment(lib, "shlwapi.lib")
// ������� ��������� �������� ����
LRESULT CALLBACK MainWindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
/*����������� ��������� WM_CREATE WM_DESTROY WM_SIZE WM_COMMAND */

BOOL OnCreate(HWND hwnd, LPCREATESTRUCT lpCreateStruct);
void OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify);

/*������� �������*/
BOOL GetFileTimeFormat(const LPFILETIME lpFileTime, LPTSTR lpszFileTime, DWORD cchFileTime);

/*������� �����*/
void ConvertFileSize(LPTSTR lpszBuffer, DWORD cch, LARGE_INTEGER size);
void ConvertDirectSize(LPTSTR lpszBuffer, DWORD cch, ULARGE_INTEGER size);

/*������� ������ �����*/
BOOL __stdcall CalculateSize(LPCTSTR lpszFileName, const LPWIN32_FILE_ATTRIBUTE_DATA lpFileAttributeData, LPVOID lpvParam);
typedef BOOL(__stdcall *LPSEARCHFUNC)(LPCTSTR lpszFileName, const LPWIN32_FILE_ATTRIBUTE_DATA lpFileAttributeData, LPVOID lpvParam);
BOOL FileSearch(LPCTSTR lpszFileName, LPCTSTR path, LPSEARCHFUNC lpSearchFunc, LPVOID lpvParam);

/*������ � ��������*/

LSTATUS RegGetValueBinary(HKEY hKey, LPCTSTR lpValueName, LPBYTE lpData, DWORD cb, LPDWORD lpcbNeeded);
LSTATUS RegGetValueSZ(HKEY hKey, LPCTSTR lpValueName, LPTSTR lpszData, DWORD cch, LPDWORD lpcchNeeded);

/*������� � ListView*/
BOOL ListViewInit(LPTSTR path, HWND hwnd);
/*Var*/
RECT rect = { 0 }; // ������ � ��������� ����
TCHAR FileName[MAX_PATH] = TEXT(""); // ���� �� �������������� �����/�����
HANDLE hFile = INVALID_HANDLE_VALUE; // ���������� �������������� ���������� �����
LPCTSTR lpszFileName = NULL; // ��������� �� ��� �����/��������
DWORD cchPath = 0; // ����� ���� � �����/��������
HKEY hKey = NULL; // ���������� ����� ������
