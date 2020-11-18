




#include <Windows.h>
#include <WindowsX.h>
#include <tchar.h>
#include <strsafe.h>
#include <richedit.h> //why ?
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

/*Работа с файлами инициализации*/
void LoadProfile(LPCTSTR lpFileName);//загрузка параметров
void SaveProfile(LPCTSTR lpFileName);//сохранение параметров

/*Асинхронные операции ввод/вывода*/
BOOL OpenFileAsync(HWND hwndCtl); //открытие и чтение
BOOL SaveFileAsync(HWND hwndCtl, BOOL fSaveAs = FALSE);// открытие и запись

/*Асинхронные операции чтения/записи*/
BOOL ReadAsync(HANDLE hFile, LPVOID lpBuffer, DWORD dwOffset, DWORD dwSize, LPOVERLAPPED lpOverlapped);//чтение
BOOL WriteAsync(HANDLE hFile, LPCVOID lpBuffer, DWORD dwOffset, DWORD dwSize, LPOVERLAPPED lpOverlapped);//запись

/*Функции ожидания и проверки ввод/вывода*/

BOOL FinishIo(LPOVERLAPPED lpOverlapped);
BOOL TryFinishIo(LPOVERLAPPED lpOverlapped);

// функция, которая вызывается в цикле обработки сообщений,
// пока в очереди нет сообщений
// для асинхронности
void OnIdle(HWND hwnd);

/*Переменные*/
POINT WindowPosition; // положение окна
SIZE WindowSize; // размер окна

TCHAR FileName[MAX_PATH] = TEXT(""); // имя редактируемого текстового файла
HANDLE hFile = INVALID_HANDLE_VALUE; // дескриптор редактируемого текстового файла

CHARFORMAT cf;//параметры символов
PARAFORMAT pf;//параметры абзацев
LOGFONT logFont; // параметры шрифта
HFONT hFont = NULL; // дескриптор шрифта

LPSTR lpszBufferText = NULL; // указатель на буфер для чтения/записи текстового файла
OVERLAPPED _oRead = { 0 }, _oWrite = { 0 };//why?

int WINAPI _tWinMain(HINSTANCE hInstance, HINSTANCE, LPTSTR lpszCmdLine, int nCmdShow)
{
	
	HINSTANCE relib = LoadLibrary(TEXT("riched32.dll"));    //load the dll don't forget this   
											//and don't forget to free it (see wm_destroy) 
	if (relib == NULL)
		MessageBox(NULL, TEXT("Failed to load riched32.dll!"), TEXT("Error"), MB_ICONEXCLAMATION);
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
	
	LoadLibrary(TEXT("ComCtl32.dll"));//для элементов общего пользования
	// создаем главное окно на основе нового оконного класса
		TCHAR InitFN[MAX_PATH];//имя файла инициализации

	{
		GetModuleFileName(NULL, InitFN, MAX_PATH);

		LPTSTR str = _tcsrchr(InitFN, TEXT('.'));
		if (NULL != str) str[0] = TEXT('\0');

		StringCchCat(InitFN, MAX_PATH, TEXT(".ini"));
	}

	// загружаем параметры приложения из файла инициализации
	LoadProfile(InitFN);
	
	HWND hWnd = CreateWindowEx(0, TEXT("MainWindowClass"), TEXT("Process"), WS_OVERLAPPEDWINDOW,
		WindowPosition.x, WindowPosition.y, WindowSize.cx, WindowSize.cy, NULL, NULL, hInstance, NULL);

	if (NULL == hWnd)
	{
		return -1; // завершаем работу приложения
	}

	ShowWindow(hWnd, nCmdShow); // отображаем главное окно
	

	   	  
	MSG  msg;
	BOOL Ret;

	for (;;)
	{
		// определяем наличие сообщений в очереди
		while (!PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE))
		{
			OnIdle(hWnd);
		} 

		// извлекаем сообщение из очереди
		Ret = GetMessage(&msg, NULL, 0, 0);
		if ( Ret == FALSE )
		{
			break; // получено WM_QUIT, выход из цикла
		} 
		else if (!TranslateAccelerator(hWnd, hAccel, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		} 
	} 
	SaveProfile(InitFN);//сохранение параметров
 
	return (int)msg.wParam;
}

