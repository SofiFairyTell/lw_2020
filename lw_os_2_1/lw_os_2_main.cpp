//#include "lw_os_2_1.h"
//
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
//	Register(hInstance);
//	/* (!Register(hInstance))
//		return -1;// не удалось зарегистрировать новый оконный класс завершаем работу */
//
//	// загружаем библиотеку
//	// элементов управления общего пользования
//	LoadLibrary(TEXT("ComCtl32.dll"));
//
//	// загружаем таблицу быстрых клавиш
//	hAccel = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDR_ACCELERATOR1));
//
//	// создаём главное окно на основе нового оконного класса
//	hWnd = CreateWindowEx(
//		0, TEXT("MyWindowClass"), TEXT("SampleWin32"), 
//		WS_OVERLAPPEDWINDOW| WS_CAPTION| WS_VISIBLE, CW_USEDEFAULT,
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