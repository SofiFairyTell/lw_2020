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

#define IDC_EDIT_TEXT        2001

#pragma warning(disable : 4996) //отключает Ошибку deprecate. Возникает, когда используется устаревшая функци
#pragma comment(linker,"\"/manifestdependency:type='win32' \
name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#pragma comment(lib, "shlwapi.lib")
// оконная процедура главного окна
LRESULT CALLBACK MainWindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
/*Обработчики сообщений WM_CREATE WM_DESTROY WM_SIZE WM_COMMAND */

BOOL OnCreate(HWND hwnd, LPCREATESTRUCT lpCreateStruct);
void OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify);

/*Перевод времени*/
BOOL GetFileTimeFormat(const LPFILETIME lpFileTime, LPTSTR lpszFileTime, DWORD cchFileTime);

/*Перевод чисел*/
void PrintFileSize(LPTSTR lpszBuffer, DWORD cch, LARGE_INTEGER size);
void PrintDirectSize(LPTSTR lpszBuffer, DWORD cch, ULARGE_INTEGER size);

/*Считать размер папки*/
BOOL __stdcall CalculateSize(LPCTSTR lpszFileName, const LPWIN32_FILE_ATTRIBUTE_DATA lpFileAttributeData, LPVOID lpvParam);
typedef BOOL(__stdcall *LPSEARCHFUNC)(LPCTSTR lpszFileName, const LPWIN32_FILE_ATTRIBUTE_DATA lpFileAttributeData, LPVOID lpvParam);
BOOL FileSearch(LPCTSTR lpszFileName, LPCTSTR path, LPSEARCHFUNC lpSearchFunc, LPVOID lpvParam);

/*Перевод в ListView*/
BOOL ListViewInit(LPTSTR path , HWND hwnd);
/*Var*/
RECT rect = { 0 }; // размер и положение окна
TCHAR FileName[MAX_PATH] = TEXT(""); // имя редактируемого текстового файла
HANDLE hFile = INVALID_HANDLE_VALUE; // дескриптор редактируемого текстового файла
LPCTSTR lpszFileName = NULL; // указатель на имя файла/каталога
DWORD cchPath = 0; // длина пути к файлу/каталогу


int WINAPI _tWinMain(HINSTANCE hInstance, HINSTANCE, LPTSTR lpszCmdLine, int nCmdShow)
{
					
	HACCEL hAccel = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDR_ACCELERATOR1));

	WNDCLASSEX wcex = { sizeof(WNDCLASSEX) };

	wcex.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
	wcex.lpfnWndProc = MainWindowProc; // оконная процедура
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_BTNFACE + 2);
	wcex.lpszMenuName = MAKEINTRESOURCE(IDR_MENU1);
	wcex.lpszClassName = TEXT("MainWindowClass"); // имя класса
	wcex.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

	LoadLibrary(TEXT("ComCtl32.dll"));//для элементов общего пользования		

	
	if (0 == RegisterClassEx(&wcex)) // регистрируем класс
	{
		return -1; // завершаем работу приложения
	}
	RECT wr = { 0, 0, 500, 500 };    // set the size, but not the position

	// создаем главное окно на основе нового оконного класса
	HWND hWnd = CreateWindowEx(0, TEXT("MainWindowClass"), TEXT("Process"), WS_OVERLAPPEDWINDOW^WS_THICKFRAME^WS_MINIMIZEBOX^WS_MAXIMIZEBOX, 300,300,
		wr.right - wr.left,   wr.bottom - wr.top, NULL, NULL, hInstance, NULL);

	if (NULL == hWnd)
	{
		return -1; // завершаем работу приложения
	}

	ShowWindow(hWnd, nCmdShow); // отображаем главное окно




	MSG  msg;
	BOOL Ret;

	for (;;)
	{

		// извлекаем сообщение из очереди
		Ret = GetMessage(&msg, NULL, 0, 0);
		if (Ret == FALSE)
		{
			break; // получено WM_QUIT, выход из цикла
		}
		else if (!TranslateAccelerator(hWnd, hAccel, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}


	return (int)msg.wParam;
}

LRESULT CALLBACK MainWindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
		HANDLE_MSG(hwnd, WM_CREATE, OnCreate);
		HANDLE_MSG(hwnd, WM_COMMAND, OnCommand);
	case WM_SIZE:
	{
		HWND hwndCtl = GetDlgItem(hwnd, IDC_EDIT_TEXT);
		MoveWindow(hwndCtl, 0, 0, LOWORD(lParam), HIWORD(lParam), TRUE); // изменяем размеры поля ввода
	}

	break;
	case WM_DESTROY:
	{
		PostQuitMessage(0); // отправляем сообщение WM_QUIT
	}break;
	case WM_CLOSE:

		DestroyWindow(hwnd); // уничтожаем окно
		break;
	}
	return DefWindowProc(hwnd, msg, wParam, lParam);
}



