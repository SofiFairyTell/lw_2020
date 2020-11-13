#include <Windows.h>
#include <WindowsX.h>
#include <tchar.h>
#include <strsafe.h>

#include "resource.h"

#define IDC_EDIT_TEXT        2001
#pragma comment(linker,"\"/manifestdependency:type='win32' \
name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

// оконная процедура главного окна
LRESULT CALLBACK MainWindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
/*Обработчики сообщений WM_CREATE WM_DESTROY WM_SIZE WM_COMMAND */

BOOL OnCreate(HWND hwnd, LPCREATESTRUCT lpCreateStruct);
void OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify);


int WINAPI _tWinMain(HINSTANCE hInstance, HINSTANCE, LPTSTR lpszCmdLine, int nCmdShow)
{
	LoadLibrary(TEXT("ComCtl32.dll"));//для элементов общего пользования								
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

	if (0 == RegisterClassEx(&wcex)) // регистрируем класс
	{
		return -1; // завершаем работу приложения
	}

	// создаем главное окно на основе нового оконного класса
	HWND hWnd = CreateWindowEx(0, TEXT("MainWindowClass"), TEXT("Process"), WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

	if (NULL == hWnd)
	{
		return -1; // завершаем работу приложения
	}

	ShowWindow(hWnd, nCmdShow); // отображаем главное окно

	TCHAR InitFN[MAX_PATH];//имя файла инициализации

	{
		GetModuleFileName(NULL, InitFN, MAX_PATH);

		LPTSTR str = _tcsrchr(InitFN, TEXT('.'));
		if (NULL != str) str[0] = TEXT('\0');

		StringCchCat(InitFN, MAX_PATH, TEXT(".ini"));
	}

	// загружаем параметры приложения из файла инициализации
	LoadProfile(InitFN);

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
