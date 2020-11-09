#include <Windows.h>
#include <tchar.h>
#include <stdio.h>
#include <locale.h>
#include <strsafe.h>
#include <iostream>
#include <shlwapi.h>
#pragma comment(lib, "shlwapi.lib")

typedef BOOL(__stdcall *LPSEARCHFUNC)(LPCTSTR lpszFileName, const LPWIN32_FILE_ATTRIBUTE_DATA lpFileAttributeData, LPVOID lpvParam);

// функция, используемая для копирования файлов и каталогов
BOOL __stdcall FileCopy(LPCTSTR lpszFileName, const LPWIN32_FILE_ATTRIBUTE_DATA lpFileAttributeData, LPVOID lpvParam);

// функция, выполняющая операцию с файлом или каталогом
BOOL FileOperation(LPCTSTR lpszFileName, LPCTSTR lpTargetDirectory, LPSEARCHFUNC lpFunc);

// функция, которая выполняет поиск внутри каталога
BOOL FileSearch(LPCTSTR lpszFileName, LPCTSTR lpszDirName, LPSEARCHFUNC lpSearchFunc, LPVOID lpvParam);

int _tmain(int argc, LPTSTR argv[])
{
	setlocale(LC_ALL, "");
	if ((4 == argc) && (_tcscmp(argv[1], TEXT("/copy")) == 0)) // копирование файлов и каталогов
	{
		std::cout << "> Копирование" << argv[2] << " в " << argv[3] << "\n";

		BOOL bRet = FileOperation(argv[2], argv[3], FileCopy);

		if (FALSE != bRet)
			std::cout << "> Успешно" << "\n";
		else
			std::cout << "> Код ошибки"<< GetLastError()<<"\n";
	} 
	else
	{
		std::cout<<"Формат ввода: /copy <имя файла> <каталог>\n\n\ \t<имя файла> - имя копируемого файла или каталога\n\n\t<каталог> - каталог, в который нужно скопировать файл или каталог\n\n";
	} 
} 


BOOL __stdcall FileCopy(LPCTSTR lpszFileName, const LPWIN32_FILE_ATTRIBUTE_DATA lpFileAttributeData, LPVOID lpvParam)
{
	LPCTSTR lpTargetDirectory = (LPCTSTR)lpvParam; // каталог, в который нужно скопировать файл/каталог

	TCHAR szNewFileName[MAX_PATH]; // новое имя файла/каталога
	StringCchPrintf(szNewFileName, _countof(szNewFileName), TEXT("%s\\%s"), (LPCTSTR)lpTargetDirectory, PathFindFileName(lpszFileName));

	if (lpFileAttributeData->dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) // нужно скопировать каталог
	{
		// создаём новый каталог (атрибуты копируются)
		BOOL bRet = CreateDirectoryEx(lpszFileName, szNewFileName, NULL);

		if ((FALSE != bRet) || (GetLastError() == ERROR_ALREADY_EXISTS)) // (!) ошибка: Невозможно создать каталог, так как он уже существует. 
		{
			// продолжим поиск внутри каталога
			bRet = FileSearch(TEXT("*"), lpszFileName, FileCopy, szNewFileName);
		} // if

		return bRet;
	} // if

	// копируем файл
	return CopyFile(lpszFileName, szNewFileName, FALSE);
} // FileCopy

// ----------------------------------------------------------------------------------------------
BOOL FileOperation(LPCTSTR lpszFileName, LPCTSTR lpTargetDirectory, LPSEARCHFUNC lpFunc)
{
	if (NULL != lpTargetDirectory)
	{
		DWORD dwFileAttributes = GetFileAttributes(lpTargetDirectory);

		if (INVALID_FILE_ATTRIBUTES == dwFileAttributes) // (!) ошибка
		{
			return FALSE;
		} // if
		else if ((dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0) // (!) не является каталогом
		{
			SetLastError(ERROR_PATH_NOT_FOUND);
			return FALSE;
		} // if
	} // if

	WIN32_FILE_ATTRIBUTE_DATA fad;

	// получим атрибутивную информацию файла/каталога
	BOOL bRet = GetFileAttributesEx(lpszFileName, GetFileExInfoStandard, &fad);

	if (FALSE != bRet)
	{
		// выполняем операцию с файлом/каталогом
		bRet = lpFunc(lpszFileName, &fad, (LPVOID)lpTargetDirectory);
	} // if

	return bRet;
} // FileOperation

// ----------------------------------------------------------------------------------------------
BOOL FileSearch(LPCTSTR lpszFileName, LPCTSTR lpszDirName, LPSEARCHFUNC lpSearchFunc, LPVOID lpvParam)
{
	WIN32_FIND_DATA fd;
	TCHAR szFileName[MAX_PATH];

	// формируем шаблон поиска
	StringCchPrintf(szFileName, MAX_PATH, TEXT("%s\\%s"), lpszDirName, lpszFileName);

	// начинаем поиск
	HANDLE hFindFile = FindFirstFile(szFileName, &fd);
	if (INVALID_HANDLE_VALUE == hFindFile) return FALSE;

	BOOL bRet = TRUE;

	for (BOOL bFindNext = TRUE; FALSE != bFindNext; bFindNext = FindNextFile(hFindFile, &fd))
	{
		if (_tcscmp(fd.cFileName, TEXT(".")) == 0 || _tcscmp(fd.cFileName, TEXT("..")) == 0)
		{
			/* пропускаем текущий и родительский каталог */
			continue;
		} // if

		// формируем полный путь к файлу
		StringCchPrintf(szFileName, MAX_PATH, TEXT("%s\\%s"), lpszDirName, fd.cFileName);

		bRet = lpSearchFunc(szFileName, (LPWIN32_FILE_ATTRIBUTE_DATA)&fd, lpvParam);
		if (FALSE == bRet) break; // прерываем поиск
	} // for

	FindClose(hFindFile); // завершаем поиск
	return bRet;
} // FileSearch