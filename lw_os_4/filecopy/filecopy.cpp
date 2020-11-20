#include <windows.h>
#include <iostream>
#include <stdio.h>
#include <conio.h>

#include <WindowsX.h>
#include <tchar.h>
#include <CommCtrl.h>
#include <strsafe.h>

#include <shlobj.h>
#include <shlwapi.h>
#include <shobjidl.h>//for browserinfo
#include <fileapi.h>
#include <string>
#include <wchar.h>

using namespace std;
#pragma warning(disable : 4996) //отключает Ошибку deprecate. Возникает, когда используется устаревшая функци
#pragma comment(linker,"\"/manifestdependency:type='win32' \
name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#pragma comment (lib, "shlwapi.lib")


typedef BOOL(__stdcall *LPSEARCHFUNC)(LPCTSTR lpszFileName, const LPWIN32_FILE_ATTRIBUTE_DATA lpFileAttributeData, LPVOID lpvParam);
BOOL Copy(LPCTSTR lpszFileName, const LPWIN32_FILE_ATTRIBUTE_DATA lpFileAttributeData, LPVOID lpvParam);
BOOL FileSearch(LPCTSTR lpszFileName, LPCTSTR path, LPSEARCHFUNC lpSearchFunc, LPVOID lpvParam);
BOOL FileOperation(LPCTSTR lpszFileName, LPCTSTR lpTargetDirectory, LPSEARCHFUNC lpFunc);


BOOL Copy(LPCTSTR lpszFileName, const LPWIN32_FILE_ATTRIBUTE_DATA lpFileAttributeData, LPVOID lpvParam)
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
			bRet = FileSearch(TEXT("*"), lpszFileName, Copy, szNewFileName);
		} // if

		return bRet;
	} // if

	// копируем файл
	return CopyFile(lpszFileName, szNewFileName, FALSE);
}


BOOL FileSearch(LPCTSTR lpszFileName, LPCTSTR path, LPSEARCHFUNC lpSearchFunc, LPVOID lpvParam)
{
	WIN32_FIND_DATA ffd;
	LARGE_INTEGER filesize;

	TCHAR szDir[MAX_PATH];
	//TCHAR Path[MAX_PATH];

	HANDLE hFind = INVALID_HANDLE_VALUE;
	StringCchCopy(szDir, MAX_PATH, path);
	StringCchCat(szDir, MAX_PATH, TEXT("\\*.*"));
	hFind = FindFirstFile(szDir, &ffd);

	BOOL bRet = TRUE;

	if (INVALID_HANDLE_VALUE == hFind)
	{
		//error
	}
	do
	{
		if ((0 == lstrcmp(ffd.cFileName, L".")) ||
			(0 == lstrcmp(ffd.cFileName, L"..")))
		{
			continue;
		}
		// формируем полный путь к файлу
		StringCchPrintf(szDir, MAX_PATH, TEXT("%s\\%s"), path, ffd.cFileName);

		if (!(ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
		{
			bRet = lpSearchFunc(szDir, (LPWIN32_FILE_ATTRIBUTE_DATA)&ffd, lpvParam);
			if (FALSE == bRet) break; // прерываем поиск
		}
	} while (FindNextFile(hFind, &ffd) != 0);

	FindClose(hFind);
	return TRUE;
}

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


int _tmain(int argc, LPTSTR argv[])
{
	_tsetlocale(LC_ALL, TEXT(""));
	BOOL bRet = FileOperation(L"C:\\test\\libr1", L"C:\\test\\libr", Copy);
	if (FALSE != bRet) _tprintf(TEXT("> Успешно!\n"));
		else _tprintf(TEXT("> Ошибка: %d\n"), GetLastError());
	//} // if

} // _tmain




//
//#include <windows.h>
//
//void Copy(LPCTSTR szInDirName, LPCTSTR szOutDirName, bool flag = false)
//{
//	WIN32_FIND_DATA ffd;
//	HANDLE hFind;
//
//	TCHAR szFind[MAX_PATH + 1];
//	TCHAR szInFileName[MAX_PATH + 1];
//	TCHAR szOutFileName[MAX_PATH + 1];
//
//	lstrcpy(szFind, szInDirName);
//	lstrcat(szFind, L"\\*.*"); //ищем файлы с любым именем и рысширением
//
//	hFind = FindFirstFile(szFind, &ffd);
//
//	do
//	{
//		//Формируем полный путь (источник)
//		lstrcpy(szInFileName, szInDirName);
//		lstrcat(szInFileName, L"\\");
//		lstrcat(szInFileName, ffd.cFileName);
//
//		//Формируем полный путь (результат)
//		lstrcpy(szOutFileName, szOutDirName);
//		lstrcat(szOutFileName, L"\\");
//		lstrcat(szOutFileName, ffd.cFileName);
//
//		if (flag) //если flag == true, то копируем и папки
//		{
//			if (ffd.dwFileAttributes & 0x00000010)
//			{
//				if (lstrcmp(ffd.cFileName, L".") == 0 ||
//					lstrcmp(ffd.cFileName, L"..") == 0) continue;
//
//				CreateDirectory(szOutFileName, NULL);
//				Copy(szInFileName, szOutFileName);
//			}
//		} //иначе пропускаем папки
//		else
//			if (ffd.dwFileAttributes & 0x00000010) continue;
//
//		CopyFile(szInFileName, szOutFileName, TRUE);
//	} while (FindNextFile(hFind, &ffd));
//
//	FindClose(hFind);
//}
//
//int main()
//{
//	//В конце пути к папкам не нужно добавлять "\"
//	Copy(L"C:\\test\\libr1", L"C:\\test\\libr", true); //если нужно скопировать и поддиректории
//	//Copy(L"C:\\test\\libr1", L"C:\\test\\libr"); //если нужно скопировать только файлы
//}
