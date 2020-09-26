#include <Windows.h>
#include <WindowsX.h>
#include <CommCtrl.h>
#include <tchar.h>
#include "resource.h"
//#include "afxres.h"

#define WM_ADDITEM WM_USER+1

HWND hWnd = NULL; //дескриптор окна
HACCEL hAccel = NULL; //дескриптор акселератора 
HWND hSaveDlg = NULL; //save as
HWND hRlsDlg = NULL; //replace
	TCHAR szBuffer[100] = TEXT("");
BOOL OnCreate(HWND hwnd, LPCREATESTRUCT lpCRStr);

LRESULT CALLBACK MainWindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,	LPSTR lpszCmdLine,int  nCmdShow)
{
	MSG  msg;
	BOOL bRet;
	
	/*WNDCLASSEX wcex = { };
	// Регистрируем класс окна для главного	окна
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.style = CS_HREDRAW | CS_VREDRAW; //для перерисовки окна при изменении ширины/высоты. Через сообщение WM_Paint
	wcex.lpfnWndProc = MainWindowProc; //  указатель на процедуру окна вызываемую функцией CallWindowProc.
	wcex.hInstance = hInstance; //дескриптор содержащий оконную процедуру класса
	wcex.hIcon = LoadIcon(NULL, IDI_WINLOGO);
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = TEXT("MainMenu");
		//MAKEINTRESOURCE(IDR_MENU1);//имя в ресурсах 
	wcex.lpszClassName = TEXT("MainWindowClass"); // имя класса 
	//aWndClass = RegisterClass(&wcex);
	//if (aWndClass == 0) return FALSE;
	if (0 == RegisterClassEx(&wcex))
		return -1;
	// загружаем библиотеку
	// элементов управления общего пользования
	LoadLibrary(TEXT("ComCtl32.dll"));

	// загружаем таблицу быстрых клавиш
	hAccel = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDR_ACCELERATOR1));

	// создаём главное окно на основе нового оконного класса
	hWnd = CreateWindowEx(0,L"MyWindowClass",L"Win32",
		WS_OVERLAPPEDWINDOW, CW_USEDEFAULT,
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, NULL, NULL, hInstance, NULL);
	
	if (NULL == hWnd) // если не удалось создать окно
	{
		system("PAUSE");
		return -1; // завершаем работу приложения
	} // if

	ShowWindow(hWnd, nCmdShow); // отображаем главное окно , можно не использовать если есть WS_VISIBLE

	//UpdateWindow(hWnd);// Посылаем в окно сообщение WM_PAINT

	while (GetMessage(&msg, NULL, 0, 0)) 
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}*/
	// регистрируем оконный класс главного окна...

	WNDCLASSEX wcex = { sizeof(WNDCLASSEX) };
	wcex.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
	wcex.lpfnWndProc = MainWindowProc; // оконная процедура
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_BTNFACE + 1);
	wcex.lpszMenuName = MAKEINTRESOURCE(IDR_MENU1);
	wcex.lpszClassName = TEXT("MyWindowClass"); // имя класса
	wcex.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

	if (0 == RegisterClassEx(&wcex)) // регистрируем класс
	{
		// не удалось зарегистрировать новый оконный класс
		return -1; // завершаем работу приложения
	} // if

	// загружаем библиотеку
	// элементов управления общего пользования
	LoadLibrary(TEXT("ComCtl32.dll"));

	// загружаем таблицу быстрых клавиш
	hAccel = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDR_ACCELERATOR1));

	// создаём главное окно на основе нового оконного класса
	hWnd = CreateWindowEx(0, TEXT("MyWindowClass"), TEXT("SampleWin32"), WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

	if (NULL == hWnd) // если не удалось создать окно
	{
		return -1; // завершаем работу приложения
	} // if

	ShowWindow(hWnd, nCmdShow); // отображаем главное окно
	system("pause");
}

LRESULT CALLBACK MainWindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	HDC hdc;             // индекс контекста устройства
	PAINTSTRUCT ps;      // структура для рисования

	/*switch (msg)
	{
		case WM_PAINT:
		{
			hdc = BeginPaint(hwnd, &ps);
			TextOut(hdc, 10, 20, L"WM_PAINT", 8);
			EndPaint(hwnd, &ps);
			return 0;
		}

		case WM_LBUTTONDOWN:
		{
			hdc = GetDC(hwnd);
			TextOut(hdc, 10, 40, L"WM_LBUTTONDOWN", 14);
			ReleaseDC(hwnd, hdc);
			return 0;
		}

		case WM_DESTROY:
		{
			PostQuitMessage(0);
			return 0;
		}
		}
	return (DefWindowProc(hwnd, msg, wParam, lParam));*/
	switch (msg)
	{
		HANDLE_MSG(hwnd, WM_CREATE, OnCreate);
	case WM_ADDITEM:
		return 0;
	//case WM_CREATE:
	//	// Initialize the window. 
	//	return 0;

	//case WM_PAINT:
	//	hdc = BeginPaint(hwnd, &ps);
	//	TextOut(hdc, 10, 20, L"WM_PAINT", 8);
	//	EndPaint(hwnd, &ps);
	//	return 0;

	//case WM_SIZE:
	//	// Set the size and position of the window. 
	//	return 0;

	//case WM_DESTROY:
	//	PostQuitMessage(0);
	//	return 0;

	//	// 
	//	// Process other messages. 
	//	// 

	/*default:
		return DefWindowProc(hwnd, msg, wParam, lParam);*/
	}
	return DefWindowProc(hwnd, msg, wParam, lParam);
}

BOOL OnCreate(HWND hwnd, LPCREATESTRUCT lpCRStr)
{
	CreateWindowEx(0, TEXT("ListBox"), NULL, WS_CHILD | WS_VISIBLE, 10, 10, 250, 410, hwnd, (HMENU)IDC_LIST1, lpCRStr->hInstance, NULL);
	return TRUE;
}