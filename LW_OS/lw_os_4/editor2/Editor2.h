#pragma once
#include <Windows.h>
#include <WindowsX.h>
#include <tchar.h>
#include <strsafe.h>
#include <richedit.h> //why ?
#include "resource.h"
#include <atlconv.h>
#include <string>


// ������� ��������� �������� ����
LRESULT CALLBACK MainWindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
/*����������� ��������� WM_CREATE WM_DESTROY WM_SIZE WM_COMMAND */

BOOL OnCreate(HWND hwnd, LPCREATESTRUCT lpCreateStruct);
void OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify);

/*������ � ������� �������������*/
void LoadProfile(LPCTSTR lpFileName);//�������� ����������
void SaveProfile(LPCTSTR lpFileName);//���������� ����������

/*����������� �������� ����/������*/
BOOL OpenFileAsync(HWND hwndCtl); //�������� � ������
BOOL SaveFileAsync(HWND hwndCtl, BOOL fSaveAs = FALSE);// �������� � ������

/*����������� �������� ������/������*/
BOOL ReadAsync(HANDLE hFile, LPVOID lpBuffer, DWORD dwOffset, DWORD dwSize, LPOVERLAPPED lpOverlapped);//������
BOOL WriteAsync(HANDLE hFile, LPCVOID lpBuffer, DWORD dwOffset, DWORD dwSize, LPOVERLAPPED lpOverlapped);//������

/*������� �������� � �������� ����/������*/

BOOL FinishIo(LPOVERLAPPED lpOverlapped);
BOOL TryFinishIo(LPOVERLAPPED lpOverlapped);

// �������, ������� ���������� � ����� ��������� ���������,
// ���� � ������� ��� ���������
// ��� �������������
void OnIdle(HWND hwnd);

/*����������*/
POINT WindowPosition; // ��������� ����
SIZE WindowSize; // ������ ����

WCHAR FileName[MAX_PATH] = L""; // ��� �������������� ���������� �����
HANDLE hFile = INVALID_HANDLE_VALUE; // ���������� �������������� ���������� �����

CHARFORMAT cf;//��������� ��������
PARAFORMAT pf;//��������� �������
LOGFONT logFont; // ��������� ������
HFONT hFont = NULL; // ���������� ������

LPWSTR lpBuffReWri = NULL; // ��������� �� ����� ��� ������/������ ���������� �����
OVERLAPPED ovlRead = { 0 }, ovlWrite = { 0 };// ��������� ���������� ����������� �������� � �����

