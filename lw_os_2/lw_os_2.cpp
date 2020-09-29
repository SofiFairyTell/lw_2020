#include <Windows.h>
#include <WindowsX.h>
#include <CommCtrl.h>
#include <tchar.h>
#include "resource.h"
#define LEFT 300
#define TOP  240
//#include "afxres.h"

#define WM_ADDITEM WM_USER+1
#define IDC_TIMER 2002
#define IDC_RADIO_MINUTES   2003
#define IDC_RADIO_SECONDS   2004
#define IDC_EDIT_SEC        2005
#define IDC_UPDOWN          2006
#define IDC_PROGRESS        2007
#define ID_TIMER            1
#pragma region Объявления
	#pragma region Variable, Const
	HWND hWnd = NULL; //дескриптор окна
	HWND hwndMonthCal = NULL; //для календаря
	HWND hDlg = NULL;
	HACCEL hAccel = NULL; //дескриптор акселератора 
	HWND hSaveDlg = NULL; //save as
	DWORD dwTimer = 0; // счётчик таймера
	HWND hRlsDlg = NULL; //replace
		TCHAR szBuffer[100] = TEXT("");
	RECT rc;

	FINDREPLACE findDlg; // структура для диалогового окна "Найти"
	UINT uFindMsgString = 0; // код сообщения FINDMSGSTRING
	HWND hFindDlg = NULL; 

	#pragma endregion
	#pragma region MainWindowFunction
	LRESULT CALLBACK MainWindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	BOOL OnCreate(HWND hwnd, LPCREATESTRUCT lpCRStr);
	void OnDestroy(HWND hwnd);
	void OnTimer(HWND hwnd, UINT id);
	#pragma endregion
	#pragma region Dialog1Function
	INT_PTR CALLBACK DialogProc(HWND hWndlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
	BOOL Dialog_OnInitDialog(HWND hWnd, HWND hWndF, LPARAM lParam);
	void OnAddItem(HWND hWnd);
	void OnCommand(HWND hWnd, int id, HWND hwnCTRL, UINT codeNotify);
	void OnKey(HWND hWnd, int id, HWND hwnCTRL, UINT codeNotify);
	void Dialog_OnClose(HWND hWnd);
	OPENFILENAME OpenFDLG;//для создания диалогового окна Сохранить как
	UINT uOpenMSG = 0;
	void OnFindMsgString(HWND hwnd, LPFINDREPLACE lpFindReplace);
	#pragma endregion
	UINT_PTR CALLBACK FRHookProc(HWND hdlg, UINT uiMsg, WPARAM wParam, LPARAM lParam);
#pragma endregion

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,	LPSTR lpszCmdLine,int  nCmdShow)
{
	MSG  msg;
	BOOL bRet;
	
	// регистрируем оконный класс главного окна...

	WNDCLASSEX wcex = { sizeof(WNDCLASSEX) };
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = MainWindowProc; // оконная процедура
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_BTNFACE + 1);
	wcex.lpszMenuName = MAKEINTRESOURCE(IDR_MENU1);
	wcex.lpszClassName = TEXT("MainWindowClass"); // имя класса
	wcex.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

	if (0 == RegisterClassEx(&wcex)) return -1; //не удалось зарегистрировать новый оконный класс завершаем работу приложения

	LoadLibrary(TEXT("ComCtl32.dll"));//библиотека элементов управления общего пользования 

	hAccel = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDR_ACCELERATOR1));// загружаем таблицу быстрых клавиш
	
	/*создание главного окна на основе нового оконного класса*/
	hWnd = CreateWindowEx(0, TEXT("MainWindowClass"), TEXT("LwOS"),	WS_OVERLAPPEDWINDOW|DS_CENTER,
		CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);
	
	if (NULL == hWnd) return -1; // завершаем работу приложения

	ShowWindow(hWnd, nCmdShow); // отображаем главное окно
	UpdateWindow(hWnd);

	while ((bRet = GetMessage(&msg, NULL, 0, 0))!= FALSE)
	{
		if (!TranslateAccelerator(hWnd, hAccel, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

	}
}

#pragma region Work with Main Window
LRESULT CALLBACK MainWindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	HDC hdc;             // индекс контекста устройства
	PAINTSTRUCT ps;      // структура для рисования

	switch (msg)
	{
		HANDLE_MSG(hwnd, WM_CREATE, OnCreate);
		HANDLE_MSG(hwnd, WM_COMMAND, OnCommand);
		HANDLE_MSG(hwnd, WM_TIMER, OnTimer);
	case WM_ADDITEM:
		{
			OnAddItem(hwnd);
			return 0;
		}
		case WM_LBUTTONDOWN:
		{
			hdc = GetDC(hwnd);
			TextOut(hdc, 10, 40, L"WM_LBUTTONDOWN", 14);
			ReleaseDC(hwnd, hdc);
			return 0;
		}
		case WM_LBUTTONUP:
		{
			hdc = GetDC(hwnd);
			TextOut(hdc, 10, 40, L"WM_LBUTTONUP", 14);
			ReleaseDC(hwnd, hdc);
			return 0;
		}
		case WM_MOUSEWHEEL:
		{
			hdc = GetDC(hwnd);
			TextOut(hdc, 10, 40, L"WM_MOUSEWHEEL", 14);
			ReleaseDC(hwnd, hdc);
			return 0;
		}
		case WM_SIZING:
		{
			hdc = GetDC(hwnd);
			TextOut(hdc, 10, 40, L"WM_SIZING", 14);
			ReleaseDC(hwnd, hdc);
			return 0;
		}
		case WM_LBUTTONDBLCLK:
		{
			hdc = GetDC(hwnd);
			TextOut(hdc, 300, 40, L"WM_LBUTTONDBLCLK", 14);
			ReleaseDC(hwnd, hdc);
			return 0;
		}
		case WM_SYSCHAR:
		{
			hdc = GetDC(hwnd);
			TextOut(hdc, 300, 40, L"WM_SYSCHAR", 14);
			ReleaseDC(hwnd, hdc);
			return 0;
		}
		case WM_SYSKEYUP:
		{
			hdc = GetDC(hwnd);
			TextOut(hdc, 300, 40, L"WM_SYSKEYUP", 14);
			ReleaseDC(hwnd, hdc);
			return 0;
		}
		case WM_DESTROY:
		{
			PostQuitMessage(0);
			return 0;
		}
		
	}
	if (uFindMsgString == msg) // сообщение FINDMSGSTRING
	{
		OnFindMsgString(hwnd, (LPFINDREPLACE)lParam);
		return 0;
	} // if
	return (DefWindowProc(hwnd, msg, wParam, lParam));
	/*switch (msg)
	{
		HANDLE_MSG(hwnd, WM_CREATE, OnCreate);
		HANDLE_MSG(hwnd, WM_DESTROY, OnDestroy);
	case WM_ADDITEM:
		return 0;
	}
	return DefWindowProc(hwnd, msg, wParam, lParam);*/
}

