#pragma once
#include <windows.h>
#include <iostream>
#include <stdio.h>
#include <conio.h>

#include <WindowsX.h>
#include <tchar.h>
#include <CommCtrl.h>
#include <strsafe.h>
#include <wchar.h>
#include <shlobj.h>
#include <shlwapi.h>
#include <shobjidl.h>//for browserinfo
#include <fileapi.h>
#include <string>
#include "resource.h"
#include <LMCons.h>

#include <shlwapi.h>

using namespace std;
#pragma warning(disable : 4996) //��������� ������ deprecate. ���������, ����� ������������ ���������� ������
#pragma comment(linker,"\"/manifestdependency:type='win32' \
name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#pragma comment (lib, "shlwapi.lib")

BOOL CopyDirectoryContent_Dir(LPCTSTR szInDirName, LPCTSTR szOutDirName);
BOOL CopyDirectoryContent(LPCTSTR szInDirName, LPCTSTR szOutDirName);
BOOL DirectoryExists(LPCTSTR szPath); //�������� �������������
// ��������� ����������� ���� 
INT_PTR CALLBACK DialogProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK MainWindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
// ���������� ��������� WM_INITDIALOG
BOOL Dialog_OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam);
// ���������� ��������� WM_CLOSE
void Dialog_OnClose(HWND hwnd);
// ���������� ��������� WM_COMMAND
void Dialog_OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify);

HWND hDlg = NULL;
HWND hwnd = NULL;