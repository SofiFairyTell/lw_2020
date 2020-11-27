#include "FileEditorHeader.h"
#include <tchar.h>
#define IDC_EDIT_TEXT        2001


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
			FinishIo(&ovlWrite);	// ожидаем завершения операции ввода/вывода
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
	if (NULL != lpBuffReWri)
	{
		if (TryFinishIo(&ovlRead) != FALSE) // асинхронное чтение данных из файла завершено
		{
			if (ERROR_SUCCESS == ovlRead.Internal) // чтение завершено успешно
			{
				WORD bom = *(LPWORD)lpBuffReWri; // маркер последовательности байтов
				if ((0xFEFF == bom)||(0xBB ==bom) ||(0xBF ==bom)) // Unicode-файл
				//if(	IsTextUnicode(lpBuffReWri,sizeof(lpBuffReWri),NULL))
				{
					LPWSTR lpszText = (LPWSTR)(lpBuffReWri + sizeof(WORD)); // Unicode-строка
					// вычисляем длину Unicode-строки
					DWORD cch = (ovlRead.InternalHigh - sizeof(WORD)) / sizeof(WCHAR);				
					lpszText[cch] = L'\0';// задаём нуль-символ в конце строки			
					SetDlgItemTextW(hwnd, IDC_EDIT_TEXT, lpszText);// копируем Unicode-строку в поле ввода
				} 
				else // ANSI-файл
				{			
					lpBuffReWri[ovlRead.InternalHigh] = '\0';// задаём нуль-символ в конце строки
					SetDlgItemTextA(hwnd, IDC_EDIT_TEXT, lpBuffReWri);// копируем ANSI-строку в поле ввода
				} 
			} 
			delete[] lpBuffReWri, lpBuffReWri = NULL;	// освобождаем выделенную память
		} 
		else if (TryFinishIo(&ovlWrite) != FALSE) // асинхронная запись данных в файл завершена
		{
			if (ERROR_SUCCESS == ovlWrite.Internal) // запись завершена успешно
			{
				// заставим операционную систему записать данные в файл не дожидаясь его закрытия
				FlushFileBuffers(hFile);
			} 			
			delete[] lpBuffReWri, lpBuffReWri = NULL;// освобождаем выделенную память
		} 
	}
} 



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
			
			FinishIo(&ovlWrite);// ожидаем завершения операции ввода/вывода
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
	
	/*Dont init without this items*/
	pf.cbSize = sizeof(pf);
	pf.dwMask = PFM_ALIGNMENT;

	//загрузка типа выравнивания
	if (GetPrivateProfileInt(TEXT("Paraformat"), TEXT("wAlignment"), 0, lpFileName)==3)
	{
		pf.wAlignment = PFA_CENTER;
	}
	if (GetPrivateProfileInt(TEXT("Paraformat"), TEXT("wAlignment"), 0, lpFileName) == 2)
	{
			pf.wAlignment = PFA_RIGHT;
	}
	if (GetPrivateProfileInt(TEXT("Paraformat"), TEXT("wAlignment"), 0, lpFileName) == 1)
	{
		pf.wAlignment = PFA_LEFT;
	}
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
	
	//сохраняем параметры выравнивания текста
	/*	warning! now this line describe all text in program. 
		so, if you have first string with PFA_LEFT and second string with PFA_CENTER 
		saveprofile write wAlignment = 3 (it is PFA_CENTER) in .ini file 
	*/
	StringCchPrintf(szString, 10, TEXT("%d"), pf.dwMask);
	WritePrivateProfileString(TEXT("Paraformat"), TEXT("dwMask"), szString, lpFileName);
	
	StringCchPrintf(szString, 10, TEXT("%d"), pf.wAlignment);
	WritePrivateProfileString(TEXT("Paraformat"), TEXT("wAlignment"), szString, lpFileName);

	// сохраняем имя последнего редактируемого текстового файла

	WritePrivateProfileString(TEXT("File"), TEXT("Filename"), FileName, lpFileName);

	

} 

