#include <Windows.h>
#include <WindowsX.h>
#include <tchar.h>
#include <CommCtrl.h>
#include <strsafe.h>
#include <time.h>//для ctime
#include <fileapi.h>
#include "resource.h"

#define IDC_EDIT_TEXT        2001

#pragma warning(disable : 4996) //отключает Ошибку deprecate. Возникает, когда используется устаревшая функци
#pragma comment(linker,"\"/manifestdependency:type='win32' \
name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

// оконная процедура главного окна
LRESULT CALLBACK MainWindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
/*Обработчики сообщений WM_CREATE WM_DESTROY WM_SIZE WM_COMMAND */

BOOL OnCreate(HWND hwnd, LPCREATESTRUCT lpCreateStruct);
void OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify);

/*Перевод времени*/
BOOL GetFileTimeFormat(const LPFILETIME lpFileTime, LPTSTR lpszFileTime, DWORD cchFileTime);

/*Перевод чисел*/
void StringCchPrintFileSize(LPTSTR lpszBuffer, DWORD cch, LARGE_INTEGER size);


TCHAR FileName[MAX_PATH] = TEXT(""); // имя редактируемого текстового файла
HANDLE hFile = INVALID_HANDLE_VALUE; // дескриптор редактируемого текстового файла


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
	HWND hwndLV = CreateWindowEx(0, TEXT("SysListView32"), NULL,
		WS_CHILD | WS_VISIBLE | WS_BORDER | LVS_REPORT | LVS_SHOWSELALWAYS , 30, 40, 400, 150, hwnd, (HMENU)IDC_LIST1, lpCRStr->hInstance, NULL);

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
			HANDLE  hFile;
			BY_HANDLE_FILE_INFORMATION  bhfi;  // информация о файле

			TCHAR TimeBuffer[100], Buffer[100];


			// открываем файл для чтения
			hFile = CreateFile(
				FileName,   // имя файла
				0,                     // получение информации о файле
				0,                     // монопольный доступ к файлу
				NULL,                  // защиты нет 
				OPEN_EXISTING,         // открываем существующий файл
				FILE_ATTRIBUTE_NORMAL, // обычный файл
				NULL                   // шаблона нет
			);
			// проверяем на успешное открытие
			if (hFile == INVALID_HANDLE_VALUE)
			{
				GetLastError();
				break;
			}
			// получаем информацию о файле
			//get info about file
			if (!GetFileInformationByHandle(hFile, &bhfi))
			{
				GetLastError();
				break;
			}
			//получение информации о размере файла
			//get info about size of file
			LARGE_INTEGER LI_Size;
			if (!GetFileSizeEx(hFile, &LI_Size))
			{
				//обработка ошибки
			}

			StringCchPrintFileSize(Buffer, _countof(Buffer), LI_Size);
					   		

			if (bhfi.dwFileAttributes == FILE_ATTRIBUTE_DIRECTORY)
			{
				
			}







			/*Работает, не трогать*/

			//Добавление найденных атрибутов в список просмотра

			HWND hwndLV = GetDlgItem(hwnd, IDC_LIST1);
			// добавляем новый элемент в список просмотра
			ListView_DeleteAllItems(hwndLV);
			LVITEM lvItem = {LVIF_TEXT | LVIF_PARAM};
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
				ListView_SetItemText(hwndLV, lvItem.iItem, 1, FileName);
			}

			//Тип

			lvItem.pszText = (LPWSTR)(L"Тип:");
			lvItem.iItem = ListView_InsertItem(hwndLV, &lvItem);
			if ((lvItem.iItem != -1))
			{
				ListView_SetItemText(hwndLV, lvItem.iItem, 1, FileName);
			}

			}


			// закрываем дескриптор файла */
			CloseHandle(hFile);






/*


			/*Работает, не трогать
			if (GetOpenFileName(&openfile) != FALSE)
				{						
					HWND hwndLV = GetDlgItem(hwnd, IDC_LIST1);
					// добавляем новый элемент в список просмотра
					ListView_DeleteAllItems(hwndLV);	
					
					//
					LVITEM lvItem = { LVIF_TEXT | LVIF_PARAM };
					lvItem.iItem = ListView_GetItemCount(hwndLV);
					lvItem.pszText = (LPTSTR)"Атрибут 1";
					// добавляем новый элемент в список просмотра
					lvItem.iItem = ListView_InsertItem(hwndLV, &lvItem);
						
					if ((lvItem.iItem != -1))
					{
						ListView_SetItemText(hwndLV, lvItem.iItem, 1, FileName);
					} 
			}
			else
				{
					MessageBox(NULL, TEXT("Не удалось открыть текстовый файл."), NULL, MB_OK | MB_ICONERROR);
					FileName[0] = _T('\0');
				}
*/			
}
	break;

	case ID_EXIT:
		SendMessage(hwnd, WM_CLOSE, 0, 0);
		break;

	}
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

void StringCchPrintFileSize(LPTSTR lpszBuffer, DWORD cch, LARGE_INTEGER size)
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