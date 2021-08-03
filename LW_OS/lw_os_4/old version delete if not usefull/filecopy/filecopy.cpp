#include <Windows.h>
#include <tchar.h>
#include <stdio.h>
#include <locale.h>
#include <strsafe.h>
#include <iostream>
#include <io.h>
#include <time.h>
#include <shlwapi.h>
#pragma comment(lib, "shlwapi.lib")

typedef BOOL(__stdcall *LPSEARCHFUNC)(LPCTSTR lpszFileName, const LPWIN32_FILE_ATTRIBUTE_DATA lpFileAttributeData, LPVOID lpvParam);

// �������, ������������ ��� ����������� ������ � ���������
BOOL __stdcall FileCopy(LPCTSTR lpszFileName, const LPWIN32_FILE_ATTRIBUTE_DATA lpFileAttributeData, LPVOID lpvParam);

// �������, ����������� �������� � ������ ��� ���������
BOOL FileOperation(LPCTSTR lpszFileName, LPCTSTR lpTargetDirectory, LPSEARCHFUNC lpFunc);

// �������, ������� ��������� ����� ������ ��������
BOOL FileSearch(LPCTSTR lpszFileName, LPCTSTR lpszDirName, LPSEARCHFUNC lpSearchFunc, LPVOID lpvParam);

int _tmain(int argc, LPTSTR argv[])
{
	setlocale(LC_ALL, "");
	std::cout << ">������ ���������\n" << std::endl;
	do 
	{
	if ((4 == argc) && (_tcscmp(argv[1], TEXT("/copy")) == 0)) // ����������� ������ � ���������
	{
		std::cout << "> �����������" << argv[2] << " � " << argv[3] << "\n";
		/*�������� ���������� ��������� ������ ��� �����������*/
		BOOL bRet = FileOperation(argv[2], argv[3], FileCopy);

		if (FALSE != bRet)
			std::cout << "> �������" << "\n";
		else
			std::cout << "> ��� ������"<< GetLastError()<<"\n";
	} 
	else
	{
		std::cout<<"������ �����: /copy <��� �����> <�������>\n\n\ \t<��� �����> - ��� ����������� ����� ��� ��������\n\n\t<�������> - �������, � ������� ����� ����������� ���� ��� �������\n\n";
	} 
	} while (argv[1] != TEXT("�����"));
	
} 
BOOL FileOperation(LPCTSTR lpszFileName, LPCTSTR lpTargetDirectory, LPSEARCHFUNC lpFunc)
{
	if (NULL != lpTargetDirectory)
	{
		DWORD dwFileAttributes = GetFileAttributes(lpTargetDirectory);

		if (INVALID_FILE_ATTRIBUTES == dwFileAttributes) // (!) ������
		{
			return FALSE;
		} 
		else if ((dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0) // (!) �� �������� ���������
		{
			SetLastError(ERROR_PATH_NOT_FOUND);
			return FALSE;
		} 
	} 

	WIN32_FILE_ATTRIBUTE_DATA fad;

	// ������� ������������ ���������� �����/��������
	BOOL bRet = GetFileAttributesEx(lpszFileName, GetFileExInfoStandard, &fad);

	if (FALSE != bRet)
	{
		// ��������� �������� � ������/���������
		bRet = lpFunc(lpszFileName, &fad, (LPVOID)lpTargetDirectory);
	} 

	return bRet;
} 

BOOL __stdcall FileCopy(LPCTSTR lpszFileName, const LPWIN32_FILE_ATTRIBUTE_DATA lpFileAttributeData, LPVOID lpvParam)
{
	LPCTSTR lpTargetDirectory = (LPCTSTR)lpvParam; // �������, � ������� ����� ����������� ����/�������

	TCHAR NewFileName[MAX_PATH]; // ����� ��� �����/��������
	StringCchPrintf(NewFileName, _countof(NewFileName), TEXT("%s\\%s"), (LPCTSTR)lpTargetDirectory, PathFindFileName(lpszFileName));

	if (lpFileAttributeData->dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) 
	{
		// ������ ����� ������� 
		BOOL Ret = CreateDirectoryEx(lpszFileName, NewFileName, NULL);

		if ((Ret != FALSE) || (GetLastError() == ERROR_ALREADY_EXISTS)) // (!) ������: ���������� ������� �������, ��� ��� �� ��� ����������. 
		{
			// ��������� ����� ������ ��������
			Ret = FileSearch(TEXT("*"), lpszFileName, FileCopy, NewFileName);
		}

		return Ret;
	} 
	return CopyFile(lpszFileName, NewFileName, FALSE);
} 




// ----------------------------------------------------------------------------------------------
BOOL FileSearch(LPCTSTR lpszFileName, LPCTSTR lpszDirName, LPSEARCHFUNC lpSearchFunc, LPVOID lpvParam)
{
	
	WIN32_FIND_DATA  fd;
	TCHAR FileName[MAX_PATH];

	//path for search
	//���� �� �������� ����
	StringCchPrintf(FileName, MAX_PATH, TEXT("%s\\%s"), lpszDirName, lpszFileName);

	// ������� ������ ����
	HANDLE hFindFile = FindFirstFile(FileName, &fd);
	if (hFindFile == INVALID_HANDLE_VALUE) return FALSE; //file not found

	BOOL bRet = TRUE;

	for (BOOL bFindNext = TRUE; FALSE != bFindNext; bFindNext = FindNextFile(hFindFile, &fd))
	{
		//���������� ������� � ������������ �������
		//ignore parent and current directory
		if (_tcscmp(fd.cFileName, TEXT(".")) == 0 || _tcscmp(fd.cFileName, TEXT("..")) == 0)
		{
			continue;
		} 
		// ��������� ������ ���� � �����
		StringCchPrintf(FileName, MAX_PATH, TEXT("%s\\%s"), lpszDirName, fd.cFileName);

		bRet = lpSearchFunc(FileName, (LPWIN32_FILE_ATTRIBUTE_DATA)&fd, lpvParam);
		if (bRet == FALSE) break; // ��������� �����
	} 
		
	FindClose(hFindFile); //close handle of search //�������� ����������� ������
	return bRet;
}