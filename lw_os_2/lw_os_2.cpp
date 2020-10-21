#include <Windows.h>
#include <WindowsX.h>
#include <CommCtrl.h>
#include <tchar.h>
#include <fstream>
#include <string>
#include <codecvt>
#include <comdef.h> //для bstr_t
#include "resource.h"

#define BEEP_TIMER 1
#define LEFT 300
#define TOP  240

#pragma comment(linker,"\"/manifestdependency:type                  = 'win32' \
                                              name                  = 'Microsoft.Windows.Common-Controls' \
                                              version               = '6.0.0.0' \
                                              processorArchitecture = '*' \
                                              publicKeyToken        = '6595b64144ccf1df' \
                                              language              = '*'\"")
#pragma warning(disable : 4996) //отключает Ошибку deprecate. Возникает, когда используется устаревшая функция 

#define WM_ADDITEM WM_USER+1
#define BOLDDAY(ds, iDay)  \
        if (iDay > 0 && iDay < 32)(ds) |= (0x00000001 << (iDay - 1)) //for process the MCN_GETDAYSTATE notification code


#pragma region Объявления
	#pragma region Handle of Window
	HWND hWnd = NULL; //дескриптор окна
	HWND hDlg = NULL; //декприптор диалогового окна
	HWND hSaveDlg = NULL; //save as
	HWND hRlsDlg = NULL; //replace
	#pragma endregion
	#pragma region Handle of control elements
		HWND hwndMonthCal = NULL; //для календаря
		HACCEL hAccel = NULL; //дескриптор акселератора 
	#pragma endregion
	#pragma region For FINDREPLACE dialog
		FINDREPLACE findDlg; // структура для диалогового окна "Найти"
		UINT uFindMsgString = 0; // код сообщения FINDMSGSTRING
		HWND hFindDlg = NULL;
		/*Для FINDREPLACE*/
		TCHAR szBuffer[100] = TEXT("");
		TCHAR szBuffer1[100] = TEXT("");	
		void OnFindMsgString(HWND hwnd, LPFINDREPLACE lpFindReplace); //Поиск, замена в listbox		
	#pragma endregion

	#pragma region MainWindowFunction
	//Здесь функции, переменные, структуры и т.п. для работы с главным окном приложения
	
	LRESULT CALLBACK MainWindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	BOOL OnCreate(HWND hwnd, LPCREATESTRUCT lpCRStr);//создание окна и элементов на нем
	void OnCommand(HWND hWnd, int id, HWND hwnCTRL, UINT codeNotify);//обработка сообщений от кнопок и SAVEAS
	
	//Обработка событий и сообщений мыши
	void OnLbuttonDClick(HWND hwnd, BOOL fDoubleClick, int x, int y, UINT keyFlags);//для WM_LBUTTONDBLCLK и WM_LBUTTONDOWN
	void OnLButtonUp(HWND hwnd, int x, int y, UINT keyFlags);
	void MouseWheel(HWND hwnd, int xPos, int yPos, int zDelta, UINT fwKeys);

	//Обработка нажатия событий и сообщений клавиатуры
	void OnSysKey(HWND hwnd, UINT vk, BOOL fDown, int cRepeat, UINT flags);//для WM_SYSKEYUP
	
	/*Для изменения цвета окна, когда было обработано WM_SYSCHAR */
	RECT rc;
	HBRUSH brushes[3]; //кисти для изменения цвета окна
	int brush_index = 0;
	
	// обработка сообщения WM_TIMER
	void OnTimer(HWND hwnd, UINT id);//SetTimer, KillTimer, запуск зведного неба
	static int sec = 0; //записывается время работы с программой
	char workTime[10];//записывается время из sec для демонстрации пользователю
	
	SYSTEMTIME st;//структура для определения времени и даты
	void OnSizing(HWND hwnd, LPRECT lpRect, WPARAM wParam); //обработка onSizing
	
	//SAVEFILE AS dialog обработка ID_SAVE_AS
	OPENFILENAME OpenFDLG;//для создания диалогового окна Сохранить как
	UINT uOpenMSG = 0;

	#pragma endregion
	
	#pragma region EditDialogFunction
	//Здесь функции, переменные и т.п. для работы с диалоговым окном, добавляющим строки в listbox	
		INT_PTR CALLBACK DialogProc(HWND hWndlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
		BOOL Dialog_OnInitDialog(HWND hWnd, HWND hWndF, LPARAM lParam);
		void OnAddItem(HWND hWnd);
		void Dialog_OnClose(HWND hWnd);
	#pragma endregion
	#pragma region  DialogProcMany
	//Здесь функции, переменные и т.п. если необходимо добавить более чем 1 запись в listbox*/
			void DialogMany_OnClose(HWND hWnd);
			BOOL Dialog_OnInitDialogMany(HWND hWnd, HWND hWndF, LPARAM lParam);
			INT_PTR CALLBACK DialogProcMany(HWND hWndlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
	#pragma endregion
	
#pragma endregion
	
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,	LPSTR lpszCmdLine,int  nCmdShow)
{
	MSG  msg;
	BOOL bRet;
	brushes[0] = (HBRUSH)CreateSolidBrush(RGB(0, 0, 100)); //Радикально-синий цвет
	brushes[1] = (HBRUSH)CreateSolidBrush(RGB(0, 100, 0)); //Радикально-зеленый цвет
	brushes[2] = (HBRUSH)CreateSolidBrush(RGB(100, 0, 0)); //Радикально-красный цвет
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
	wcex.hbrBackground = brushes[brush_index];

	if (0 == RegisterClassEx(&wcex)) return -1; //не удалось зарегистрировать новый оконный класс завершаем работу приложения

	LoadLibrary(TEXT("ComCtl32.dll"));//библиотека элементов управления общего пользования 

	hAccel = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDR_ACCELERATOR1));// загружаем таблицу быстрых клавиш
	
	/*создание главного окна на основе нового оконного класса*/
	hWnd = CreateWindowEx(0, TEXT("MainWindowClass"), TEXT("LwOS"),	WS_OVERLAPPEDWINDOW|DS_CENTER, CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);
	
	if (NULL == hWnd) return -1; // завершаем работу приложения

	ShowWindow(hWnd, nCmdShow); // отображаем главное окно
	UpdateWindow(hWnd);

	SetTimer(hWnd, 1, 1000, (TIMERPROC)NULL);//установка таймера, который считает время работы с приложением и показывает "звезды"
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
			HANDLE_MSG(hwnd, WM_LBUTTONDBLCLK, OnLbuttonDClick);
			HANDLE_MSG(hwnd, WM_LBUTTONDOWN, OnLbuttonDClick);
			HANDLE_MSG(hwnd, WM_LBUTTONUP, OnLButtonUp);
			HANDLE_MSG(hwnd, WM_SYSKEYDOWN, OnSysKey);
			HANDLE_MSG(hwnd, WM_SYSKEYUP, OnSysKey);
			HANDLE_MSG(hwnd, WM_KEYDOWN, OnSysKey);//чтобы поймать стрелки
			HANDLE_MSG(hwnd, WM_KEYUP, OnSysKey);//чтобы поймать стрелки
			HANDLE_MSG(hwnd, WM_MOUSEWHEEL, MouseWheel);
				
		case WM_TIMER:
		{
			OnTimer(hwnd,0);
			sec++;
			// В ответ на сообщение таймера выдаем
			 // звуковой сигнал
			MessageBeep(-1);
			return 0;
		}
			case WM_ADDITEM:
			{
				OnAddItem(hwnd);
				return 0;
			}
			
			case WM_SIZING:
			{
				OnSizing(hwnd, (LPRECT)lParam, (WPARAM)wParam);
				return 0;
			}	

			case WM_SYSCHAR:
			{
				//ввод символов в программу
				/*ALT+<ANY KEY> перекрашивает экран одним из 3-х цветов*/
				wchar_t x = wParam;
				HWND hwndCtl = GetDlgItem(hwnd, IDC_LIST1);
				int count = ListBox_GetCount(hwndCtl);
				for (int i = 0; i < count; i++)
				{
					int len = ListBox_GetTextLen(hwndCtl, i);
					wchar_t* buffer = new wchar_t[len + 1];
					ListBox_GetText(hwndCtl, i, buffer);
					/*Найдет первое где встречается введенный символ*/
					for (int j = 0; j < len + 1; j++)
					{
						if (wcschr(buffer,x))
						{
							ListBox_SetCurSel(hwndCtl, i);
							if (brush_index == 2) 
							brush_index = 0;
							else brush_index++;
							InvalidateRect(hwnd, NULL, FALSE);	
							break;
						}
						else j++;
					}
					delete[]buffer;	
				}
				/*Здесь ошибка не возникает
				int len = ListBox_GetTextLen(hwndCtl, 0);
				char* buffer = new char[len + 1];
				delete[]buffer;*/			
				return 0;
			}
			case WM_PAINT:
			{
				PAINTSTRUCT ps;
				BeginPaint(hwnd, &ps);
				FillRect(ps.hdc, &ps.rcPaint, brushes[brush_index]);
				EndPaint(hwnd, &ps);
				return 0;
			}

			case WM_DESTROY:
			{
				KillTimer(hwnd, 1);
				KillTimer(hwnd, BEEP_TIMER);
				itoa(sec, workTime, 10); //преобразование подсчитанных секунд в символы
				MessageBoxA(NULL, (LPSTR)workTime, "Время работы программы (сек.):", MB_ICONASTERISK | MB_OK);
				PostQuitMessage(0);//without this app contie work after close messagebox
				return 0;
			}
			
			case WM_NOTIFY:
			{	
				switch (((LPNMHDR)lParam)->code)
				{
				case MCN_GETDAYSTATE:
				{
					/*выделяет 1  и 15 день месяца*/
				//*https://docs.microsoft.com/ru-ru/windows/win32/controls/set-day-states */
					MONTHDAYSTATE rgMonths[12] = { 0 };
					int cMonths = ((NMDAYSTATE*)lParam)->cDayState;
					for (int i = 0; i < cMonths; i++)
					{
						BOLDDAY(rgMonths[i], 1);
						BOLDDAY(rgMonths[i], 15);
					}
					((NMDAYSTATE*)lParam)->prgDayState = rgMonths;
					return TRUE;
				}//MCN_GETDAYSTATE
				case MCN_SELECT:
				{
					TCHAR szText[100];			
					MonthCal_GetSelRange(GetDlgItem(hwnd, IDC_MONTHCALENDAR1), &st);
					GetDateFormat(LOCALE_USER_DEFAULT, DATE_LONGDATE, &st, NULL, szText, _countof(szText));
					HWND hwndCtl = GetDlgItem(hwnd, IDC_LIST1);
					ListBox_AddString(hwndCtl, szText);//добавляем выделенную дату в список
					return TRUE;
				}//MCN_SELECT
				}
			}
			
			}
		if (uFindMsgString == msg) 
		{
			OnFindMsgString(hwnd, (LPFINDREPLACE)lParam);
			return 0;
		} 
		return (DefWindowProc(hwnd, msg, wParam, lParam));
	}
	

	BOOL OnCreate(HWND hwnd, LPCREATESTRUCT lpCRStr) 
	{
		
		CreateWindowEx(0, TEXT("ListBox"), NULL, WS_CHILD | WS_VISIBLE | WS_BORDER| LBS_WANTKEYBOARDINPUT| LBS_NOTIFY, 10, 10, 250, 410, hwnd, (HMENU)IDC_LIST1, lpCRStr->hInstance, NULL);
		hwndMonthCal = CreateWindowEx(0, TEXT("SysMonthCal32"), NULL, WS_CHILD | WS_VISIBLE|WS_BORDER| MCS_DAYSTATE| MCS_MULTISELECT, 163, 31, 140, 100, hwnd, (HMENU)IDC_MONTHCALENDAR1, lpCRStr->hInstance, NULL);
		// Return if creating the month calendar failed. 
		if (hwndMonthCal == NULL) return HRESULT_FROM_WIN32(GetLastError());
		// Get the size required to show an entire month.
		MonthCal_GetMinReqRect(hwndMonthCal, &rc);
		// Resize the control now that the size values have been obtained.
		SetWindowPos(hwndMonthCal, NULL, LEFT, TOP,	rc.right, rc.bottom, SWP_NOZORDER);
		
		MonthCal_SetCurrentView(hwndMonthCal, MCMV_YEAR);// Set the calendar to the annual view.
		
														 
		CreateWindowEx(0, TEXT("Button"), TEXT("ДОБАВИТЬ ЗАПИСЬ"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 270, 10, 200, 40, hwnd, (HMENU)ID_NEW_RECORD, lpCRStr->hInstance, NULL);
		// создаём кнопку "Добавить неск. записей"
		CreateWindowEx(0, TEXT("Button"), TEXT("ДОБАВИТЬ ЗАПИСИ"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 270, 55, 200, 40, hwnd, (HMENU)ID_NEW_RECORD2, lpCRStr->hInstance, NULL);
		CreateWindowEx(0, TEXT("Button"), TEXT("УДАЛИТЬ  ЗАПИСЬ"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 270, 100, 200, 40, hwnd, (HMENU)ID_DEL_RECORD, lpCRStr->hInstance, NULL);
		CreateWindowEx(0, TEXT("Button"), TEXT("ЗАМЕНИТЬ ЗАПИСЬ"),	WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 270, 145, 200, 40, hwnd, (HMENU)ID_REPLACE, lpCRStr->hInstance, NULL);
	
		SetTimer(hwnd, BEEP_TIMER, 1000, NULL);
		return TRUE;
	}
	
	void OnTimer(HWND hwnd, UINT id)
	{
		int x, y;
		HDC hdc;
			hdc = GetDC(hwnd); //Выделяем контекст отображения
			x = rand() % 1600; //случайное число по х от 0 до 500
			y = rand() % 900; //по у от 0 до 400
			SetPixel(hdc, x, y, RGB(255, 255, 255)); //Выводим точку
			ReleaseDC(hwnd, hdc);
	}

	void OnCommand(HWND hWnd, int id, HWND hwnCTRL, UINT codeNotify)
	{
		HINSTANCE hInstance = GetWindowInstance(hWnd);

		switch (id)
		{
		case ID_NEW_RECORD:
		{
			int DialogResult = DialogBoxParam(hInstance, MAKEINTRESOURCE(IDD_DIALOG1), hWnd, (DialogProc), NULL);
			if (IDOK == DialogResult)
			{
				SendMessage(hWnd, WM_ADDITEM, 0, 0);
			}
		} break;
		case ID_NEW_RECORD2:
		{
			int DialogResult = DialogBoxParam(hInstance, MAKEINTRESOURCE(IDD_DIALOG2), hWnd, (DialogProcMany), NULL);
			if (IDOK == DialogResult)
			{
				SendMessage(hWnd, WM_ADDITEM, 0, 0);
			}
		}
		break;
		case ID_DEL_RECORD: // нажата кнопка "Удалить запись"
		{
			HWND hwndCtl = GetDlgItem(hWnd, IDC_LIST1);	// получим дескриптор списка
			int iItem = ListBox_GetCurSel(hwndCtl);// определим текущий выделенный элемент в списке
			if (iItem != -1)
			{
				int mbResult = MessageBox(hWnd, TEXT("Удалить выбранный элемент?"), TEXT("LW_OS_2"), MB_YESNO | MB_ICONQUESTION);

				if (mbResult == IDYES)
				{
					ListBox_DeleteString(hwndCtl, iItem);// удаляем выделенный элемент из списка
				}
			}
		}
		break;
		case ID_SAVE_AS:
		{			
			char pListBox[] = { GetListBoxInfo(hWnd) };
			TCHAR szFileName[MAX_PATH] = TEXT("");
			OpenFDLG.lStructSize = sizeof(OPENFILENAME);
			OpenFDLG.hInstance = GetWindowInstance(hWnd);
			OpenFDLG.lpstrFilter = TEXT("Bitmape Files(*.bmp)\0*.bmp\0 JPEG files(*.jpg)\0*.jpg\0TEXT files(*.txt)\0*.txt\0All files(*.*)\0*.*\0\0");
			OpenFDLG.lpstrFile = szFileName;
			OpenFDLG.nMaxFile = _countof(szFileName);
			OpenFDLG.lpstrTitle = TEXT("Сохранить как");
			OpenFDLG.Flags = OFN_EXPLORER | OFN_ENABLESIZING | OFN_CREATEPROMPT | OFN_OVERWRITEPROMPT | OFN_EXTENSIONDIFFERENT;
			OpenFDLG.lpstrDefExt = TEXT("txt");

			if (GetSaveFileName(&OpenFDLG) != FALSE)
			{
				MessageBox(hWnd, szFileName, TEXT("Сохранить как"), MB_OK | MB_ICONINFORMATION);
				std::wofstream fout(szFileName); // создаём объект класса ofstream для записи
				fout << "Файл создан в программе LWOS\n";
				/*Соберем данные из listbox и добавим их в файл*/
				HWND hwndCtl = GetDlgItem(hWnd, IDC_LIST1);
				int count = ListBox_GetCount(hwndCtl);
				for (int i = 0; i < count; i++)
				{
					int len = ListBox_GetTextLen(hwndCtl, i);
					wchar_t* buffer = new wchar_t[len + 1];
					ListBox_GetText(hwndCtl, i, buffer);
					//std::wstring_convert<std::codecvt_utf8<wchar_t>> utf8_conv;
					//std::wstring ws(buffer);
					fout << buffer << "\n";
					//fout << ws << "\n";
					//fout.write(reinterpret_cast<char const*>(buffer), sizeof(buffer)); //<< "\n";
					delete[]buffer;
				}
				fout.close();

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
				findDlg.lpstrReplaceWith = szBuffer1;
				findDlg.wReplaceWithLen = _countof(szBuffer1);			// указываем размер буфера
				findDlg.wFindWhatLen = _countof(szBuffer);
				hFindDlg = ReplaceText(&findDlg);
			}
			break;
		}
	}
		void OnLbuttonDClick(HWND hwnd, BOOL fDoubleClick, int x, int y, UINT keyFlags)
		{
			// передаём фокус клавиатуры главному окну
			SetFocus(hwnd);

			if (fDoubleClick == FALSE) // двойной клик
			{
				if (IsMaximized(hwnd)) // окно развёрнуто
				{
					ShowWindow(hwnd, SW_RESTORE);
				}
				else
				{
					// развернём окно
					ShowWindow(hwnd, SW_MAXIMIZE);
				}
			}

		}
		void OnLButtonUp(HWND hwnd, int x, int y, UINT keyFlags)
		{
			ReleaseCapture(); // освободим мышь
		}
		void OnSysKey(HWND hwnd, UINT vk, BOOL fDown, int cRepeat, UINT flags)
		{
			RECT rect; // размеры окна

	   // получим размеры окна
			GetWindowRect(hwnd, &rect);
			
			if (fDown)
			{
				
				switch (vk)
				{
				case VK_LEFT: // нажата стрелка влево
					// перемещаем окно влево
					SetWindowPos(hwnd, NULL, rect.left - 10, rect.top, 0, 0, SWP_NOSIZE);
					break;

				case VK_RIGHT: // нажата стрелка вправо
					// перемещаем окно вправо
					SetWindowPos(hwnd, NULL, rect.left + 10, rect.top, 0, 0, SWP_NOSIZE);
					break;

				case VK_UP: // нажата стрелка вверх
					// перемещаем окно вверх
					SetWindowPos(hwnd, NULL, rect.left, rect.top - 10, 0, 0, SWP_NOSIZE);
					break;

				case VK_DOWN: // нажата стрелка вниз
					// перемещаем окно вниз
					SetWindowPos(hwnd, NULL, rect.left, rect.top + 10, 0, 0, SWP_NOSIZE);
					break;
				//case VK_MENU:
				//	SetWindowPos(hwnd, NULL,0, 0, 0, 0, SWP_NOSIZE);//левый верхний угол
				//	break;
				
				case VK_HOME:
						break;
				} // switch
			} // if
			else
			{
				switch (vk)
				{
				case VK_LEFT: // отпущена стрелка влево
					// перемещаем окно к левому краю
					SetWindowPos(hwnd, NULL, 0, rect.top, 0, 0, SWP_NOSIZE);
					break;

				case VK_UP: // отпущена стрелка вверх
					// перемещаем окно к верхнему краю
					SetWindowPos(hwnd, NULL, rect.left, 0, 0, 0, SWP_NOSIZE);
					break;
				}
			}
		}
		void OnSizing(HWND hwnd, LPRECT lpRect, WPARAM wParam)
		{
			/*изменение одной стороны ведет за собой увеличение другой*/
			static double fixedRate = 1.0;
			/*LPRECT lpRect = (LPRECT)lParam;*/
			int w = lpRect->right - lpRect->left;//ширина
			int h = lpRect->bottom - lpRect->top;//высота
			int dw = (int)(h * fixedRate - w);//диагональ при изменении ширины
			int dh = (int)(w / fixedRate - h);//диагональ при изменении высоты
			switch (wParam)
			{
			case WMSZ_TOP:
			case WMSZ_BOTTOM:
				lpRect->right += dw;
				break;
			case WMSZ_LEFT:
			case WMSZ_RIGHT:
				lpRect->bottom += dh;
				break;
			case WMSZ_TOPLEFT:
				if (dw > 0) lpRect->left -= dw;
				else lpRect->top -= dh;
				break;
			case WMSZ_TOPRIGHT:
				if (dw > 0) lpRect->right += dw;
				else lpRect->top -= dh;
				break;
			case WMSZ_BOTTOMLEFT:
				if (dw > 0) lpRect->left -= dw;
				else lpRect->bottom += dh;
				break;
			}
		}	// OnSizing
		void MouseWheel(HWND hwnd, int xPos, int yPos, int zDelta, UINT fwKeys)
		{
			if (fwKeys & MK_SHIFT) // нажата клавиша SHIFT
			{
				RECT rect; // размеры окна

				// получим размеры окна
				GetWindowRect(hwnd, &rect);

				// изменим размеры

				int d = 5 * zDelta / WHEEL_DELTA;
				InflateRect(&rect, d, d);

				MoveWindow(hwnd, rect.left, rect.top, (rect.right - rect.left), (rect.bottom - rect.top), TRUE);
			} // if
		} // OnMouseWheel


#pragma endregion
#pragma region For FINDREPLACE dialog
		void OnFindMsgString(HWND hwnd, LPFINDREPLACE lpFindReplace)
		{
			HWND hwndCtl = GetDlgItem(hwnd, IDC_LIST1);
			if (lpFindReplace->Flags&FR_FINDNEXT) // нажата кнопка "Найти далее"
			{

				int iItem = ListBox_GetCurSel(hwndCtl);// определим текущий выделенный элемент в списке
				iItem = ListBox_FindString(hwndCtl, iItem, lpFindReplace->lpstrFindWhat);// выполним поиск указанного текста в списке
				ListBox_SetCurSel(hwndCtl, iItem);// выделяем найденный элемент
				if (LB_ERR == iItem) // элемент не найден
				{
					MessageBox(hFindDlg, TEXT("Элемент не найден"), TEXT("LWOS"), MB_OK | MB_ICONINFORMATION);
				}

			}
			else
			{
				if (lpFindReplace->Flags&FR_REPLACE) // нажата кнопка "Заменить"
				{

					int iItem = ListBox_GetCurSel(hwndCtl);
					iItem = ListBox_FindString(hwndCtl, iItem, lpFindReplace->lpstrFindWhat);
					ListBox_SetCurSel(hwndCtl, iItem);
					if (LB_ERR == iItem) // элемент не найден
					{
						MessageBox(hFindDlg, TEXT("Элемент не найден"), TEXT("LWOS"), MB_OK | MB_ICONINFORMATION);
					}
					if (iItem != -1)
					{
						ListBox_DeleteString(hwndCtl, iItem);// удаляем выделенный элемент из списка
						ListBox_InsertString(hwndCtl, iItem, lpFindReplace->lpstrReplaceWith); //добавление на то же место
						SetForegroundWindow(hwndCtl);
					}
				}
				else
				if (lpFindReplace->Flags&FR_REPLACEALL)
					{
						int iItem = -1;
						for (;;)
						{
							int iItem = ListBox_GetCurSel(hwndCtl);
							iItem = ListBox_FindString(hwndCtl, iItem, lpFindReplace->lpstrFindWhat);
							ListBox_SetCurSel(hwndCtl, iItem);//выделяем элемент
							if (LB_ERR == iItem) // элемент не найден
							{
								MessageBox(hFindDlg, TEXT("Элемент не найден"), TEXT("LWOS"), MB_OK | MB_ICONINFORMATION);
								break;
							}
							if (iItem != -1)
							{
								ListBox_DeleteString(hwndCtl, iItem);// удаляем выделенный элемент из списка
								ListBox_InsertString(hwndCtl, iItem, lpFindReplace->lpstrReplaceWith); //добавление на то же место
								SetForegroundWindow(hwndCtl);
							}
						}//for
					}//if
			}
		}
		
#pragma endregion
#pragma region EditDialogFunction
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
					SendMessage(GetParent(hWndlg), WM_ADDITEM, 0, 0); //отправка текста с сообщением
				} break;
				case IDCANCEL:
				{
					EndDialog(hWndlg, 0);
					return TRUE;
				}break;
				}
			} break;
			case WM_CLOSE:
			{
				HANDLE_WM_CLOSE(hWndlg, wParam, lParam, Dialog_OnClose);
				return TRUE;
			}break;
			}
			return FALSE;
		}

		BOOL Dialog_OnInitDialog(HWND hWnd, HWND hWndF, LPARAM lParam)
		{
			HWND hwndEdit = GetDlgItem(hWnd, IDC_EDIT1);
			Edit_LimitText(hwndEdit, _countof(szBuffer) - 1);
			Edit_SetCueBannerText(hwndEdit, L"Новая запись");
			return TRUE;
		}
		void OnAddItem(HWND hWnd)
		{
			HWND hWNDctrl = GetDlgItem(hWnd, IDC_LIST1);
			int iItem = ListBox_AddString(hWNDctrl, szBuffer);
			ListBox_SetCurSel(hWNDctrl, iItem);
		}
		void Dialog_OnClose(HWND hWnd)
		{
			if (hWnd == hDlg) { DestroyWindow(hWnd); }
			else { EndDialog(hWnd, 0); }
		}
