#include <Windows.h>
#include <WindowsX.h>
#include <tchar.h>
#include <CommCtrl.h>
#include <strsafe.h>

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
	/*static HWND name, type,address, size,timecreate,timechange,timeopen;
	static const int width = 30, length = 120, shift = 70;
	name = CreateWindowEx(0, WC_EDIT,TEXT(""), WS_CHILD | WS_VISIBLE | WS_BORDER,	110, shift, length, width - 1, hwnd, (HMENU)IDC_EDIT, lpCRStr->hInstance, NULL);
	type = CreateWindowEx(0, WC_STATIC,	TEXT("Тип файла"),	WS_CHILD | WS_VISIBLE | WS_BORDER,	10, 10, 100, 10, hwnd, (HMENU)IDC_STATIC_TYPE, lpCRStr->hInstance, NULL);*/
	
	//CreateWindowEx(0, TEXT("ListBox"), NULL, WS_CHILD | WS_VISIBLE | WS_BORDER | LBS_WANTKEYBOARDINPUT | LBS_NOTIFY, 10, 10, 200, 410, hwnd, (HMENU)IDC_LIST1, lpCRStr->hInstance, NULL);

	/*
	HWND hlistview = CreateWindow(WC_LISTVIEW, L"",	WS_VISIBLE | WS_BORDER | WS_CHILD | LVS_REPORT | LVS_SINGLESEL,	10, 10, 100, 100,hwnd, (HMENU)IDC_LIST1, NULL, 0);
	ListView_SetExtendedListViewStyle(hlistview, LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_HEADERDRAGDROP | LVS_EX_DOUBLEBUFFER | LVS_EX_FLATSB | LVS_EX_INFOTIP);

	LVCOLUMN lvc;

	lvc.mask = LVCF_TEXT | LVCF_SUBITEM | LVCF_WIDTH | LVCF_FMT;    //Стиль таблицы
	lvc.fmt = LVCFMT_LEFT;                                          //выравнивание по левому краю

	lvc.iSubItem = 0;                                               //Индекс столбца
	lvc.pszText = (LPWSTR)(" ");                                   //Название столбца
	lvc.cx = 30;                                                    //Длина столбца относительно левого края
	ListView_InsertColumn(hlistview, 0, &lvc);                      //Функция вставки столбцов

	lvc.iSubItem = 1;
	lvc.pszText = (LPWSTR)(" ");
	lvc.cx = 75;
	ListView_InsertColumn(hlistview, 1, &lvc);
	*/
	HWND hwndLV = CreateWindowEx(0, TEXT("SysListView32"), NULL,
		WS_CHILD | WS_VISIBLE | WS_BORDER | LVS_REPORT | LVS_SHOWSELALWAYS | LVS_SORTASCENDING, 10, 10, 400, 250, hwnd, (HMENU)IDC_LIST1, lpCRStr->hInstance, NULL);

	// задаёс расширенный 
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
	} // for
	

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











			/*Работает, не трогать*/
			if (GetOpenFileName(&openfile) != FALSE)
				{						
					HWND hwndLV = GetDlgItem(hwnd, IDC_LIST1);
					// добавляем новый элемент в список просмотра
					ListView_DeleteAllItems(hwndLV);						
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
			}
	break;

	case ID_EXIT:
		SendMessage(hwnd, WM_CLOSE, 0, 0);
		break;

	}
} 