LRESULT CALLBACK MainWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg) 
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
		if (INVALID_HANDLE_VALUE != hFile)
		{
			FinishIo(&_oWrite);	// ожидаем завершения операции ввода/вывода
			CloseHandle(hFile), hFile = INVALID_HANDLE_VALUE;// закрываем дескриптор файла
		} 
	
		if (NULL != hFont)
			DeleteObject(hFont), hFont = NULL;// удаляем созданный шрифт
		PostQuitMessage(0); // отправляем сообщение WM_QUIT
	}break;
	case WM_CLOSE:
		RECT rect;
		GetWindowRect(hwnd, &rect);

		WindowPosition.x = rect.left;
		WindowPosition.y = rect.top;

		WindowSize.cx = rect.right - rect.left;
		WindowSize.cy = rect.bottom - rect.top;

		DestroyWindow(hwnd); // уничтожаем окно
		break;
	} 
	return DefWindowProc(hwnd, uMsg, wParam, lParam);
} 






void OnIdle(HWND hwnd)
{
	if (NULL != lpszBufferText)
	{
		if (TryFinishIo(&_oRead) != FALSE) // асинхронное чтение данных из файла завершено
		{
			if (ERROR_SUCCESS == _oRead.Internal) // чтение завершено успешно
			{
				WORD bom = *(LPWORD)lpszBufferText; // маркер последовательности байтов

				if (0xFEFF == bom) // Unicode-файл
				{
					LPWSTR lpszText = (LPWSTR)(lpszBufferText + sizeof(WORD)); // Unicode-строка

					// вычисляем длину Unicode-строки
					DWORD cch = (_oRead.InternalHigh - sizeof(WORD)) / sizeof(WCHAR);

					// задаём нуль-символ в конце строки
					lpszText[cch] = L'\0';
					// копируем Unicode-строку в поле ввода
					SetDlgItemTextW(hwnd, IDC_EDIT_TEXT, lpszText);
				} // if
				else // ANSI-файл
				{
					// задаём нуль-символ в конце строки
					lpszBufferText[_oRead.InternalHigh] = '\0';
					// копируем ANSI-строку в поле ввода
					SetDlgItemTextA(hwnd, IDC_EDIT_TEXT, lpszBufferText);
				} // else
			} // if

			// освобождаем выделенную память
			delete[] lpszBufferText, lpszBufferText = NULL;
		} // if
		else if (TryFinishIo(&_oWrite) != FALSE) // асинхронная запись данных в файл завершена
		{
			if (ERROR_SUCCESS == _oWrite.Internal) // запись завершена успешно
			{
				// заставим операционную систему записать данные в файл
				// не дожидаясь его закрытия
				FlushFileBuffers(hFile);
			} // if

			// освобождаем выделенную память
			delete[] lpszBufferText, lpszBufferText = NULL;
		} // if
	} // if
} // OnIdle



BOOL OnCreate(HWND hwnd, LPCREATESTRUCT lpCreateStruct)
{
	// создаёи поле ввода для редактирования текста
	DWORD Styles = WS_VISIBLE | WS_CHILD | WS_BORDER | WS_HSCROLL | WS_VSCROLL | ES_NOHIDESEL | ES_AUTOVSCROLL | ES_MULTILINE | ES_SAVESEL| ES_SUNKEN;

	// Создаем орган управления Rich Edit
	HWND hwndCtl = CreateWindowEx(0, TEXT( "RICHEDIT"), TEXT(""), Styles, 0, 0, 100, 100, hwnd, (HMENU)IDC_EDIT_TEXT, lpCreateStruct->hInstance, NULL);
	
	if (hwndCtl == NULL)
		return FALSE;
	
	// Передаем фокус ввода органу управления Rich Edit
	SetFocus(hwndCtl);


	// задаем ограничение на размер текста
	//Edit_LimitText(hwndCtl, (DWORD)-1); //If it will be return RichEdit will be block for edit and entering text

	logFont.lfCharSet = DEFAULT_CHARSET; //значение по умолчанию
	logFont.lfPitchAndFamily = DEFAULT_PITCH; //значения по умолчанию
	wcscpy_s(logFont.lfFaceName, L"Times New Roman"); //копируем в строку название шрифта
	logFont.lfHeight = 20; //высота
	logFont.lfWidth = 10; //ширина
	logFont.lfWeight = 40; //толщина
	logFont.lfEscapement = 0; //шрифт без поворота
	
	// создаём шрифт
	hFont = CreateFontIndirect(&logFont);
	static HWND hEdit = GetDlgItem(hwnd, IDC_EDIT_TEXT);
	if (NULL != hFont)
	{
		// устанавливаем шрифт для поля ввода
		SendMessage(hwndCtl, WM_SETFONT, (WPARAM)hFont, (LPARAM)TRUE);
	} 

	// открываем последний редактируемый текстовый фал
	if (OpenFileAsync(hwndCtl) != FALSE)
	{
		// задаём заголовок главного окна
		SetWindowText(hwnd, FileName);
		SendMessage(hEdit, EM_SETPARAFORMAT, 0, (LPARAM)&pf);//выравнивание текста
	} 
	else
	{
		FileName[0] = _T('\0');// очищаем имя редактируемого текстового файла	
		SetWindowText(hwnd, TEXT("Безымянный"));// задаём заголовок главного окна
	} 
	
	return TRUE;
} 






void OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify)
{
	static HWND hEdit = GetDlgItem(hwnd, IDC_EDIT_TEXT);
	CHOOSEFONT choosef = { sizeof(CHOOSEFONT) };
	HDC hDC;


	switch (id)
	{
	case ID_NEW_FILE: // Создать
	{
		if (INVALID_HANDLE_VALUE != hFile)
		{
			
			FinishIo(&_oWrite);// ожидаем завершения операции ввода/вывода
			CloseHandle(hFile), hFile = INVALID_HANDLE_VALUE;
		} 	
		Edit_SetText(hEdit, NULL);// удаляем текст из поля ввода

		FileName[0] = _T('\0');
		SetWindowText(hwnd, TEXT("Безымянный"));
	}
	break;

	case ID_OPEN: // Открыть
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
			if (OpenFileAsync(hEdit) != FALSE) // открываем файл
			{		
				SetWindowText(hwnd, FileName);// задаём заголовок главного окна
			} 
			else
			{
				MessageBox(NULL, TEXT("Не удалось открыть текстовый файл."), NULL, MB_OK | MB_ICONERROR);
				FileName[0] = _T('\0');
				SetWindowText(hwnd, TEXT("Безымянный"));
			} 
		}
	}
	break;

	case ID_SAVE: // Сохранить
		if (SaveFileAsync(hEdit) != FALSE) // сохраняем файл
		{
			break;
		} 
	case ID_SAVE_AS: // Сохранить как
	{
		OPENFILENAME savefile = { sizeof(OPENFILENAME) };

		savefile.hInstance = GetWindowInstance(hwnd);
		savefile.lpstrFilter = TEXT("Текстовые документы (*.txt)\0*.txt\0");
		savefile.lpstrFile = FileName;
		savefile.nMaxFile = _countof(FileName);
		savefile.lpstrTitle = TEXT("Сохранить как");
		savefile.Flags = OFN_EXPLORER | OFN_ENABLESIZING | OFN_CREATEPROMPT | OFN_OVERWRITEPROMPT;
		savefile.lpstrDefExt = TEXT("txt");

		if (GetSaveFileName(&savefile) != FALSE)
		{
			if (SaveFileAsync(hEdit, TRUE) != FALSE) // пересохраняем файл
			{		
				SetWindowText(hwnd, FileName);// задаём заголовок главного окна
			}
			else
			{
				MessageBox(NULL, TEXT("Не удалось сохранить текстовый файл."), NULL, MB_OK | MB_ICONERROR);
			}
		} 
	}
	break;

	case ID_EXIT:
		SendMessage(hwnd, WM_CLOSE, 0, 0);
		break;

	case ID_UNDO: // Отменить
	{
		
		Edit_Undo(hEdit);// отменяем последне изменения в поле ввода	
		SetFocus(hEdit);// передаём фокус клавиатуы в поле ввода
	}
	break;

	case ID_SELECT_ALL: // Выделить все
	{
		Edit_SetSel(hEdit, 0, -1);// выделяем текст в поле ввода
		SetFocus(hEdit);// передаём фокус клавиатуы в поле ввода
	}
	break;
