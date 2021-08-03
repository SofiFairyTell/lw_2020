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
///*�����������*/
//HWND hwnd = NULL; // ���������� �������� ����
//HKEY hKey = NULL; // ���������� ����� ������� ��� ���������� ����
//HANDLE hStopper = NULL; // ������� ��� ����������� ������ ThreadFuncClipboard � ���������� ����������
//HANDLE hThreads[2]; // ����������� ��������� �������
//
///*������� ���������*/
//LRESULT CALLBACK MainWindowProcess(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
//
//// �������, ������� ���������� � ����� ��������� ���������
//// ����� ���, ��� ��������� ����� �������� � ������� ���������
//BOOL PreTranslateMessage(LPMSG lpMsg);
///*����������� WM_CREATE, WM_DESTROY, WM_SIZE*/
//
//BOOL OnCreate(HWND hwnd, LPCREATESTRUCT lpCreateStruct);
//void OnDestroy(HWND hwnd);
//void OnSize(HWND hwnd, UINT state, int cx, int cy);
//
//
///*---------������ � ��������---------------*/
//BOOL RegisterApplication();// ����������� ������� ������ ���������� ����������
//void UregisterApplication();// �������� ���������� � ������ ��������� ����������
///*---------------------------------------*/
//
///*-----�������� ������� ���������-------*/
//void SendText(LPCTSTR lpText, DWORD cchText, BOOL fCopyData);// �������� � ������� ������� ���������
//void OnSetText(HWND hwnd, LPCTSTR lpszText);// ���������� ������� WM_SETTEXT
//
//BOOL OnCopyData(HWND hwnd, HWND hwndFrom, PCOPYDATASTRUCT pcds);// ���������� ������� WM_COPYDATA
//
//
//
//int WINAPI _tWinMain(HINSTANCE hInstance, HINSTANCE, LPTSTR CmdLine, int CmdShow)
//{
//
//	/*����������� �������� ������ � ��������� ������*/
//	WNDCLASSEX wcex = { sizeof(WNDCLASSEX) };
//
//	wcex.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
//	wcex.lpfnWndProc = MainWindowProcess; // ������� ���������
//	wcex.hInstance = hInstance;
//	wcex.hIcon = LoadIcon(NULL, IDI_APPLICATION);
//	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
//	wcex.hbrBackground = (HBRUSH)(COLOR_BTNFACE + 1);
//	wcex.lpszClassName = TEXT("MainWindowProcess"); // ��� ������
//	wcex.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
//
//	if (0 == RegisterClassEx(&wcex))
//	{
//		return -1;
//	}
//	/*---------------------------------------------*/
//	LoadLibrary(TEXT("ComCtl32.dll"));//
//
//	/*�������� �������� ����� � ��������� ������ */
//	hwnd = CreateWindowEx(0, TEXT("MainWindowProcess"), TEXT("Chat"),
//		WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, 0, 900, 500, NULL, NULL, hInstance, NULL);
//
//	if (hwnd == NULL)
//	{
//		return -1;
//	}
//	/*--------------------------------------------------*/
//	/*����������� � ��������� ������� � ��������� ������*/
//	BOOL RetRes = RegisterApplication();// ������������ ���������� � ��������� �������
//	if (RetRes == FALSE)
//	{
//		return -1;
//	}
//	/*-----------------------------------------------*/
//
//	ShowWindow(hwnd, CmdShow); // ���������� ������� ����
//
//	/*���� ��������� ���������*/
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
//	UregisterApplication();// ������� ���������� � ���������� �� ���������� �������
//	return (int)msg.wParam;
//}
//
//
///*��������� �������� ����*/
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
///*���������� ���������*/
//BOOL PreTranslateMessage(LPMSG Msg)
//{
//	if ((WM_KEYDOWN == Msg->message) && (VK_RETURN == Msg->wParam)) // ������ ������� Enter
//	{
//		HWND hwndCtl = GetDlgItem(hwnd, IDC_EDIT_TEXT);
//
//		if (GetFocus() == hwndCtl) // ���� ����� �������� ������� ����������
//		{
//			/*����� ����� ���� ��������� ������������� �����*/
//			/*CTRL+ENTER*/
//			if (GetKeyState(VK_CONTROL) < 0) // ������ ������� Ctrl
//			{
//				Edit_ReplaceSel(hwndCtl, TEXT("\r\n"));
//			}
//			else
//			{
//				TCHAR szText[MAX_TEXT];
//
//				DWORD cch = Edit_GetText(hwndCtl, szText, _countof(szText));// �������� ����� �� ���� �����
//
//				if (cch > 0)
//				{
//					// ������� ���� �����
//					Edit_SetText(hwndCtl, NULL);
//					// �������� �����
//					/*if (IsDlgButtonChecked(hwnd, IDC_RBUTTON_WM_SETTEXT) == BST_CHECKED)
//					{*/
//					StringCchCat(szText, _countof(szText), TEXT("\nWN_SETTEXT\n"));
//
//					SendText(szText, _tcslen(szText), FALSE);
//					//} 
//					//������� ���� ���� ���������� ����� WM_COPYDATA
//				/*else
//						if (IsDlgButtonChecked(hwnd, IDC_RBUTTON_WM_COPYDATA) == BST_CHECKED)
//							{
//								StringCchCat(szText, _countof(szText),TEXT("\n��������� ����� WM_COPYDATA\n"));
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
//	// ������ ������� ��� ����������� ������ ThreadFuncClipboard � ���������� ����������
//	hStopper = CreateEventEx(NULL, NULL, CREATE_EVENT_MANUAL_RESET, EVENT_ALL_ACCESS);
//
//	// ������ ��������� �����
//	CreateWindowEx(0, TEXT("Button"), TEXT("������������� ��������������"), WS_CHILD | WS_VISIBLE | BS_GROUPBOX,
//		10, 10, 300, 130, hwnd, (HMENU)0, lpCreateStruct->hInstance, NULL);
//
//	// ������ �������������
//
//	CreateWindowEx(0, TEXT("Button"), TEXT("������� ��������� WM_SETTEXT"), WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON,
//		20, 30, 280, 25, hwnd, (HMENU)IDC_RBUTTON_WM_SETTEXT, lpCreateStruct->hInstance, NULL);
//
//	CreateWindowEx(0, TEXT("Button"), TEXT("������� ��������� WM_COPYDATA"), WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON,
//		20, 55, 280, 25, hwnd, (HMENU)IDC_RBUTTON_WM_COPYDATA, lpCreateStruct->hInstance, NULL);
//
//	CheckRadioButton(hwnd, IDC_RBUTTON_WM_SETTEXT, IDC_RBUTTON_PIPE, IDC_RBUTTON_WM_SETTEXT);
//
//
//	DWORD dwStyle = WS_CHILD | WS_VISIBLE | WS_VSCROLL | WS_HSCROLL | ES_MULTILINE | ES_AUTOHSCROLL | ES_AUTOVSCROLL;
//
//	// ������ ���� ������ ���������
//	CreateWindowEx(WS_EX_STATICEDGE, TEXT("Edit"), TEXT(""), dwStyle | ES_READONLY,
//		320, 10, 400, 400, hwnd, (HMENU)IDC_EDIT_MESSAGES, lpCreateStruct->hInstance, NULL);
//
//	// ������ ���� �����
//	HWND hwndCtl = CreateWindowEx(WS_EX_CLIENTEDGE, TEXT("Edit"), TEXT(""), dwStyle,
//		320, 420, 400, 100, hwnd, (HMENU)IDC_EDIT_TEXT, lpCreateStruct->hInstance, NULL);
//	// ����� ����������� �� ���� ������
//	Edit_LimitText(hwndCtl, MAX_TEXT);
//	// ������� ����� ���� �����
//	SetFocus(hwndCtl);
//
//	return TRUE;
//}
//
//void OnDestroy(HWND hwnd)
//{
//	// ���������� � ���������� ����������,
//	SetEvent(hStopper);// ����� ��������� ����� ThreadFuncClipboard
//
//	WaitForMultipleObjects(_countof(hThreads), hThreads, TRUE, INFINITE);// ������� ���������� ��������� �������
//
//	CloseHandle(hStopper);// ��������� ���������� ������� ��� ���������� ����������
//
//	PostQuitMessage(0); // ���������� ��������� WM_QUIT
//}
//
//void OnSize(HWND hwnd, UINT state, int cx, int cy)
//{
//	if (state != SIZE_MINIMIZED)
//	{
//		MoveWindow(GetDlgItem(hwnd, IDC_EDIT_MESSAGES), 320, 10, cx - 320, cy - 130, TRUE);// �������� ������� ���� ������ ���������
//		MoveWindow(GetDlgItem(hwnd, IDC_EDIT_TEXT), 320, cy - 110, cx - 330, 100, TRUE);// �������� ������� ���� �����
//	}
//}
//
//void OnSetText(HWND hwnd, LPCTSTR lpszText)
//{
//	HWND hwndCtl = GetDlgItem(hwnd, IDC_EDIT_MESSAGES);
//	Edit_SetSel(hwndCtl, Edit_GetTextLength(hwndCtl), -1);// ������������� ������ � ����� ���� ������
//
//	Edit_ReplaceSel(hwndCtl, lpszText);// ��������� ����� � ���� ������
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
//	// ������ � ��������� ���� ������� HKEY_CURRENT_USER\Software\\IT-311
//
//	DWORD dwDisposition;
//
//	LONG lStatus = RegCreateKeyEx(HKEY_CURRENT_USER, TEXT("Software\\IT-311"),
//		0, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hKey, &dwDisposition);
//
//	if (ERROR_SUCCESS == lStatus)
//	{
//		DWORD dwProcessId = GetCurrentProcessId();// ������� ������������� �������� ��������
//
//		HKEY  hSubKey = NULL; // ���������� ���������� ����� �������, ������� ����� ��������� ������ �������� ����������
//
//		TCHAR szSubKey[20]; // ��� ���������� ����� �������
//
//		StringCchPrintf(szSubKey, _countof(szSubKey), TEXT("%d"), dwProcessId);// ��������� ��� ���������� ����� �������
//
//		lStatus = RegCreateKey(hKey, szSubKey, &hSubKey);// ������ ��������� ���� �������
//
//		if (ERROR_SUCCESS == lStatus)
//		{
//			// ��������� ������������� �������� ��������
//			RegSetValueEx(hSubKey, NULL, 0, REG_DWORD, (LPBYTE)&dwProcessId, sizeof(DWORD));
//			// ��������� �������� ���������� �������� ����
//			RegSetValueEx(hSubKey, TEXT("hwnd"), 0, REG_BINARY, (LPBYTE)&hwnd, sizeof(HWND));
//			// ��������� ��������� ���� �������
//			RegCloseKey(hSubKey), hSubKey = NULL;
//			return TRUE;
//		}
//		// ��������� ���� �������
//		RegCloseKey(hKey), hKey = NULL;
//	}
//	return FALSE;
//}
//
//void UregisterApplication()
//{
//	if (NULL != hKey)
//	{
//		DWORD cSubKeys = 0; // ���������� ��������� ������	
//		LSTATUS lStatus = RegQueryInfoKey(hKey, NULL, NULL, NULL, &cSubKeys, NULL, NULL, NULL, NULL, NULL, NULL, NULL);// ���������� ���������� ��������� ������
//
//		if ((ERROR_SUCCESS == lStatus) && (cSubKeys < 2))
//		{
//			RegDeleteTree(hKey, NULL);// ������� ��� �����		
//			RegDeleteKey(HKEY_CURRENT_USER, TEXT("Software\\IT-311"));// ������� ����
//		}
//		else
//		{
//			TCHAR szSubKey[20]; // ��� ���������� ����� �������, ������� �������� ������ �������� ����������	
//
//			// ��������� ��� ���������� ����� �������
//			StringCchPrintf(szSubKey, _countof(szSubKey), TEXT("%d"), GetCurrentProcessId());
//			RegDeleteKey(hKey, szSubKey);// ������� ��������� ����
//		}
//		RegCloseKey(hKey), hKey = NULL;// ��������� ���� �������
//	}
//}
//
//// ----------------------------------------------------------------------------------------------
//void SendText(LPCTSTR lpData, DWORD cbData, BOOL DataCopy)
//{
//	/*�����������*/
//	HKEY  hSubKey = NULL; // ���������� ���������� ����� �������
//	TCHAR szSubKey[20]; // ��� ���������� ����� �������
//	DWORD cSubKeys = 0; // ���������� ��������� ������
//	LSTATUS lStatus; //��� ����������� � ���������� �������
//
//	if ((lpData != NULL) && (cbData > 0))
//	{
//		lStatus = RegQueryInfoKey(hKey, NULL, NULL, NULL, &cSubKeys, NULL, NULL, NULL, NULL, NULL, NULL, NULL);// ���������� ���������� ��������� ������
//
//		if ((ERROR_SUCCESS == lStatus) && (cSubKeys > 0))
//		{
//			for (DWORD dwIndex = 0; dwIndex < cSubKeys; ++dwIndex)// ����������� ��������� ����� ...
//			{
//				// ������� ��� ���������� ����� � �������� dwIndex
//				DWORD cchSubKey = _countof(szSubKey);
//				lStatus = RegEnumKeyEx(hKey, dwIndex, szSubKey, &cchSubKey, NULL, NULL, NULL, NULL);
//
//				if (lStatus == ERROR_SUCCESS)
//				{
//					lStatus = RegOpenKeyEx(hKey, szSubKey, 0, KEY_QUERY_VALUE, &hSubKey);// ��������� ��������� ����
//				}
//
//				if (ERROR_SUCCESS == lStatus)
//				{
//					HWND hRecvWnd; // ���������� ���� ����������
//					DWORD cb = sizeof(HWND);
//
//					lStatus = RegQueryValueEx(hSubKey, TEXT("hwnd"), NULL, NULL, (LPBYTE)&hRecvWnd, &cb);// �������� ���������� ���� ����������
//
//					if (lStatus == ERROR_SUCCESS)
//					{
//						if (DataCopy != FALSE)
//						{
//							// �������������� ���������
//							COPYDATASTRUCT cds;
//							cds.lpData = (PVOID)lpData;
//							cds.cbData = (cbData + 1) * sizeof(TCHAR);
//							cds.dwData = 0;
//							SendMessage(hRecvWnd, WM_COPYDATA, (WPARAM)hwnd, (LPARAM)&cds);// ���������� ��������� WM_COPYDATA
//						}
//						else
//						{
//							SendMessage(hRecvWnd, WM_SETTEXT, 0, (LPARAM)lpData);// ���������� ��������� WM_SETTEXT
//						}
//					}
//					RegCloseKey(hSubKey), hSubKey = NULL;// ��������� ��������� ���� �������
//				}
//			}
//		}
//	}
//}
//
//