BOOL OnCreate(HWND hwnd, LPCREATESTRUCT lpCRStr) 
{
	CreateWindowEx(0, TEXT("ListBox"), NULL, WS_CHILD | WS_VISIBLE | WS_BORDER, 10, 10, 250, 410, hwnd, (HMENU)IDC_LIST1, lpCRStr->hInstance, NULL);
	hwndMonthCal = CreateWindowEx(0, TEXT("SysMonthCal32"), NULL, WS_CHILD | WS_VISIBLE|WS_BORDER| MCS_DAYSTATE| MCS_MULTISELECT, 163, 31, 140, 100, hwnd, (HMENU)IDC_MONTHCALENDAR1, lpCRStr->hInstance, NULL);
	// Return if creating the month calendar failed. 
	if (hwndMonthCal == NULL) return HRESULT_FROM_WIN32(GetLastError());
	// Get the size required to show an entire month.
	MonthCal_GetMinReqRect(hwndMonthCal, &rc);
	// Resize the control now that the size values have been obtained.
	SetWindowPos(hwndMonthCal, NULL, LEFT, TOP,	rc.right, rc.bottom, SWP_NOZORDER);
	// Set the calendar to the annual view.
	MonthCal_SetCurrentView(hwndMonthCal, MCMV_YEAR);
		
	
	CreateWindowEx(0, TEXT("Button"), TEXT("ДОБАВИТЬ ЗАПИСЬ"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 270, 10, 200, 40, hwnd, (HMENU)ID_NEW_RECORD, lpCRStr->hInstance, NULL);
	CreateWindowEx(0, TEXT("Button"), TEXT("УДАЛИТЬ ЗАПИСЬ"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 270, 55, 200, 40, hwnd, (HMENU)ID_DEL_RECORD, lpCRStr->hInstance, NULL);
	CreateWindowEx(0, TEXT("Button"), TEXT("ЗАМЕНИТЬ ЗАПИСЬ"),	WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 270, 145, 200, 40, hwnd, (HMENU)ID_REPLACE, lpCRStr->hInstance, NULL);
	return TRUE;
}
void OnDestroy(HWND hwnd)
{
	int UserAnswer = MessageBox(hWnd, TEXT("Завершить работу?"), TEXT("Выход"), MB_YESNO | MB_ICONQUESTION | MB_DEFBUTTON1);
	if (IDYES == UserAnswer)
	{
		DestroyWindow(hWnd);
	}
}
void OnTimer(HWND hwnd, UINT id)
{
	if (ID_TIMER == id)
	{
		// увеличиваем индикатор процесса
		SendDlgItemMessage(hwnd, IDC_PROGRESS, PBM_STEPIT, 0, 0);

		if (0 == --dwTimer) // время истекло
		{
			// останавливаем таймер
			KillTimer(hwnd, ID_TIMER);

			// разблокируем элементы управления таймером

			EnableWindow(GetDlgItem(hwnd, IDC_EDIT_SEC), TRUE);
			EnableWindow(GetDlgItem(hwnd, IDC_UPDOWN), TRUE);
			EnableWindow(GetDlgItem(hwnd, IDC_TIMER), TRUE);
			EnableWindow(GetDlgItem(hwnd, IDC_RADIO_SECONDS), TRUE);
			EnableWindow(GetDlgItem(hwnd, IDC_RADIO_MINUTES), TRUE);

			// устанавливаем в начало позицию индикатора процесса
			SendDlgItemMessage(hwnd, IDC_PROGRESS, PBM_SETPOS, (WPARAM)0, 0);
		} // if
	} // if
}
void OnCommand(HWND hWnd, int id, HWND hwnCTRL, UINT codeNotify)
{
	HINSTANCE hInstance = GetWindowInstance(hWnd);
	switch (id)
	{
	case ID_NEW_RECORD:
	{
		int DialogResult = DialogBoxParam(hInstance, MAKEINTRESOURCE(IDD_DIALOG1), hWnd, (DialogProc),NULL);
		if (IDOK == DialogResult)
		{
			SendMessage(hWnd, WM_ADDITEM, 0, 0);
		}
	} break;
	case ID_DEL_RECORD: // нажата кнопка "Удалить запись"
	{
		// получим дескриптор списка
		HWND hwndCtl = GetDlgItem(hWnd, IDC_LIST1);

		// определим текущий выделенный элемент в списке
		int iItem = ListBox_GetCurSel(hwndCtl);

		if (iItem != -1)
		{
			int mbResult = MessageBox(hWnd, TEXT("Удалить выбранный элемент?"), TEXT("LW_OS_2"), MB_YESNO | MB_ICONQUESTION);

			if (mbResult == IDYES)
			{
				// удаляем выделенный элемент из списка
				ListBox_DeleteString(hwndCtl, iItem);
			} 
		} 
	}
	break;
	case ID_SAVE_AS:
	{
		TCHAR szFileName[MAX_PATH] = TEXT("");
		OpenFDLG.lStructSize = sizeof(OPENFILENAME);
		OpenFDLG.hInstance = GetWindowInstance(hWnd);
		OpenFDLG.lpstrFilter = TEXT("Bitmape Files(*.bmp)\0*.bmp\0 JPEG files(*.jpg)\0*.jpg\0TEXT files(*.txt)\0*.txt\0All files(*.*)\0*.*\0\0");
		OpenFDLG.lpstrFile = szFileName;
		OpenFDLG.nMaxFile = _countof(szFileName);
		OpenFDLG.lpstrTitle = TEXT("Сохранить как");
		OpenFDLG.Flags = OFN_EXPLORER | OFN_ENABLESIZING | OFN_CREATEPROMPT | OFN_OVERWRITEPROMPT| OFN_EXTENSIONDIFFERENT;
		OpenFDLG.lpstrDefExt = TEXT("txt");
		if (GetSaveFileName(&OpenFDLG) != FALSE)
		{
			MessageBox(hWnd, szFileName, TEXT("Сохранить как"), MB_OK | MB_ICONINFORMATION);
		}
	}break;
	case ID_REPLACE: // нажата кнопка "Найти запись"
		if (0 == uFindMsgString)
		{
			uFindMsgString = RegisterWindowMessage(FINDMSGSTRING);// получим код сообщения FINDMSGSTRING
		} 
		if (IsWindow(hFindDlg) == FALSE)
		{
			findDlg.lStructSize = sizeof(FINDREPLACE);
			findDlg.hInstance = hInstance;// указываем дескриптор экземпляра приложения
			findDlg.hwndOwner = hWnd;			// указываем дескриптор окна владельца
			findDlg.lpstrFindWhat = szBuffer;
			findDlg.lpstrReplaceWith = szBuffer;
			findDlg.wReplaceWithLen  = _countof(szBuffer);			// указываем размер буфера
			findDlg.wFindWhatLen = _countof(szBuffer);	
			findDlg.Flags = FR_REPLACE|FR_REPLACEALL|FR_DIALOGTERM|FR_FINDNEXT;
			hFindDlg = ReplaceText(&findDlg);
		} // if
		break;
	}
}
void OnKey(HWND hWnd, int id, HWND hwnCTRL, UINT codeNotify)
{

}
void OnFindMsgString(HWND hwnd, LPFINDREPLACE lpFindReplace)
{
	
	if (lpFindReplace->Flags&FR_FINDNEXT) // нажата кнопка "Найти далее"
	{
		// получим дескриптор списка
		HWND hwndCtl = GetDlgItem(hwnd, IDC_LIST1);

		// определим текущий выделенный элемент в списке
		int iItem = ListBox_GetCurSel(hwndCtl);
	
		// выполним поиск указанного текста в списке
		// сразу после текущего выделенного элемента
		iItem = ListBox_FindString(hwndCtl, iItem, lpFindReplace->lpstrFindWhat);

		// выделяем найденный элемент
		ListBox_SetCurSel(hwndCtl, iItem);
		//ListBox_
		if (LB_ERR == iItem) // элемент не найден
		{
			MessageBox(hFindDlg, TEXT("Элемент не найден"), TEXT("LWOS"), MB_OK | MB_ICONINFORMATION);
		} 
	} 
} 

UINT_PTR CALLBACK FRHookProc(HWND hdlg, UINT uiMsg, WPARAM wParam, LPARAM lParam)
{
	if (uiMsg == WM_INITDIALOG)
	{
		HWND hBtn = GetDlgItem(hdlg, 1); // The "Find Next" button is ID 1
		if (hBtn)
			ShowWindow(hBtn, SW_HIDE);
	}
	return 0;
}
void OnAddItem(HWND hWnd)
{
	HWND hWNDctrl = GetDlgItem(hWnd, IDC_LIST1);
	int iItem = ListBox_AddString(hWNDctrl, szBuffer);
	ListBox_SetCurSel(hWNDctrl, iItem);
}
#pragma endregion

#pragma region Work with Dialog1 Window
INT_PTR CALLBACK DialogProc(HWND hWndlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_INITDIALOG:
	{
		BOOL bRet = HANDLE_WM_INITDIALOG(hWndlg, wParam, lParam, Dialog_OnInitDialog);
		return SetDlgMsgResult(hWndlg, uMsg, bRet);
	}break;
	case WM_COMMAND:
	{
		switch (LOWORD(wParam))
		{
		case IDOK:
		{
			int cch = GetDlgItemText(hWndlg, IDC_EDIT1, szBuffer, _countof(szBuffer));
			SetDlgItemText(hWndlg, IDC_EDIT1, NULL);
			SendMessage(GetParent(hWndlg), WM_ADDITEM, 0, 0);
			//MessageBox(hWndlg, TEXT("Ok"), NULL, NULL); 
		} break;
		case IDCANCEL:
		{
			EndDialog(hWndlg, 0);
			//MessageBox(hWndlg, TEXT("Cancel"), NULL, NULL);
			return FALSE;
		}break;

		}
		/*switch (LOWORD(wParam))
		{
		case IDOK:
		{
			//int cch = GetDlgItemText(hWndlg, IDC_EDIT1, szBuffer, _countof(szBuffer));
			//SetDlgItemText(hWnd, IDC_EDIT1, _T(""));
			//if (0 == cch) // в редактируемого поле нет текста
			//{
			//	 /*получим дескриптор окна редактируемого поля*/
			//	HWND hwndEdit = GetDlgItem(hWndlg, IDC_EDIT1);
			//	/*блок ниже для всплывающих подсказок*/
			//	EDITBALLOONTIP ebt;
			//	ebt.cbStruct = sizeof(EDITBALLOONTIP);
			//	ebt.pszTitle = L"Добавить запись";
			//	ebt.pszText = L"Укажите название новой записи";
			//	ebt.ttiIcon = TTI_ERROR_LARGE;
			//	SendMessage(hWnd, WM_ADDITEM, 0, 0);
			//	Edit_ShowBalloonTip(hwndEdit, &ebt);
			//	SendMessage(hWndlg, EM_SHOWBALLOONTIP, 0, (LPARAM)&ebt);
			//}
			//else
			//	if (hWndlg == hDlg)
			//	{
			//		// очистим редактируемое поле
			//		SetDlgItemText(hWndlg, IDC_EDIT1, NULL);
			//		 //отправляем окну-владельцу сообщение о том, что нужно добавить запись
			//		SendMessage(GetParent(hWndlg), WM_ADDITEM, 0, 0);
			//	} // if

		/*}break;
		// потерпел неудачу. 
		/*case IDCANCEL:
			EndDialog(hWndlg, 0);
			return FALSE;
		*/}
		break;

	case WM_CLOSE:
	{
		HANDLE_WM_CLOSE(hWndlg, wParam, lParam, Dialog_OnClose);
		/*EndDialog(hWndlg, 0);
		return FALSE;*/
	}
	}
	return FALSE;
	}




BOOL Dialog_OnInitDialog(HWND hWnd, HWND hWndF, LPARAM lParam)
{
	HWND hwndEdit = GetDlgItem(hWnd, IDC_EDIT1);
	Edit_LimitText(hwndEdit, _countof(szBuffer)-1);
	Edit_SetCueBannerText(hwndEdit, L"Новая запись");
	return TRUE;
}

void Dialog_OnClose(HWND hWnd)
{
	if (hWnd == hDlg) { DestroyWindow(hWnd); }
	else { EndDialog(hWnd, 0); }
}

#pragma endregion