#pragma endregion
#pragma region ManyStringAdd
		INT_PTR CALLBACK DialogProcMany(HWND hWndlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
		{
			switch (uMsg)
			{
			case WM_INITDIALOG:
			{
				BOOL bRet = HANDLE_WM_INITDIALOG(hWndlg, wParam, lParam, Dialog_OnInitDialogMany);
				return SetDlgMsgResult(hWndlg, uMsg, bRet);
			}break;
			case WM_COMMAND:
			{
				TCHAR str[] = TEXT("");
				switch (LOWORD(wParam))
				{
				case IDOK:
				{
					int counter = GetDlgItemInt(hWndlg, IDC_EDIT2, NULL, NULL);
					int cch = GetDlgItemText(hWndlg, IDC_EDIT3, szBuffer, _countof(szBuffer));
					while (counter != 0)
					{
						SetDlgItemText(hWndlg, IDC_EDIT3, NULL);
						SendMessage(GetParent(hWndlg), WM_ADDITEM, 0, 0);
						counter--;
					}

				} break;
				case IDCANCEL:
				{
					EndDialog(hWndlg, 0);
					return FALSE;
				}break;
				}
			} break;
			case WM_CLOSE:
			{
				HANDLE_WM_CLOSE(hWndlg, wParam, lParam, DialogMany_OnClose);
			}
			}
			return FALSE;
		}

	BOOL Dialog_OnInitDialogMany(HWND hWnd, HWND hWndF, LPARAM lParam)
	{
		HWND hwndEdit = GetDlgItem(hWnd, IDC_EDIT3);
		SendDlgItemMessage(hwndEdit, IDC_SPIN1, UDM_SETRANGE, 0, 2);
		Edit_LimitText(hwndEdit, _countof(szBuffer) - 1);
		Edit_SetCueBannerText(hwndEdit, L"Новая запись");
		return TRUE;
	}
	void DialogMany_OnClose(HWND hWnd)
	{
		if (hWnd == hDlg) { DestroyWindow(hWnd); }
		else { EndDialog(hWnd, 0); }
	}
#pragma endregion 

	//void OnDestroy(HWND hwnd)
	//{
	//	//int UserAnswer = MessageBox(hWnd, TEXT("Завершить работу?"), TEXT("Выход"), MB_YESNO | MB_ICONQUESTION | MB_DEFBUTTON1);
	//	//if (IDYES == UserAnswer)
	//	//{
	//	//	DestroyWindow(hWnd);
	//	//}
	//	PostQuitMessage(0); // отправляем сообщение WM_QUIT
	//}