BOOL OnCreate(HWND hwnd, LPCREATESTRUCT lpCRStr)
{
	CreateWindowEx(0, TEXT("Edit"),NULL, WS_CHILD | WS_VISIBLE | WS_BORDER, 30, 10, 400, 30, hwnd, (HMENU)IDC_EDIT_FILENAME, lpCRStr->hInstance, NULL);
	HWND hwndLV = CreateWindowEx(0, TEXT("SysListView32"), NULL,WS_CHILD | WS_VISIBLE | WS_BORDER | LVS_REPORT | LVS_SHOWSELALWAYS , 30, 40, 400, 150, hwnd, (HMENU)IDC_LIST1, lpCRStr->hInstance, NULL);
	
	
	//значения атрибутов
	CreateWindowEx(0, TEXT("button"), TEXT("Только для чтения"), WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX, 30, 200, 400, 30, hwnd, (HMENU)IDC_ATTRIBUTE_READONLY, lpCRStr->hInstance, NULL);
	CreateWindowEx(0, TEXT("button"), TEXT("Скрытый"), WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX, 30, 230, 400, 30, hwnd, (HMENU)IDC_ATTRIBUTE_HIDDEN, lpCRStr->hInstance, NULL);
	CreateWindowEx(0, TEXT("button"), TEXT("Файл готов для архивирирования"), WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX, 30, 260, 400, 30, hwnd, (HMENU)IDC_ATTRIBUTE_ARCHIVE, lpCRStr->hInstance, NULL);
	CreateWindowEx(0, TEXT("button"), TEXT("Системный"), WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX, 30, 290, 400, 30, hwnd, (HMENU)IDC_ATTRIBUTE_SYSTEM, lpCRStr->hInstance, NULL);
	CreateWindowEx(0, TEXT("button"), TEXT("Временный"), WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX, 30, 320, 400, 30, hwnd, (HMENU)IDC_ATTRIBUTE_TEMPORARY, lpCRStr->hInstance, NULL);
	CreateWindowEx(0, TEXT("button"), TEXT("Сжимать для экономии места"), WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX, 30, 350, 400, 30, hwnd, (HMENU)IDC_ATTRIBUTE_COMPRESSED, lpCRStr->hInstance, NULL);
	CreateWindowEx(0,TEXT("button"), TEXT("Шифровать содержимое для защиты"), WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX,30, 380, 400, 30, hwnd,(HMENU)IDC_ATTRIBUTE_ENCRYPTED,lpCRStr->hInstance, NULL);

	// задем расширенный 
	ListView_SetExtendedListViewStyle(hwndLV, LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

	// вставляем три столбца в список просмотра

	LVCOLUMN lvColumns[] = {
		{ LVCF_WIDTH | LVCF_TEXT, 0, 200, (LPTSTR)TEXT("Свойство") },
		{ LVCF_WIDTH | LVCF_TEXT, 0, 200, (LPTSTR)TEXT("Значение") },
	};

	for (int i = 0; i < _countof(lvColumns); ++i)
	{
		// вставляем столбец
		ListView_InsertColumn(hwndLV, i, &lvColumns[i]);
	} 
	

	return TRUE;
}

void OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify)
{
	static HWND hEdit = GetDlgItem(hwnd, IDC_EDIT_TEXT);

	switch (id)
	{
	case ID_OPEN_FILE: // Открыть
		{
			OPENFILENAME openfile = { sizeof(OPENFILENAME) };

			openfile.hInstance = GetWindowInstance(hwnd);
			openfile.lpstrFilter = TEXT("Текстовые документы (*.txt)\0*.txt\0");
			openfile.lpstrFile = FileName;
			
			openfile.nMaxFile = _countof(FileName);
			openfile.lpstrTitle = TEXT("Открыть");
			openfile.Flags = OFN_EXPLORER | OFN_ENABLESIZING | OFN_FILEMUSTEXIST;
			openfile.lpstrDefExt = TEXT("txt");

			if (GetOpenFileName(&openfile) != FALSE)
			{
				// получаем информацию о файле
				//get info about file
				if (!(ListViewInit(FileName, hwnd)))
				{
					GetLastError();
					break;
				}
				else
				{
					break;
				}
			}
			else
			{
					MessageBox(NULL, TEXT("Не удалось открыть текстовый файл."), NULL, MB_OK | MB_ICONERROR);
					FileName[0] = _T('\0');
			}
		
		}
	break;
	case ID_OPEN_DIR://Открыть папку
	{
		BROWSEINFO bi;//structure for open special box with folder in treview
		TCHAR                   szDisplayName[MAX_PATH];//for name of path
		LPITEMIDLIST            pidl;
		LPMALLOC  pMalloc = NULL;

		ZeroMemory(&bi, sizeof(bi));
		bi.hwndOwner = NULL;
		bi.pszDisplayName = FileName;
		bi.lpszTitle = TEXT("Select folder");
		bi.ulFlags = BIF_RETURNONLYFSDIRS;
		
		pidl = SHBrowseForFolder(&bi);//open window for select
		if (pidl)
		{
			SHGetPathFromIDList(pidl, FileName);//get path

			if (!(ListViewInit(FileName, hwnd)))
			{
				GetLastError();
				break;
			}
		}
	}
		break;
	case ID_CHANGE_ATR://Изменение атрибутов
	{
		TCHAR NewFileName[MAX_PATH]; // новое имя файла/каталога
		GetDlgItemText(hwnd, IDC_EDIT_FILENAME, NewFileName, _countof(NewFileName));//это имя и его к указателю lpszFileName

			// найдём имя в пути к файлу/каталогу
		lpszFileName = PathFindFileName(FileName);

		// вычисляем длину пути к файлу/каталогу
		cchPath = (DWORD)(lpszFileName - FileName) - 1;
		// разделяем нуль-символом путь и имя файла/каталога
		FileName[cchPath] = _T('\0');

		if (CompareString(LOCALE_USER_DEFAULT, 0, lpszFileName, -1, NewFileName, -1) != CSTR_EQUAL) // (!) изменилось имя файла/каталога
		{
			TCHAR ExistingFileName[MAX_PATH]; // старое имя файла/каталога
			StringCchPrintf(ExistingFileName, _countof(ExistingFileName), TEXT("%s\\%s"), FileName, lpszFileName);

			// формируем новый путь к файлу/каталогу
			PathAppend(FileName, NewFileName);
			// переименовываем файл/каталог
			MoveFile(ExistingFileName, FileName);
		} // if
		else
		{
			// заменим нуль-символ, разделяющий путь и имя файла/каталога
			FileName[cchPath] = _T('\\');
		} // else

		// массив атрибутов
		constexpr DWORD attr[] = {
			FILE_ATTRIBUTE_READONLY, FILE_ATTRIBUTE_HIDDEN, FILE_ATTRIBUTE_ARCHIVE,
			FILE_ATTRIBUTE_SYSTEM, FILE_ATTRIBUTE_TEMPORARY, FILE_ATTRIBUTE_COMPRESSED, FILE_ATTRIBUTE_ENCRYPTED
		};

		// массив идентификаторов флажков для атрибутов
		constexpr DWORD ids[] = {
			IDC_ATTRIBUTE_READONLY, IDC_ATTRIBUTE_HIDDEN, IDC_ATTRIBUTE_ARCHIVE,
			IDC_ATTRIBUTE_SYSTEM, IDC_ATTRIBUTE_TEMPORARY, IDC_ATTRIBUTE_COMPRESSED, IDC_ATTRIBUTE_ENCRYPTED
		};

		DWORD dwFileAttributes = 0; // атрибуты файла/каталога

		// определяем, какие атрибуты выбраны

		for (int i = 0; i < _countof(attr); ++i)
		{
			if (IsDlgButtonChecked(hwnd, ids[i]) == BST_CHECKED) // флажок установлен
			{
				dwFileAttributes |= attr[i]; // добавим соответствующий атрибут
			} // if
		} // for
		
		// зададим атрибуты
		SetFileAttributes(FileName, dwFileAttributes);

		// запомним размер и положение окна
		GetWindowRect(hwnd, &rect);

		ListViewInit(FileName, hwnd);
	}
	
	break;

	case ID_EXIT:
		SendMessage(hwnd, WM_CLOSE, 0, 0);
		break;

	}
} 