#pragma region Font

	case ID_BOLD_FONT:
	{
		cf.cbSize = sizeof(cf);

		// Определяем формат символов
		SendMessage(hEdit, EM_GETCHARFORMAT, TRUE, (LPARAM)&cf);

		// Изменяем бит поля dwEffects, с помощью которого
		// можно выделить символы как bold (жирное начертание)
		cf.dwMask = CFM_BOLD;

		// Инвертируем бит, определяющий жирное начертание
		cf.dwEffects ^= CFE_BOLD;

		// Изменяем формат символов
		SendMessage(hEdit, EM_SETCHARFORMAT, SCF_SELECTION, (LPARAM)&cf);
		break;
	}

	// Устанавливаем или отменяем наклонное
	// начертание символов
	case ID_ITALIC_FONT:
	{
		cf.cbSize = sizeof(cf);
		SendMessage(hEdit, EM_GETCHARFORMAT,
			TRUE, (LPARAM)&cf);

		cf.dwMask = CFM_ITALIC;
		cf.dwEffects ^= CFE_ITALIC;
		SendMessage(hEdit, EM_SETCHARFORMAT, SCF_SELECTION, (LPARAM)&cf);
		break;
	}


	case ID_FONT_EDIT: // Шрифт
	{
		// Определяем текущий формат символов
		SendMessage(hEdit, EM_GETCHARFORMAT, TRUE, (LPARAM)&cf);
	
		memset(&choosef, 0, sizeof(choosef));
		memset(&logFont, 0, sizeof(logFont));

		choosef.hInstance = GetWindowInstance(hwnd); // указываем дескриптор экземпляра приложения
		choosef.hwndOwner = hwnd; // указываем дескриптор окна владельца
			   		 
		// Если было задано выделение наклоном или жирным
			// шрифтом,подбираем шрифт с соответствующими атрибутами
		logFont.lfItalic = (BOOL)(cf.dwEffects & CFE_ITALIC);
		logFont.lfUnderline = (BOOL)(cf.dwEffects & CFE_UNDERLINE);

		// Преобразуем высоту из TWIPS-ов в пикселы.
		// Устанавливаем отрицательный знак, чтобы 
		// выполнялось преобразование и использовалось
		// абсолютное значение высоты символов
		logFont.lfHeight = -cf.yHeight / 20;

		// Набор символов, принятый по умолчанию
		logFont.lfCharSet = ANSI_CHARSET;

		// Качество символов, принятое по умолчанию
		logFont.lfQuality = DEFAULT_QUALITY;

		// Выбираем семейство шрифтов
		logFont.lfPitchAndFamily = cf.bPitchAndFamily;

		// Название начертания шрифта
		lstrcpy(logFont.lfFaceName, cf.szFaceName);

		// Устанавливаем вес шрифта в зависимости от того,
		// было использовано выделение жирным шрифтом 
		// или нет
		if (cf.dwEffects & CFE_BOLD)
			logFont.lfWeight = FW_BOLD;
		else
			logFont.lfWeight = FW_NORMAL;
		hDC = GetDC(hwnd);
		// Заполняем структуру для функции выбора шрифта
		choosef.lStructSize = sizeof(choosef);
		choosef.Flags = CF_SCREENFONTS | CF_INITTOLOGFONTSTRUCT;
		choosef.hDC = hDC;
		choosef.hwndOwner = hwnd;
		choosef.lpLogFont = &logFont;
		choosef.rgbColors = RGB(0, 0, 0);
		choosef.nFontType = SCREEN_FONTTYPE;

		// Выводим на экран диалоговую панель для
		// выбора шрифта
		if (ChooseFont(&choosef))
		{
			// Можно использовать все биты поля dwEffects
			cf.dwMask = CFM_BOLD | CFM_FACE | CFM_ITALIC |
				CFM_UNDERLINE | CFM_SIZE | CFM_OFFSET;

			// Преобразование в TWIPS-ы
			cf.yHeight = -logFont.lfHeight * 20;

			// Устанавливаем поле dwEffects 
			cf.dwEffects = 0;
			if (logFont.lfUnderline)
				cf.dwEffects |= CFE_UNDERLINE;

			if (logFont.lfWeight == FW_BOLD)
				cf.dwEffects |= CFE_BOLD;

			if (logFont.lfItalic)
				cf.dwEffects |= CFE_ITALIC;

			// Устанавливаем семейство шрифта
			cf.bPitchAndFamily = logFont.lfPitchAndFamily;

			// Устанавливаем название начертания шрифта
			lstrcpy(cf.szFaceName, logFont.lfFaceName);

			// Изменяем шрифтовое оформление символов
			SendMessage(hEdit, EM_SETCHARFORMAT,
				SCF_SELECTION, (LPARAM)&cf);
		}

		// Освобождаем контекст отображения
		ReleaseDC(hwnd, hDC);

	}
	break;

