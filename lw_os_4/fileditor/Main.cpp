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
void OnDestroy(HWND hwnd);
void OnSize(HWND hwnd, UINT state, int cx, int cy);
void OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify);
void OnClose(HWND hwnd);

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
POINT wndPos; // положение окна
SIZE wndSize; // размер окна

TCHAR FileName[MAX_PATH] = TEXT(""); // имя редактируемого текстового файла
HANDLE hFile = INVALID_HANDLE_VALUE; // дескриптор редактируемого текстового файла

LOGFONT logFont; // параметры шрифта
HFONT hFont = NULL; // дескриптор шрифта

LPSTR lpszBufferText = NULL; // указатель на буфер для чтения/записи текстового файла
OVERLAPPED _oRead = { 0 }, _oWrite = { 0 };

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
	
	TCHAR szIniFileName[MAX_PATH];

	{
		GetModuleFileName(NULL, szIniFileName, MAX_PATH);

		LPTSTR str = _tcsrchr(szIniFileName, TEXT('.'));
		if (NULL != str) str[0] = TEXT('\0');

		StringCchCat(szIniFileName, MAX_PATH, TEXT(".ini"));
	}

	// загружаем параметры приложения из файла инициализации
	LoadProfile(szIniFileName);
	   	  
	MSG  msg;
	BOOL bRet;

	for (;;)
	{
		// определяем наличие сообщений в очереди
		while (!PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE))
		{
			OnIdle(hWnd);
		} 

		// извлекаем сообщение из очереди
		bRet = GetMessage(&msg, NULL, 0, 0);

		if (bRet == -1)
		{
			/* обработка ошибки и возможно выход из цикла */
		} // if
		else if (FALSE == bRet)
		{
			break; // получено WM_QUIT, выход из цикла
		} 
		else if (!TranslateAccelerator(hWnd, hAccel, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		} 
	} 

	// сохраняем параметры приложения в файл инициализации
	SaveProfile(szIniFileName);
 
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
			// изменяем размеры поля ввода
			MoveWindow(hwndCtl, 0, 0, LOWORD(lParam), HIWORD(lParam), TRUE);
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

		wndPos.x = rect.left;
		wndPos.y = rect.top;

		wndSize.cx = rect.right - rect.left;
		wndSize.cy = rect.bottom - rect.top;

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
	DWORD dwStyle = WS_CHILD | WS_VISIBLE | WS_VSCROLL | WS_HSCROLL |WS_BORDER| ES_MULTILINE | ES_AUTOHSCROLL | ES_AUTOVSCROLL | ES_LEFT| ES_WANTRETURN;
	HWND hwndCtl = CreateWindowEx(0, TEXT("Edit"), TEXT(""), dwStyle, 0, 0, 0, 0, hwnd, (HMENU)IDC_EDIT_TEXT, lpCreateStruct->hInstance, NULL);
	
	// задаем ограничение на размер текста
	Edit_LimitText(hwndCtl, (DWORD)-1);

	// создаём шрифт
	hFont = CreateFontIndirect(&logFont);

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
	} 
	else
	{
		// очищаем имя редактируемого текстового файла
		FileName[0] = _T('\0');
		// задаём заголовок главного окна
		SetWindowText(hwnd, TEXT("Безымянный"));
	} 

	return TRUE;
} 






void OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify)
{
	static HWND hEdit = GetDlgItem(hwnd, IDC_EDIT_TEXT);

	switch (id)
	{
	case ID_NEW_FILE: // Создать
	{
		if (INVALID_HANDLE_VALUE != hFile)
		{
			// ожидаем завершения операции ввода/вывода
			FinishIo(&_oWrite);
			// закрываем дескриптор файла
			CloseHandle(hFile), hFile = INVALID_HANDLE_VALUE;
		} // if

		// удаляем текст из поля ввода
		Edit_SetText(hEdit, NULL);

		// очищаем имя редактируемого текстового файла
		FileName[0] = _T('\0');
		// задаём заголовок главного окна
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
				// задаём заголовок главного окна
				SetWindowText(hwnd, FileName);
			} // if
			else
			{
				MessageBox(NULL, TEXT("Не удалось открыть текстовый файл."), NULL, MB_OK | MB_ICONERROR);

				// очищаем имя редактируемого текстового файла
				FileName[0] = _T('\0');
				// задаём заголовок главного окна
				SetWindowText(hwnd, TEXT("Безымянный"));
			} // else
		} // if
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
		// отменяем последне изменения в поле ввода
		Edit_Undo(hEdit);
		// передаём фокус клавиатуы в поле ввода
		SetFocus(hEdit);
	}
	break;

	case ID_SELECT_ALL: // Выделить все
	{
		Edit_SetSel(hEdit, 0, -1);// выделяем текст в поле ввода
		// передаём фокус клавиатуы в поле ввода
		SetFocus(hEdit);
	}
	break;

	case ID_FONT_EDIT: // Шрифт
	{
		CHOOSEFONT choosef = { sizeof(CHOOSEFONT) };

		choosef.hInstance = GetWindowInstance(hwnd); // указываем дескриптор экземпляра приложения
		choosef.hwndOwner = hwnd; // указываем дескриптор окна владельца

		LOGFONT lf; //для определения логического шрифта
		ZeroMemory(&lf, sizeof(lf));

		choosef.lpLogFont = &lf; // указываем структуру, которая будет использоваться для создания шрифта

		if (ChooseFont(&choosef) != FALSE)
		{
			// создаём новый шрифт
			HFONT hNewFont = CreateFontIndirect(choosef.lpLogFont);

			if (NULL != hNewFont)
			{
				// удаляем созданный ранее шрифт
				if (NULL != hFont) DeleteObject(hFont);
				// устанавливаем шрифт для поля ввода
				hFont = hNewFont;
				SendDlgItemMessage(hwnd, IDC_EDIT_TEXT, WM_SETFONT, (WPARAM)hFont, (LPARAM)TRUE);

				// копируем параметры шрифта
				CopyMemory(&logFont, choosef.lpLogFont, sizeof(LOGFONT));
			} 
		} 
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
	} 
} // OnCommand

void LoadProfile(LPCTSTR lpFileName)
{
	// загружаем положение и размер окна

	wndPos.x = GetPrivateProfileInt(TEXT("Window"), TEXT("X"), CW_USEDEFAULT, lpFileName);
	wndPos.y = GetPrivateProfileInt(TEXT("Window"), TEXT("Y"), 0, lpFileName);

	wndSize.cx = GetPrivateProfileInt(TEXT("Window"), TEXT("Width"), CW_USEDEFAULT, lpFileName);
	wndSize.cy = GetPrivateProfileInt(TEXT("Window"), TEXT("Height"), 600, lpFileName);

	// загружаем имя последнего редактируемого текстового файла

	GetPrivateProfileString(TEXT("File"), TEXT("Filename"), NULL, FileName, MAX_PATH, lpFileName);

	// загружаем параметры шрифта

	logFont.lfHeight = (LONG)GetPrivateProfileInt(TEXT("Font"), TEXT("lfHeight"), 0, lpFileName);
	logFont.lfWidth = (LONG)GetPrivateProfileInt(TEXT("Font"), TEXT("lfWidth"), 0, lpFileName);
	logFont.lfEscapement = (LONG)GetPrivateProfileInt(TEXT("Font"), TEXT("lfEscapement"), 0, lpFileName);
	logFont.lfOrientation = (LONG)GetPrivateProfileInt(TEXT("Font"), TEXT("lfOrientation"), 0, lpFileName);
	logFont.lfWeight = (LONG)GetPrivateProfileInt(TEXT("Font"), TEXT("lfWeight"), 0, lpFileName);
	logFont.lfItalic = (BYTE)GetPrivateProfileInt(TEXT("Font"), TEXT("lfItalic"), 0, lpFileName);
	logFont.lfUnderline = (BYTE)GetPrivateProfileInt(TEXT("Font"), TEXT("lfUnderline"), 0, lpFileName);
	logFont.lfStrikeOut = (BYTE)GetPrivateProfileInt(TEXT("Font"), TEXT("lfStrikeOut"), 0, lpFileName);
	logFont.lfCharSet = (BYTE)GetPrivateProfileInt(TEXT("Font"), TEXT("lfCharSet"), 0, lpFileName);
	logFont.lfOutPrecision = (BYTE)GetPrivateProfileInt(TEXT("Font"), TEXT("lfOutPrecision"), 0, lpFileName);
	logFont.lfClipPrecision = (BYTE)GetPrivateProfileInt(TEXT("Font"), TEXT("lfClipPrecision"), 0, lpFileName);
	logFont.lfQuality = (BYTE)GetPrivateProfileInt(TEXT("Font"), TEXT("lfQuality"), 0, lpFileName);
	logFont.lfPitchAndFamily = (BYTE)GetPrivateProfileInt(TEXT("Font"), TEXT("lfPitchAndFamily"), 0, lpFileName);

	GetPrivateProfileString(TEXT("Font"), TEXT("lfFaceName"), NULL, logFont.lfFaceName, LF_FACESIZE, lpFileName);
} // LoadProfile

