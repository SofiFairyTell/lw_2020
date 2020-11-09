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

// функция, используемая для копирования файлов и каталогов
BOOL __stdcall FileCopy(LPCTSTR lpszFileName, const LPWIN32_FILE_ATTRIBUTE_DATA lpFileAttributeData, LPVOID lpvParam);

// функция, выполняющая операцию с файлом или каталогом
BOOL FileOperation(LPCTSTR lpszFileName, LPCTSTR lpTargetDirectory, LPSEARCHFUNC lpFunc);

// функция, которая выполняет поиск внутри каталога
BOOL FileSearch(LPCTSTR lpszFileName, LPCTSTR lpszDirName, LPSEARCHFUNC lpSearchFunc, LPVOID lpvParam);

int _tmain(int argc, LPTSTR argv[])
{
	setlocale(LC_ALL, "");
	std::cout << ">Начало программы\n" << std::endl;
	do 
	{
	if ((4 == argc) && (_tcscmp(argv[1], TEXT("/copy")) == 0)) // копирование файлов и каталогов
	{
		std::cout << "> Копирование" << argv[2] << " в " << argv[3] << "\n";
		/*передача аргументов командной строки для копирования*/
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
	} while (argv[1] != TEXT("выход"));
	
} 
BOOL FileOperation(LPCTSTR lpszFileName, LPCTSTR lpTargetDirectory, LPSEARCHFUNC lpFunc)
{
	if (NULL != lpTargetDirectory)
	{
		DWORD dwFileAttributes = GetFileAttributes(lpTargetDirectory);

		if (INVALID_FILE_ATTRIBUTES == dwFileAttributes) // (!) ошибка
		{
			return FALSE;
		} 
		else if ((dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0) // (!) не является каталогом
		{
			SetLastError(ERROR_PATH_NOT_FOUND);
			return FALSE;
		} 
	} 

	WIN32_FILE_ATTRIBUTE_DATA fad;

	// получим атрибутивную информацию файла/каталога
	BOOL bRet = GetFileAttributesEx(lpszFileName, GetFileExInfoStandard, &fad);

	if (FALSE != bRet)
	{
		// выполняем операцию с файлом/каталогом
		bRet = lpFunc(lpszFileName, &fad, (LPVOID)lpTargetDirectory);
	} // if

	return bRet;
} 

BOOL __stdcall FileCopy(LPCTSTR lpszFileName, const LPWIN32_FILE_ATTRIBUTE_DATA lpFileAttributeData, LPVOID lpvParam)
{
	LPCTSTR lpTargetDirectory = (LPCTSTR)lpvParam; // каталог, в который нужно скопировать файл/каталог

	TCHAR NewFileName[MAX_PATH]; // новое имя файла/каталога
	StringCchPrintf(NewFileName, _countof(NewFileName), TEXT("%s\\%s"), (LPCTSTR)lpTargetDirectory, PathFindFileName(lpszFileName));

	if (lpFileAttributeData->dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) 
	{
		// создаём новый каталог 
		BOOL Ret = CreateDirectoryEx(lpszFileName, NewFileName, NULL);

		if ((Ret != FALSE) || (GetLastError() == ERROR_ALREADY_EXISTS)) // (!) ошибка: Невозможно создать каталог, так как он уже существует. 
		{
			// продолжим поиск внутри каталога
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
	//путь по которому ищем
	StringCchPrintf(FileName, MAX_PATH, TEXT("%s\\%s"), lpszDirName, lpszFileName);

	// находим первый файл
	HANDLE hFindFile = FindFirstFile(FileName, &fd);
	if (hFindFile == INVALID_HANDLE_VALUE) return FALSE; //file not found

	BOOL bRet = TRUE;

	for (BOOL bFindNext = TRUE; FALSE != bFindNext; bFindNext = FindNextFile(hFindFile, &fd))
	{
		//игнорируем текущий и родительский каталог
		//ignore parent and current directory
		if (_tcscmp(fd.cFileName, TEXT(".")) == 0 || _tcscmp(fd.cFileName, TEXT("..")) == 0)
		{
			continue;
		} 
		// формируем полный путь к файлу
		StringCchPrintf(FileName, MAX_PATH, TEXT("%s\\%s"), lpszDirName, fd.cFileName);

		bRet = lpSearchFunc(FileName, (LPWIN32_FILE_ATTRIBUTE_DATA)&fd, lpvParam);
		if (bRet == FALSE) break; // прерываем поиск
	} 
		
	FindClose(hFindFile); //close handle of search //закрытие дескриптора поиска
	return bRet;
}