BOOL OpenFileAsync(HWND hwndCtl)
{
	HANDLE hExistingFile = CreateFile(FileName, 
		GENERIC_READ | GENERIC_WRITE,//запись и чтние
		FILE_SHARE_READ, //для совместного чтения
		NULL, //защиты нет
		OPEN_EXISTING, //открыть существующий
		FILE_FLAG_OVERLAPPED,//асинхронный доступ к файлу
		NULL); //шаблона нет

	if (INVALID_HANDLE_VALUE == hExistingFile) // не удалось открыть файл
	{
		CloseHandle(hExistingFile);
		return FALSE;
	} 

	Edit_SetText(hwndCtl, NULL);// удаляем текст из поля ввода

	if (INVALID_HANDLE_VALUE != hFile)
	{
		FinishIo(&ovlWrite);	// ожидаем завершения операции ввода/вывода
		CloseHandle(hFile);
	} 

	hFile = hExistingFile;
	
	LARGE_INTEGER size;// определяем размер файла 
	BOOL bRet = GetFileSizeEx(hFile, &size);

	if ((FALSE != bRet) && (size.LowPart > 0))
	{
		// выделяем память для буфера, в который будет считываться данные из файла
		lpBuffReWri = new CHAR[size.LowPart + 2];
		
		bRet = ReadAsync(hFile, lpBuffReWri, 0, size.LowPart, &ovlRead);// асинхронное чтение данных из файла

		if (FALSE == bRet) // возникла ошибка
		{		
			delete[] lpBuffReWri, lpBuffReWri = NULL;// освобождаем выделенную память
		} 
	} 
	return bRet;
} 

BOOL SaveFileAsync(HWND hwndCtl, BOOL fSaveAs)
{
	if (fSaveAs != FALSE)
	{
		// создаём и открываем файл для чтения и записи
			HANDLE hNewFile = CreateFileW(FileName,
			GENERIC_READ | GENERIC_WRITE,//запись и чтние
			FILE_SHARE_READ, //для совместного чтения
			NULL, //защиты нет
			CREATE_ALWAYS, //создание нового файла
			FILE_FLAG_OVERLAPPED,//асинхронный доступ к файлу
			NULL); //шаблона нет


		if (hNewFile == INVALID_HANDLE_VALUE) // не удалось открыть файл
		{
			CloseHandle(hNewFile);
			return FALSE;
		}

		if (hFile != INVALID_HANDLE_VALUE)
		{		
			FinishIo(&ovlWrite);// ожидаем завершения операции ввода/вывода
			CloseHandle(hFile);
		} 

		hFile = hNewFile;
	} 
	else if (hFile != INVALID_HANDLE_VALUE)
	{	
		FinishIo(&ovlWrite);// ожидаем завершения операции ввода/вывода
	}
	else
	{
		// создаём и открываем файл для чтения и записи
		hFile = CreateFile(FileName, 
			GENERIC_READ | GENERIC_WRITE, 
			FILE_SHARE_READ, 
			NULL, 
			CREATE_ALWAYS, 
			FILE_FLAG_OVERLAPPED, NULL);

		if (hFile == INVALID_HANDLE_VALUE) // не удалось открыть файл
		{
			CloseHandle(hFile);
			return FALSE;
		} 
	} 
	
	LARGE_INTEGER size;// определяем размер текста
	size.QuadPart = GetWindowTextLengthW(hwndCtl);

	SetFilePointerEx(hFile, size, NULL, FILE_BEGIN);// изменяем положение указателя файла
	const WORD BOM = 0XFEFF;
	//BOOL bRet = WriteFile(hFile, &BOM, sizeof(BOM), &dwSize, ovl);
	WriteFile(hFile, "0XFEFF", 3, NULL, NULL);
	BOOL bRet = SetFilePointerEx(hFile, size, NULL, FILE_END);
	if (FALSE != bRet)
		bRet = SetEndOfFile(hFile);// устанавливаем конец файла

	if ((FALSE != bRet) && (size.LowPart > 0))
	{

		lpBuffReWri = new CHAR[size.LowPart + 1];	// выделяем память для буфера, из которого будут записываться данные в файл
		
		/*С этим конвертером строка читается как Unicode*/
		/*Сохранение идет в Unicode, но при чтении все ломается*/
		/*А и еще обрезается текст*/
		USES_CONVERSION;
		LPWSTR x = A2W(lpBuffReWri);

		GetWindowTextA(hwndCtl, lpBuffReWri, size.LowPart + 1);		// копируем ANSI-строку из поля ввода в буффер
		//GetWindowTextW(hwndCtl, x, size.LowPart + 1);		// копируем ANSI-строку из поля ввода в буффер

		bRet = WriteAsync(hFile, lpBuffReWri, 0, size.LowPart, &ovlWrite);// асинхронная запись данных в файл
	//	bRet = WriteAsync(hFile, x, 0, size.LowPart, &ovlWrite);// асинхронная запись данных в файл

		if (FALSE == bRet) 
		{
			delete[] lpBuffReWri, lpBuffReWri = NULL;// освобождаем выделенную память
		} 
	} 

	return bRet;
} 