#pragma endregion	
case IDM_EDCUT:
		SendMessage(hEdit, WM_CUT, 0, 0);
		break;

	case IDM_EDCOPY:
		SendMessage(hEdit, WM_COPY, 0, 0);
		break;

	case IDM_EDPASTE:
		SendMessage(hEdit, WM_PASTE, 0, 0);
		break;

	// Устанавливаем выравнивание параграфа по правой границе
	// окна органа управления Rich Edit
	case ID_FORMAT_PARAGRAPH_RIGHT:
		{
		pf.cbSize = sizeof(pf);
		pf.dwMask = PFM_ALIGNMENT;
		pf.wAlignment = PFA_RIGHT;
		SendMessage(hEdit, EM_SETPARAFORMAT, 0, (LPARAM)&pf);
		break;
		}

	// Выполняем центровку текущего параграфа
	case ID_FORMAT_PARAGRAPH_CENTER:
	{
		pf.cbSize = sizeof(pf);
		pf.dwMask = PFM_ALIGNMENT;
		pf.wAlignment = PFA_CENTER;
		SendMessage(hEdit, EM_SETPARAFORMAT, 0, (LPARAM)&pf);

		break;
	}
	// Устанавливаем выравнивание параграфа по левой границе
// окна органа управления Rich Edit
	case ID_FORMAT_PARAGRAPH_LEFT:
		{
			pf.cbSize = sizeof(pf);
			pf.dwMask = PFM_ALIGNMENT;
			pf.wAlignment = PFA_LEFT;

			// Изменяем тип выравнивания текущего параграфа
			SendMessage(hEdit, EM_SETPARAFORMAT, 0, (LPARAM)&pf);
			break;
		}
	} 

    

} // OnCommand

void LoadProfile(LPCTSTR lpFileName)
{
	// загружаем положение и размер окна

	WindowPosition.x = GetPrivateProfileInt(TEXT("Window"), TEXT("X"), CW_USEDEFAULT, lpFileName);
	WindowPosition.y = GetPrivateProfileInt(TEXT("Window"), TEXT("Y"), 0, lpFileName);

	WindowSize.cx = GetPrivateProfileInt(TEXT("Window"), TEXT("Width"), CW_USEDEFAULT, lpFileName);
	WindowSize.cy = GetPrivateProfileInt(TEXT("Window"), TEXT("Height"), 600, lpFileName);
	
	//загрузка типа выравнивания
	/*Работает*/

	if (GetPrivateProfileInt(TEXT("Paraformat"), TEXT("wAlignment"), 0, lpFileName)==3)
	{
		pf.cbSize = sizeof(pf);
		pf.dwMask = PFM_ALIGNMENT;
		pf.wAlignment = PFA_CENTER;
	}
	if (GetPrivateProfileInt(TEXT("Paraformat"), TEXT("wAlignment"), 0, lpFileName) == 2)
	{
			pf.cbSize = sizeof(pf);
			pf.dwMask = PFM_ALIGNMENT;
			pf.wAlignment = PFA_RIGHT;
	}
	if (GetPrivateProfileInt(TEXT("Paraformat"), TEXT("wAlignment"), 0, lpFileName) == 1)
	{
		pf.cbSize = sizeof(pf);
		pf.dwMask = PFM_ALIGNMENT;
		pf.wAlignment = PFA_LEFT;
	}
		

	/*HOW DO THIS in another way????*/
	
	/*
	pf.dwMask = GetPrivateProfileInt(TEXT("Paraformat"), TEXT("dwMask"), 0, lpFileName);
	pf.wAlignment = GetPrivateProfileInt(TEXT("Paraformat"), TEXT("wAlignment"), 0, lpFileName);
	*/

	/*
	switch ((GetPrivateProfileInt(TEXT("Paraformat"), TEXT("wAlignment"), 0, lpFileName)))
	{
		pf.cbSize = sizeof(pf);
		pf.dwMask = PFM_ALIGNMENT;
		case 1:
			pf.wAlignment = PFA_LEFT;
			break;
		case 2:
			pf.wAlignment = PFA_RIGHT;
			break;
		case 3:
			pf.wAlignment = PFA_CENTER;
			break;
	}
	*/
	// загружаем имя последнего редактируемого текстового файла

	GetPrivateProfileString(TEXT("File"), TEXT("Filename"), NULL, FileName, MAX_PATH, lpFileName);


} 