void SaveProfile(LPCTSTR lpFileName)
{
	TCHAR szString[10];

	// сохраняем положение и размер окна

	StringCchPrintf(szString, 10, TEXT("%d"), wndPos.x);
	WritePrivateProfileString(TEXT("Window"), TEXT("X"), szString, lpFileName);

	StringCchPrintf(szString, 10, TEXT("%d"), wndPos.y);
	WritePrivateProfileString(TEXT("Window"), TEXT("Y"), szString, lpFileName);

	StringCchPrintf(szString, 10, TEXT("%d"), wndSize.cx);
	WritePrivateProfileString(TEXT("Window"), TEXT("Width"), szString, lpFileName);

	StringCchPrintf(szString, 10, TEXT("%d"), wndSize.cy);
	WritePrivateProfileString(TEXT("Window"), TEXT("Height"), szString, lpFileName);

	// сохраняем имя последнего редактируемого текстового файла

	WritePrivateProfileString(TEXT("File"), TEXT("Filename"), FileName, lpFileName);

	// сохраняем параметры шрифта

	StringCchPrintf(szString, 10, TEXT("%d"), logFont.lfHeight);
	WritePrivateProfileString(TEXT("Font"), TEXT("lfHeight"), szString, lpFileName);

	StringCchPrintf(szString, 10, TEXT("%d"), logFont.lfWidth);
	WritePrivateProfileString(TEXT("Font"), TEXT("lfWidth"), szString, lpFileName);

	StringCchPrintf(szString, 10, TEXT("%d"), logFont.lfEscapement);
	WritePrivateProfileString(TEXT("Font"), TEXT("lfEscapement"), szString, lpFileName);

	StringCchPrintf(szString, 10, TEXT("%d"), logFont.lfOrientation);
	WritePrivateProfileString(TEXT("Font"), TEXT("lfOrientation"), szString, lpFileName);

	StringCchPrintf(szString, 10, TEXT("%d"), logFont.lfWeight);
	WritePrivateProfileString(TEXT("Font"), TEXT("lfWeight"), szString, lpFileName);

	StringCchPrintf(szString, 10, TEXT("%d"), logFont.lfItalic);
	WritePrivateProfileString(TEXT("Font"), TEXT("lfItalic"), szString, lpFileName);

	StringCchPrintf(szString, 10, TEXT("%d"), logFont.lfUnderline);
	WritePrivateProfileString(TEXT("Font"), TEXT("lfUnderline"), szString, lpFileName);

	StringCchPrintf(szString, 10, TEXT("%d"), logFont.lfStrikeOut);
	WritePrivateProfileString(TEXT("Font"), TEXT("lfStrikeOut"), szString, lpFileName);

	StringCchPrintf(szString, 10, TEXT("%d"), logFont.lfCharSet);
	WritePrivateProfileString(TEXT("Font"), TEXT("lfCharSet"), szString, lpFileName);

	StringCchPrintf(szString, 10, TEXT("%d"), logFont.lfOutPrecision);
	WritePrivateProfileString(TEXT("Font"), TEXT("lfOutPrecision"), szString, lpFileName);

	StringCchPrintf(szString, 10, TEXT("%d"), logFont.lfClipPrecision);
	WritePrivateProfileString(TEXT("Font"), TEXT("lfClipPrecision"), szString, lpFileName);

	StringCchPrintf(szString, 10, TEXT("%d"), logFont.lfQuality);
	WritePrivateProfileString(TEXT("Font"), TEXT("lfQuality"), szString, lpFileName);

	StringCchPrintf(szString, 10, TEXT("%d"), logFont.lfPitchAndFamily);
	WritePrivateProfileString(TEXT("Font"), TEXT("lfPitchAndFamily"), szString, lpFileName);

	WritePrivateProfileString(TEXT("Font"), TEXT("lfFaceName"), logFont.lfFaceName, lpFileName);
} // SaveProfile

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