BOOL ListViewInit(LPTSTR path, HWND hwnd)
{
	WIN32_FILE_ATTRIBUTE_DATA bhfi;
	TCHAR TimeBuffer[100], Buffer[100];
	if (!GetFileAttributesEx(path, GetFileExInfoStandard, &bhfi))
	{
		GetLastError();
	}

	//получение информации о размере файла
	//get info about size of file
	LARGE_INTEGER LI_Size;
	ULARGE_INTEGER sizeDir = { 0 };
	hFile = CreateFile(
		FileName,   // имя файла
		GENERIC_READ,          // чтение из файла
		FILE_SHARE_READ,       // совместный доступ к файлу
		NULL,                  // защиты нет
		OPEN_EXISTING,         // открываем существующий файл
		FILE_ATTRIBUTE_NORMAL,  // асинхронный ввод
		NULL                   // шаблона нет
	);
	if (!GetFileSizeEx(hFile, &LI_Size))
	{
		//обработка ошибки
	}
	if (bhfi.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
	{
		//расчет для папки 
		CalculateSize(path, &bhfi, &sizeDir);
		PrintDirectSize(Buffer, _countof(Buffer), sizeDir);
	}
	else
	{

		PrintFileSize(Buffer, _countof(Buffer), LI_Size);
		
	}

	LPTSTR lpFN = PathFindFileNameW(path);
	SetDlgItemText(hwnd, IDC_EDIT_FILENAME, lpFN);
	/*Работает, не трогать*/

	//Добавление найденных атрибутов в список просмотра
	HWND hwndLV = GetDlgItem(hwnd, IDC_LIST1);
	// добавляем новый элемент в список просмотра
	ListView_DeleteAllItems(hwndLV);
	LVITEM lvItem = { LVIF_TEXT | LVIF_PARAM };
	lvItem.iItem = ListView_GetItemCount(hwndLV);
	//Размер
	lvItem.pszText = (LPWSTR)(L"Размер:");
	lvItem.iItem = ListView_InsertItem(hwndLV, &lvItem);
	if ((lvItem.iItem != -1))
	{
		ListView_SetItemText(hwndLV, lvItem.iItem, 1, Buffer);
	}
	//третий параметр
	lvItem.pszText = (LPWSTR)(L"Время изменения:");
	lvItem.iItem = ListView_InsertItem(hwndLV, &lvItem);
	if ((lvItem.iItem != -1))
	{
		GetFileTimeFormat(&bhfi.ftCreationTime, TimeBuffer, _countof(TimeBuffer));//время создания
		ListView_SetItemText(hwndLV, lvItem.iItem, 1, TimeBuffer);
	}
	//второй параметр
	lvItem.pszText = (LPWSTR)(L"Время последнего обращения:");
	lvItem.iItem = ListView_InsertItem(hwndLV, &lvItem);
	if ((lvItem.iItem != -1))
	{
		GetFileTimeFormat(&bhfi.ftLastAccessTime, TimeBuffer, _countof(TimeBuffer));//время последнего обращения
		ListView_SetItemText(hwndLV, lvItem.iItem, 1, TimeBuffer);
	}
	//первый параметр
	lvItem.pszText = (LPWSTR)(L"Время создания:");
	lvItem.iItem = ListView_InsertItem(hwndLV, &lvItem);
	if ((lvItem.iItem != -1))
	{
		GetFileTimeFormat(&bhfi.ftLastWriteTime, TimeBuffer, _countof(TimeBuffer));//время изменения
		ListView_SetItemText(hwndLV, lvItem.iItem, 1, TimeBuffer);
	}
	//Расположение
	lvItem.pszText = (LPWSTR)(L"Расположение:");
	lvItem.iItem = ListView_InsertItem(hwndLV, &lvItem);
	if ((lvItem.iItem != -1))
	{
		ListView_SetItemText(hwndLV, lvItem.iItem, 1, path);
	}
	//Тип
	lvItem.pszText = (LPWSTR)(L"Тип:");
	lvItem.iItem = ListView_InsertItem(hwndLV, &lvItem);
	if ((lvItem.iItem != -1))
	{
		if (bhfi.dwFileAttributes != FILE_ATTRIBUTE_DIRECTORY)
		{
			ListView_SetItemText(hwndLV, lvItem.iItem, 1, (LPWSTR)(L"Файл"));
		}
		else
			ListView_SetItemText(hwndLV, lvItem.iItem, 1, (LPWSTR)(L"Папка с файлами"));
	}

	// массив атрибутов
	constexpr DWORD attr[] = {
		FILE_ATTRIBUTE_READONLY, FILE_ATTRIBUTE_HIDDEN, FILE_ATTRIBUTE_ARCHIVE,
		FILE_ATTRIBUTE_SYSTEM, FILE_ATTRIBUTE_TEMPORARY, FILE_ATTRIBUTE_COMPRESSED, FILE_ATTRIBUTE_ENCRYPTED
	};
	// массив идентификаторов флажков для атрибутов
	constexpr DWORD ids[] = {
		IDC_ATTRIBUTE_READONLY, IDC_ATTRIBUTE_HIDDEN, IDC_ATTRIBUTE_ARCHIVE,
		IDC_ATTRIBUTE_SYSTEM, IDC_ATTRIBUTE_TEMPORARY, IDC_ATTRIBUTE_COMPRESSED, IDC_ATTRIBUTE_ENCRYPTED
	};

	// раставим флажки соответственно с установленными атрибутами файла/каталога

	for (int i = 0; i < _countof(attr); ++i)
	{
		UINT uCheck = (bhfi.dwFileAttributes & attr[i]) ? BST_CHECKED : BST_UNCHECKED;
		CheckDlgButton(hwnd, ids[i], uCheck);
	} 

	// закрываем дескриптор файла */
	CloseHandle(hFile);
	return TRUE;
}

BOOL __stdcall CalculateSize(LPCTSTR lpszFileName, const LPWIN32_FILE_ATTRIBUTE_DATA lpFileAttributeData, LPVOID lpvParam)
{
	if (lpFileAttributeData->dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
	{
		// продолжим поиск внутри каталога
		return FileSearch(TEXT("*"), lpszFileName, CalculateSize, lpvParam);
	} // if
	return TRUE; // возвращаем TRUE, чтобы продолжить поиск
}



BOOL FileSearch(LPCTSTR lpszFileName,LPCTSTR path, LPSEARCHFUNC lpSearchFunc, LPVOID lpvParam)
{
	WIN32_FIND_DATA ffd;
	LARGE_INTEGER filesize;
	LARGE_INTEGER size;
	TCHAR szDir[MAX_PATH];
	size_t length_of_arg;
	HANDLE hFind = INVALID_HANDLE_VALUE;
	StringCchCopy(szDir, MAX_PATH, path);
	StringCchCat(szDir, MAX_PATH, TEXT("\\*"));
	hFind = FindFirstFile(szDir, &ffd);

	if (INVALID_HANDLE_VALUE == hFind)
	{
			//error
	}
	do
	{
		if (!(ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
		{
			filesize.LowPart = ffd.nFileSizeLow;
			filesize.HighPart = ffd.nFileSizeHigh;
			((ULARGE_INTEGER *)lpvParam)->QuadPart += filesize.QuadPart;
		}
	} while (FindNextFile(hFind, &ffd) != 0);

	FindClose(hFind);
	return TRUE;
} 

BOOL GetFileTimeFormat(const LPFILETIME lpFileTime, LPTSTR lpszFileTime, DWORD cchFileTime)
{
	SYSTEMTIME st;

	// преобразуем дату и время из FILETIME в SYSTEMTIME
	BOOL bRet = FileTimeToSystemTime(lpFileTime, &st);

	// приведем дату и время к текущему часовому поясу
	if (FALSE != bRet)
		bRet = SystemTimeToTzSpecificLocalTime(NULL, &st, &st);

	if (FALSE != bRet)
	{
		// скопируем дату в результирующую строку
		GetDateFormat(LOCALE_USER_DEFAULT, DATE_LONGDATE, &st, NULL, lpszFileTime, cchFileTime);

		// добавим время в результирующую строку

		StringCchCat(lpszFileTime, cchFileTime, TEXT(", "));
		DWORD len = _tcslen(lpszFileTime);

		if (len < cchFileTime)
			GetTimeFormat(LOCALE_USER_DEFAULT, TIME_FORCE24HOURFORMAT, &st, NULL, lpszFileTime + len, cchFileTime - len);
	} // if

	return bRet;
} // GetFileTimeFormat

void PrintFileSize(LPTSTR lpszBuffer, DWORD cch, LARGE_INTEGER size)
{
	if (size.QuadPart >= 0x40000000ULL)
	{
		StringCchPrintf(lpszBuffer, cch, TEXT("%.1f ГБ"), (size.QuadPart / (float)0x40000000ULL));
	} // if
	else if (size.QuadPart >= 0x100000ULL)
	{
		StringCchPrintf(lpszBuffer, cch, TEXT("%.1f МБ"), (size.QuadPart / (float)0x100000ULL));
	} // if
	else if (size.QuadPart >= 0x0400ULL)
	{
		StringCchPrintf(lpszBuffer, cch, TEXT("%.1f КБ"), (size.QuadPart / (float)0x0400ULL));
	} // if
	else
	{
		StringCchPrintf(lpszBuffer, cch, TEXT("%u байт"), size.LowPart);
	} // else

	size_t len = _tcslen(lpszBuffer);

	if (len < cch)
	{
		StringCchPrintf((lpszBuffer + len), (cch - len), TEXT(" (%llu байт)"), size.QuadPart);
	} // if
} // StringCchPrintFileSize

void PrintDirectSize(LPTSTR lpszBuffer, DWORD cch, ULARGE_INTEGER size)
{
	if (size.QuadPart >= 0x40000000ULL)
	{
		StringCchPrintf(lpszBuffer, cch, TEXT("%.1f ГБ"), (size.QuadPart / (float)0x40000000ULL));
	} // if
	else if (size.QuadPart >= 0x100000ULL)
	{
		StringCchPrintf(lpszBuffer, cch, TEXT("%.1f МБ"), (size.QuadPart / (float)0x100000ULL));
	} // if
	else if (size.QuadPart >= 0x0400ULL)
	{
		StringCchPrintf(lpszBuffer, cch, TEXT("%.1f КБ"), (size.QuadPart / (float)0x0400ULL));
	} // if
	else
	{
		StringCchPrintf(lpszBuffer, cch, TEXT("%u байт"), size.LowPart);
	} // else

	size_t len = _tcslen(lpszBuffer);

	if (len < cch)
	{
		StringCchPrintf((lpszBuffer + len), (cch - len), TEXT(" (%llu байт)"), size.QuadPart);
	} // if
} // StringCchPrintFileSize