void SaveProfile(LPCTSTR lpFileName)
{
	TCHAR szString[10];

	// сохраняем положение и размер окна

	StringCchPrintf(szString, 10, TEXT("%d"), WindowPosition.x);
	WritePrivateProfileString(TEXT("Window"), TEXT("X"), szString, lpFileName);

	StringCchPrintf(szString, 10, TEXT("%d"), WindowPosition.y);
	WritePrivateProfileString(TEXT("Window"), TEXT("Y"), szString, lpFileName);

	StringCchPrintf(szString, 10, TEXT("%d"), WindowSize.cx);
	WritePrivateProfileString(TEXT("Window"), TEXT("Width"), szString, lpFileName);

	StringCchPrintf(szString, 10, TEXT("%d"), WindowSize.cy);
	WritePrivateProfileString(TEXT("Window"), TEXT("Height"), szString, lpFileName);
	
	StringCchPrintf(szString, 10, TEXT("%d"), pf.dwMask);
	WritePrivateProfileString(TEXT("Paraformat"), TEXT("dwMask"), szString, lpFileName);
	
	StringCchPrintf(szString, 10, TEXT("%d"), pf.wAlignment);
	WritePrivateProfileString(TEXT("Paraformat"), TEXT("wAlignment"), szString, lpFileName);


	// сохраняем имя последнего редактируемого текстового файла

	WritePrivateProfileString(TEXT("File"), TEXT("Filename"), FileName, lpFileName);

	

} 

BOOL OpenFileAsync(HWND hwndCtl)
{
	// открываем существующий файл для чтения и записи
	HANDLE hExistingFile = CreateFile(FileName, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_FLAG_OVERLAPPED, NULL);

	if (INVALID_HANDLE_VALUE == hExistingFile) // не удалось открыть файл
	{
		return FALSE;
	} // if

	// удаляем текст из поля ввода
	Edit_SetText(hwndCtl, NULL);

	if (INVALID_HANDLE_VALUE != hFile)
	{
		// ожидаем завершения операции ввода/вывода
		FinishIo(&_oWrite);
		// закрываем дескриптор файла
		CloseHandle(hFile);
	} // if

	hFile = hExistingFile;

	// определяем размер файла 
	LARGE_INTEGER size;
	BOOL bRet = GetFileSizeEx(hFile, &size);

	if ((FALSE != bRet) && (size.LowPart > 0))
	{
		// выделяем память для буфера, в который будет считываться данные из файла
		lpszBufferText = new CHAR[size.LowPart + 2];

		// начинаем асинхронное чтение данных из файла
		bRet = ReadAsync(hFile, lpszBufferText, 0, size.LowPart, &_oRead);

		if (FALSE == bRet) // возникла ошибка
		{
			// освобождаем выделенную память
			delete[] lpszBufferText, lpszBufferText = NULL;
		} // if
	} // if

	return bRet;
} // OpenFileAsync

