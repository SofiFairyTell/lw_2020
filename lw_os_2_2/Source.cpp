//#include<Windows.h>
//#include <windowsx.h>
//#include <CommCtrl.h>
//#include <tchar.h>
//
//#include "resource.h"
//
//
//LRESULT CALLBACK MainWindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
//int WINAPI _tWinMain(HINSTANCE hInstance,
//	HINSTANCE hPrevInstance,
//	LPSTR     lpszCmdLine,
//	int       nCmdShow)
//{
//	HWND hWnd = NULL;
//	HWND hSaveDlg = NULL; //save as
//	HWND hRlsDlg = NULL; //replace
//	HACCEL hAccel = NULL;
//	MSG  msg;
//	BOOL bRet;
//
//	TCHAR szBuffer[100] = TEXT("");
//
//	WNDCLASSEX wcex = { sizeof(WNDCLASSEX) }; // структура для регистрации класса окна
//	// Регистрируем класс окна для главного	окна
//
//	wcex.style = CS_HREDRAW | CS_VREDRAW; //для перерисовки окна при изменении ширины/высоты. Через сообщение WM_Paint
//	wcex.lpfnWndProc = MainWindowProc; //  указатель на процедуру окна вызываемую функцией CallWindowProc.
//	wcex.hInstance = hInstance; //дескриптор содержащий оконную процедуру класса
//	wcex.hIcon = LoadIcon(NULL, IDI_APPLICATION);
//	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
//	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
//	wcex.lpszMenuName = MAKEINTRESOURCE(IDR_MENU1);//имя в ресурсах 
//	wcex.lpszClassName = TEXT("MainWindowClass"); // имя класса 
//	//aWndClass = RegisterClass(&wcex);
//	//if (aWndClass == 0) return FALSE;
//	if (0 == RegisterClassEx(&wcex))
//		return -1;
//		// загружаем библиотеку
//		// элементов управления общего пользования
//	LoadLibrary(TEXT("ComCtl32.dll"));
//
//	// загружаем таблицу быстрых клавиш
//	hAccel = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDR_ACCELERATOR1));
//
//	// создаём главное окно на основе нового оконного класса
//	hWnd = CreateWindowEx(
//		0, TEXT("MyWindowClass"), TEXT("SampleWin32"),
//		WS_OVERLAPPEDWINDOW | WS_CAPTION | WS_VISIBLE, CW_USEDEFAULT,
//		0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);
//
//	if (NULL == hWnd) // если не удалось создать окно
//	{
//		return -1; // завершаем работу приложения
//	} // if
//
//// ShowWindow(hWnd, nCmdShow); // отображаем главное окно , можно не использовать если есть WS_VISIBLE
//
//	UpdateWindow(hWnd);// Посылаем в окно сообщение WM_PAINT
//}
//LRESULT CALLBACK MainWindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
//{
//	HDC hdc;             // индекс контекста устройства
//	PAINTSTRUCT ps;      // структура для рисования
//
//	switch (msg)
//	{
//	case WM_PAINT:
//	{
//		hdc = BeginPaint(hwnd, &ps);
//		TextOut(hdc, 10, 20, L"WM_PAINT", 8);
//		EndPaint(hwnd, &ps);
//		return 0;
//	}
//
//	case WM_LBUTTONDOWN:
//	{
//		hdc = GetDC(hwnd);
//		TextOut(hdc, 10, 40, L"WM_LBUTTONDOWN", 14);
//		ReleaseDC(hwnd, hdc);
//		return 0;
//	}
//
//	case WM_DESTROY:
//	{
//		PostQuitMessage(0);
//		return 0;
//	}
//	}
//	return DefWindowProc(hwnd, msg, wParam, lParam);
//}