//#include <Windows.h>
//#include <WindowsX.h>
//#include <CommCtrl.h>
//#include <tchar.h>
//#include <strsafe.h>
//#include <process.h>
//
//#define IDC_EDIT_MESSAGES           2001
//#define IDC_EDIT_TEXT               2002
//#define IDC_RBUTTON_WM_SETTEXT      2003
//#define IDC_RBUTTON_WM_COPYDATA     2004
//#define IDC_RBUTTON_CLIPBOARD       2005
//#define IDC_RBUTTON_PIPE            2006
//#define MAX_TEXT                    1024
//
///*Дескрипторы*/
//HWND hwnd = NULL; // дескриптор главного окна
//HKEY hKey = NULL; // дескриптор ключа реестра для запущенных экзм
//HANDLE hStopper = NULL; // событие для уведомления потока ThreadFuncClipboard о завершения приложения
//HANDLE hThreads[2]; // дескрипторы созданных потоков
//
///*Оконные процедуры*/
//LRESULT CALLBACK MainWindowProcess(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
//
//// функция, которая вызывается в цикле обработки сообщений
//// перед тем, как сообщение будет передано в оконную процедуру
//BOOL PreTranslateMessage(LPMSG lpMsg);
///*Обработчики WM_CREATE, WM_DESTROY, WM_SIZE*/
//
//BOOL OnCreate(HWND hwnd, LPCREATESTRUCT lpCreateStruct);
//void OnDestroy(HWND hwnd);
//void OnSize(HWND hwnd, UINT state, int cx, int cy);
//
//
///*---------Работа с реестром---------------*/
//BOOL RegisterApplication();// регистрация каждого нового экземпляра приложения
//void UregisterApplication();// удаление информации о каждом созданном экземпляре
///*---------------------------------------*/
//
///*-----Отправка оконных сообщений-------*/
//void SendText(LPCTSTR lpText, DWORD cchText, BOOL fCopyData);// отправка с помощью оконных сообщений
//void OnSetText(HWND hwnd, LPCTSTR lpszText);// обработчик события WM_SETTEXT
//
//BOOL OnCopyData(HWND hwnd, HWND hwndFrom, PCOPYDATASTRUCT pcds);// обработчик события WM_COPYDATA
//
//
//
//int WINAPI _tWinMain(HINSTANCE hInstance, HINSTANCE, LPTSTR CmdLine, int CmdShow)
//{
//
//	/*Регистрация оконного класса и обработка ошибки*/
//	WNDCLASSEX wcex = { sizeof(WNDCLASSEX) };
//
//	wcex.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
//	wcex.lpfnWndProc = MainWindowProcess; // оконная процедура
//	wcex.hInstance = hInstance;
//	wcex.hIcon = LoadIcon(NULL, IDI_APPLICATION);
//	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
//	wcex.hbrBackground = (HBRUSH)(COLOR_BTNFACE + 1);
//	wcex.lpszClassName = TEXT("MainWindowProcess"); // имя класса
//	wcex.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
//
//	if (0 == RegisterClassEx(&wcex))
//	{
//		return -1;
//	}
//	/*---------------------------------------------*/
//	LoadLibrary(TEXT("ComCtl32.dll"));//
//
//	/*Создание главного файла и обработка ошибки */
//	hwnd = CreateWindowEx(0, TEXT("MainWindowProcess"), TEXT("Chat"),
//		WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, 0, 900, 500, NULL, NULL, hInstance, NULL);
//
//	if (hwnd == NULL)
//	{
//		return -1;
//	}
//	/*--------------------------------------------------*/
//	/*Регистрация в системном реестре и обработка ошибки*/
//	BOOL RetRes = RegisterApplication();// регистрируем приложение в системном реестре
//	if (RetRes == FALSE)
//	{
//		return -1;
//	}
//	/*-----------------------------------------------*/
//
//	ShowWindow(hwnd, CmdShow); // отображаем главное окно
//
//	/*Цикл обработки сообщений*/
//	MSG  msg;
//
//	while ((RetRes = GetMessage(&msg, NULL, 0, 0)) != FALSE)
//	{
//		if (RetRes == -1)
//		{
//			//Error editing
//		}
//		else if (!PreTranslateMessage(&msg))
//		{
//			TranslateMessage(&msg);
//			DispatchMessage(&msg);
//		}
//	}
//
//	UregisterApplication();// удаляем информацию о приложении из системного реестра
//	return (int)msg.wParam;
//}
//
//
///*Процедура главного окна*/
//LRESULT CALLBACK MainWindowProcess(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
//{
//	switch (msg)
//	{
//		HANDLE_MSG(hWnd, WM_CREATE, OnCreate);
//		HANDLE_MSG(hWnd, WM_DESTROY, OnDestroy);
//		HANDLE_MSG(hWnd, WM_SIZE, OnSize);
//		HANDLE_MSG(hWnd, WM_SETTEXT, OnSetText);
//	case WM_COPYDATA:
//		return HANDLE_WM_COPYDATA(hWnd, wParam, lParam, OnCopyData);
//	}
//	return DefWindowProc(hWnd, msg, wParam, lParam);
//}
//
///*Обработчик сообщений*/
//BOOL PreTranslateMessage(LPMSG Msg)
//{
//	if ((WM_KEYDOWN == Msg->message) && (VK_RETURN == Msg->wParam)) // нажата клавиша Enter
//	{
//		HWND hwndCtl = GetDlgItem(hwnd, IDC_EDIT_TEXT);
//
//		if (GetFocus() == hwndCtl) // поле ввода обладает фокусом клавиатуры
//		{
//			/*Чтобы можно было отправить многострочный текст*/
//			/*CTRL+ENTER*/
//			if (GetKeyState(VK_CONTROL) < 0) // нажата клавиша Ctrl
//			{
//				Edit_ReplaceSel(hwndCtl, TEXT("\r\n"));
//			}
//			else
//			{
//				TCHAR szText[MAX_TEXT];
//
//				DWORD cch = Edit_GetText(hwndCtl, szText, _countof(szText));// копируем текст из поля ввода
//
//				if (cch > 0)
//				{
//					// очищаем поле ввода
//					Edit_SetText(hwndCtl, NULL);
//					// отправим текст
//					/*if (IsDlgButtonChecked(hwnd, IDC_RBUTTON_WM_SETTEXT) == BST_CHECKED)
//					{*/
//					StringCchCat(szText, _countof(szText), TEXT("\nWN_SETTEXT\n"));
//
//					SendText(szText, _tcslen(szText), FALSE);
//					//} 
//					//вернуть если надо отправлять через WM_COPYDATA
//				/*else
//						if (IsDlgButtonChecked(hwnd, IDC_RBUTTON_WM_COPYDATA) == BST_CHECKED)
//							{
//								StringCchCat(szText, _countof(szText),TEXT("\nОтправлен через WM_COPYDATA\n"));
//
//								SendText(szText, _tcslen(szText), TRUE);
//							} */
//				}
//			}
//			return TRUE;
//		}
//	}
//	return FALSE;
//}
//
//BOOL OnCreate(HWND hwnd, LPCREATESTRUCT lpCreateStruct)
//{
//	// создаём событие для уведомления потока ThreadFuncClipboard о завершения приложения
//	hStopper = CreateEventEx(NULL, NULL, CREATE_EVENT_MANUAL_RESET, EVENT_ALL_ACCESS);
//
//	// создаём групповую рамку
//	CreateWindowEx(0, TEXT("Button"), TEXT("Межпроцессное взаимодействие"), WS_CHILD | WS_VISIBLE | BS_GROUPBOX,
//		10, 10, 300, 130, hwnd, (HMENU)0, lpCreateStruct->hInstance, NULL);
//
//	// создаём переключатели
//
//	CreateWindowEx(0, TEXT("Button"), TEXT("Оконное сообщение WM_SETTEXT"), WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON,
//		20, 30, 280, 25, hwnd, (HMENU)IDC_RBUTTON_WM_SETTEXT, lpCreateStruct->hInstance, NULL);
//
//	CreateWindowEx(0, TEXT("Button"), TEXT("Оконное сообщение WM_COPYDATA"), WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON,
//		20, 55, 280, 25, hwnd, (HMENU)IDC_RBUTTON_WM_COPYDATA, lpCreateStruct->hInstance, NULL);
//
//	CheckRadioButton(hwnd, IDC_RBUTTON_WM_SETTEXT, IDC_RBUTTON_PIPE, IDC_RBUTTON_WM_SETTEXT);
//
//
//	DWORD dwStyle = WS_CHILD | WS_VISIBLE | WS_VSCROLL | WS_HSCROLL | ES_MULTILINE | ES_AUTOHSCROLL | ES_AUTOVSCROLL;
//
//	// создаём поле вывода сообщений
//	CreateWindowEx(WS_EX_STATICEDGE, TEXT("Edit"), TEXT(""), dwStyle | ES_READONLY,
//		320, 10, 400, 400, hwnd, (HMENU)IDC_EDIT_MESSAGES, lpCreateStruct->hInstance, NULL);
//
//	// создаём поле ввода
//	HWND hwndCtl = CreateWindowEx(WS_EX_CLIENTEDGE, TEXT("Edit"), TEXT(""), dwStyle,
//		320, 420, 400, 100, hwnd, (HMENU)IDC_EDIT_TEXT, lpCreateStruct->hInstance, NULL);
//	// задаём ограничение на ввод текста
//	Edit_LimitText(hwndCtl, MAX_TEXT);
//	// передаём фокус полю ввода
//	SetFocus(hwndCtl);
//
//	return TRUE;
//}
//
//void OnDestroy(HWND hwnd)
//{
//	// уведомляем о завершении приложения,
//	SetEvent(hStopper);// чтобы завершить поток ThreadFuncClipboard
//
//	WaitForMultipleObjects(_countof(hThreads), hThreads, TRUE, INFINITE);// ожидаем завершения созданных потоков
//
//	CloseHandle(hStopper);// закрываем дескриптор события для завершении приложения
//
//	PostQuitMessage(0); // отправляем сообщение WM_QUIT
//}
//
//void OnSize(HWND hwnd, UINT state, int cx, int cy)
//{
//	if (state != SIZE_MINIMIZED)
//	{
//		MoveWindow(GetDlgItem(hwnd, IDC_EDIT_MESSAGES), 320, 10, cx - 320, cy - 130, TRUE);// изменяем размеры поля вывода сообщений
//		MoveWindow(GetDlgItem(hwnd, IDC_EDIT_TEXT), 320, cy - 110, cx - 330, 100, TRUE);// изменяем размеры поля ввода
//	}
//}
//
//void OnSetText(HWND hwnd, LPCTSTR lpszText)
//{
//	HWND hwndCtl = GetDlgItem(hwnd, IDC_EDIT_MESSAGES);
//	Edit_SetSel(hwndCtl, Edit_GetTextLength(hwndCtl), -1);// устанавливаем курсор в конец поля вывода
//
//	Edit_ReplaceSel(hwndCtl, lpszText);// вставляем текст в поле вывода
//}
//
//// ----------------------------------------------------------------------------------------------
//BOOL OnCopyData(HWND hwnd, HWND hwndFrom, PCOPYDATASTRUCT pcds)
//{
//	OnSetText(hwnd, (LPCTSTR)pcds->lpData);
//	return TRUE;
//} // OnCopyData
//
//BOOL RegisterApplication()
//{
//	// создаём и открываем ключ реестра HKEY_CURRENT_USER\Software\\IT-311
//
//	DWORD dwDisposition;
//
//	LONG lStatus = RegCreateKeyEx(HKEY_CURRENT_USER, TEXT("Software\\IT-311"),
//		0, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hKey, &dwDisposition);
//
//	if (ERROR_SUCCESS == lStatus)
//	{
//		DWORD dwProcessId = GetCurrentProcessId();// получим идентификатор текущего процесса
//
//		HKEY  hSubKey = NULL; // дескриптор вложенного ключа реестра, который будет содержать данные текущего приложения
//
//		TCHAR szSubKey[20]; // имя вложенного ключа реестра
//
//		StringCchPrintf(szSubKey, _countof(szSubKey), TEXT("%d"), dwProcessId);// формируем имя вложенного ключа реестра
//
//		lStatus = RegCreateKey(hKey, szSubKey, &hSubKey);// создаём вложенный ключ реестра
//
//		if (ERROR_SUCCESS == lStatus)
//		{
//			// сохраняем идентификатор текущего процесса
//			RegSetValueEx(hSubKey, NULL, 0, REG_DWORD, (LPBYTE)&dwProcessId, sizeof(DWORD));
//			// сохраняем значение дескриптор главного окна
//			RegSetValueEx(hSubKey, TEXT("hwnd"), 0, REG_BINARY, (LPBYTE)&hwnd, sizeof(HWND));
//			// закрываем вложенный ключ реестра
//			RegCloseKey(hSubKey), hSubKey = NULL;
//			return TRUE;
//		}
//		// закрываем ключ реестра
//		RegCloseKey(hKey), hKey = NULL;
//	}
//	return FALSE;
//}
//
//void UregisterApplication()
//{
//	if (NULL != hKey)
//	{
//		DWORD cSubKeys = 0; // количество вложенных ключей	
//		LSTATUS lStatus = RegQueryInfoKey(hKey, NULL, NULL, NULL, &cSubKeys, NULL, NULL, NULL, NULL, NULL, NULL, NULL);// определяем количество вложенных ключей
//
//		if ((ERROR_SUCCESS == lStatus) && (cSubKeys < 2))
//		{
//			RegDeleteTree(hKey, NULL);// удаляем всю ветку		
//			RegDeleteKey(HKEY_CURRENT_USER, TEXT("Software\\IT-311"));// удаляем ключ
//		}
//		else
//		{
//			TCHAR szSubKey[20]; // имя вложенного ключа реестра, который содержит данные текущего приложения	
//
//			// формируем имя вложенного ключа реестра
//			StringCchPrintf(szSubKey, _countof(szSubKey), TEXT("%d"), GetCurrentProcessId());
//			RegDeleteKey(hKey, szSubKey);// удаляем вложенный ключ
//		}
//		RegCloseKey(hKey), hKey = NULL;// закрываем ключ реестра
//	}
//}
//
//// ----------------------------------------------------------------------------------------------
//void SendText(LPCTSTR lpData, DWORD cbData, BOOL DataCopy)
//{
//	/*Переменнные*/
//	HKEY  hSubKey = NULL; // дескриптор вложенного ключа реестра
//	TCHAR szSubKey[20]; // имя вложенного ключа реестра
//	DWORD cSubKeys = 0; // количество вложенных ключей
//	LSTATUS lStatus; //для результатов с вложенными ключами
//
//	if ((lpData != NULL) && (cbData > 0))
//	{
//		lStatus = RegQueryInfoKey(hKey, NULL, NULL, NULL, &cSubKeys, NULL, NULL, NULL, NULL, NULL, NULL, NULL);// определяем количество вложенных ключей
//
//		if ((ERROR_SUCCESS == lStatus) && (cSubKeys > 0))
//		{
//			for (DWORD dwIndex = 0; dwIndex < cSubKeys; ++dwIndex)// перечисляем вложенные ключи ...
//			{
//				// получим имя вложенного ключа с индексом dwIndex
//				DWORD cchSubKey = _countof(szSubKey);
//				lStatus = RegEnumKeyEx(hKey, dwIndex, szSubKey, &cchSubKey, NULL, NULL, NULL, NULL);
//
//				if (lStatus == ERROR_SUCCESS)
//				{
//					lStatus = RegOpenKeyEx(hKey, szSubKey, 0, KEY_QUERY_VALUE, &hSubKey);// открываем вложенный ключ
//				}
//
//				if (ERROR_SUCCESS == lStatus)
//				{
//					HWND hRecvWnd; // дескриптор окна получателя
//					DWORD cb = sizeof(HWND);
//
//					lStatus = RegQueryValueEx(hSubKey, TEXT("hwnd"), NULL, NULL, (LPBYTE)&hRecvWnd, &cb);// получаем дескриптор окна получателя
//
//					if (lStatus == ERROR_SUCCESS)
//					{
//						if (DataCopy != FALSE)
//						{
//							// инициализируем структуру
//							COPYDATASTRUCT cds;
//							cds.lpData = (PVOID)lpData;
//							cds.cbData = (cbData + 1) * sizeof(TCHAR);
//							cds.dwData = 0;
//							SendMessage(hRecvWnd, WM_COPYDATA, (WPARAM)hwnd, (LPARAM)&cds);// отправляем сообщение WM_COPYDATA
//						}
//						else
//						{
//							SendMessage(hRecvWnd, WM_SETTEXT, 0, (LPARAM)lpData);// отправляем сообщение WM_SETTEXT
//						}
//					}
//					RegCloseKey(hSubKey), hSubKey = NULL;// закрываем вложенный ключ реестра
//				}
//			}
//		}
//	}
//}
//
//