BOOL SaveFileAsync(HWND hwndCtl, BOOL fSaveAs)
{
	if (FALSE != fSaveAs)
	{
		// создаём и открываем файл для чтения и записи
		HANDLE hNewFile = CreateFile(FileName, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_FLAG_OVERLAPPED, NULL);

		if (INVALID_HANDLE_VALUE == hNewFile) // не удалось открыть файл
		{
			return FALSE;
		} // if

		if (INVALID_HANDLE_VALUE != hFile)
		{
			// ожидаем завершения операции ввода/вывода
			FinishIo(&_oWrite);
			// закрываем дескриптор файла
			CloseHandle(hFile);
		} // if

		hFile = hNewFile;
	} // if
	else if (INVALID_HANDLE_VALUE != hFile)
	{
		// ожидаем завершения операции ввода/вывода
		FinishIo(&_oWrite);
	} // if
	else
	{
		// создаём и открываем файл для чтения и записи
		hFile = CreateFile(FileName, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_FLAG_OVERLAPPED, NULL);

		if (INVALID_HANDLE_VALUE == hFile) // не удалось открыть файл
		{
			return FALSE;
		} // if
	} // else

	// определяем размер текста
	LARGE_INTEGER size;
	size.QuadPart = GetWindowTextLengthA(hwndCtl);

	// изменяем положение указателя файла
	BOOL bRet = SetFilePointerEx(hFile, size, NULL, FILE_BEGIN);
	// устанавливаем конец файла
	if (FALSE != bRet)
		bRet = SetEndOfFile(hFile);

	if ((FALSE != bRet) && (size.LowPart > 0))
	{
		// выделяем память для буфера, из которого будут записываться данные в файл
		lpszBufferText = new CHAR[size.LowPart + 1];
		// копируем ANSI-строку из поля ввода в буффер
		GetWindowTextA(hwndCtl, lpszBufferText, size.LowPart + 1);

		// начинаем асинхронную запись данных в файл
		bRet = WriteAsync(hFile, lpszBufferText, 0, size.LowPart, &_oWrite);

		if (FALSE == bRet) // возникла ошибка
		{
			// освобождаем выделенную память
			delete[] lpszBufferText, lpszBufferText = NULL;
		} // if
	} // if

	return bRet;
} // SaveFileAsync

/*Asynch work*/
// ----------------------------------------------------------------------------------------------
BOOL ReadAsync(HANDLE hFile, LPVOID lpBuffer, DWORD dwOffset, DWORD dwSize, LPOVERLAPPED lpOverlapped)
{
	// инициализируем структуру OVERLAPPED ...

	ZeroMemory(lpOverlapped, sizeof(OVERLAPPED));

	lpOverlapped->Offset = dwOffset;
	lpOverlapped->hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);

	// начинаем асинхронную операцию чтения данных из файла
	BOOL bRet = ReadFile(hFile, lpBuffer, dwSize, NULL, lpOverlapped);

	if (FALSE == bRet && ERROR_IO_PENDING != GetLastError())
	{
		CloseHandle(lpOverlapped->hEvent), lpOverlapped->hEvent = NULL;
		return FALSE;
	} // if

	return TRUE;
} // ReadAsync

// ----------------------------------------------------------------------------------------------
BOOL WriteAsync(HANDLE hFile, LPCVOID lpBuffer, DWORD dwOffset, DWORD dwSize, LPOVERLAPPED lpOverlapped)
{
	// инициализируем структуру OVERLAPPED ...

	ZeroMemory(lpOverlapped, sizeof(OVERLAPPED));

	lpOverlapped->Offset = dwOffset;
	lpOverlapped->hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);

	// начинаем асинхронную операцию записи данных в файл
	BOOL bRet = WriteFile(hFile, lpBuffer, dwSize, NULL, lpOverlapped);

	if (FALSE == bRet && ERROR_IO_PENDING != GetLastError())
	{
		CloseHandle(lpOverlapped->hEvent), lpOverlapped->hEvent = NULL;
		return FALSE;
	} // if

	return TRUE;
} // WriteAsync

// ----------------------------------------------------------------------------------------------
BOOL FinishIo(LPOVERLAPPED lpOverlapped)
{
	if (NULL != lpOverlapped->hEvent)
	{
		// ожидаем завершения операции ввода/вывода
		DWORD dwResult = WaitForSingleObject(lpOverlapped->hEvent, INFINITE);

		if (WAIT_OBJECT_0 == dwResult) // операция завершена
		{
			CloseHandle(lpOverlapped->hEvent), lpOverlapped->hEvent = NULL;
			return TRUE;
		} // if
	} // if

	return FALSE;
} // FinishIo

// ----------------------------------------------------------------------------------------------
BOOL TryFinishIo(LPOVERLAPPED lpOverlapped)
{
	if (NULL != lpOverlapped->hEvent)
	{
		// определяем состояние операции ввода/вывода
		DWORD dwResult = WaitForSingleObject(lpOverlapped->hEvent, 0);

		if (WAIT_OBJECT_0 == dwResult) // операция завершена
		{
			CloseHandle(lpOverlapped->hEvent), lpOverlapped->hEvent = NULL;
			return TRUE;
		} // if
	} // if

	return FALSE;
} // TryFinishIo