/*Asynch work*/

BOOL ReadAsync(HANDLE hFile, LPVOID lpBuffer, DWORD dwOffset, DWORD dwSize, LPOVERLAPPED ovl)
{
	// инициализируем структуру OVERLAPPED 
	ZeroMemory(ovl, sizeof(ovl));

	ovl->Offset = dwOffset; // младшая часть смещения
	ovl->hEvent = CreateEvent(NULL, FALSE, FALSE, NULL); //событие для оповещения завершения записи

	// начинаем асинхронную операцию чтения данных из файла
	BOOL bRet = ReadFile(hFile, lpBuffer, dwSize, NULL, ovl);
	DWORD  dwRet = GetLastError();
	if (FALSE == bRet && ERROR_IO_PENDING != dwRet)
	{
		CloseHandle(ovl->hEvent), ovl->hEvent = NULL;
		return FALSE;
	}

	return TRUE;
} 

BOOL WriteAsync(HANDLE hFile, LPCVOID lpBuffer, DWORD dwOffset, DWORD dwSize, LPOVERLAPPED ovl)
{
	// инициализируем структуру OVERLAPPED
	ZeroMemory(ovl, sizeof(ovl));
	ovl->Offset = dwOffset; // младшая часть смещения
	ovl->hEvent = CreateEvent(NULL, FALSE, FALSE, NULL); //событие для оповещения завершения записи
	// начинаем асинхронную операцию записи данных в файл
	//TCHAR tcFF[] = TEXT("\xFF");
	//const WORD BOM = 0XFEFF;
	//BOOL bRet = WriteFile(hFile, &BOM, sizeof(BOM), &dwSize, ovl);
	//WriteFile(hFile, &BOM, sizeof(BOM), NULL, NULL);

	BOOL bRet = WriteFile(hFile, lpBuffer, dwSize, NULL, ovl);
	//BOOL bRet = WriteFile(hFile, &tcFF,1, dwSize, NULL, ovl);

	DWORD  dwRet = GetLastError();
	if (FALSE == bRet && ERROR_IO_PENDING != dwRet)
	{
		CloseHandle(ovl->hEvent), ovl->hEvent = NULL;
		return FALSE;
	} 
	return TRUE;

	

} 

BOOL FinishIo(LPOVERLAPPED ovl)
{
	if (NULL != ovl->hEvent)
	{
		// ожидаем завершения операции ввода/вывода
		DWORD dwRes = WaitForSingleObject(ovl->hEvent, INFINITE);

		if (WAIT_OBJECT_0 == dwRes) // операция завершена
		{
			CloseHandle(ovl->hEvent), ovl->hEvent = NULL;
			return TRUE;
		} 
	} 

	return FALSE;
} 


BOOL TryFinishIo(LPOVERLAPPED lpOverlapped)
{
	if (NULL != lpOverlapped->hEvent)
	{
		
		DWORD dwResult = WaitForSingleObject(lpOverlapped->hEvent, 0);// определяем состояние операции ввода/вывода

		if (WAIT_OBJECT_0 == dwResult) // операция завершена
		{
			CloseHandle(lpOverlapped->hEvent), lpOverlapped->hEvent = NULL;
			return TRUE;
		} 
	}

	return FALSE;
} 

