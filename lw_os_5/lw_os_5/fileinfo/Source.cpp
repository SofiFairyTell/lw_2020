
#include "FileInfoFuncHeader.h"

int WINAPI _tWinMain(HINSTANCE hInstance, HINSTANCE, LPTSTR lpszCmdLine, int nCmdShow)
{

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



	//DWORD dwDisposition;

	//// создаём и открываем ключ реестра для сохранения параметров приложения
	//RegCreateKeyEx(HKEY_CURRENT_USER, TEXT("Software\\IT-311"),
	//	0, NULL, REG_OPTION_NON_VOLATILE, KEY_QUERY_VALUE | KEY_SET_VALUE, NULL, &hKey, &dwDisposition);
	//
	//// копируем имя файла/каталога из системного реестра
	//RegGetValueSZ(hKey, TEXT("Path"), FileName, _countof(FileName), NULL);
	//// копируем положение окна из системного реестра
	//RegGetValueBinary(hKey, TEXT("rect"), (LPBYTE)&rect, sizeof(rect), NULL);


	LoadLibrary(TEXT("ComCtl32.dll"));//для элементов общего пользования		


	if (0 == RegisterClassEx(&wcex)) // регистрируем класс
	{
		return -1; // завершаем работу приложения
	}
	RECT wr = { 0, 0, 500, 500 };    // set the size, but not the position

	// создаем главное окно на основе нового оконного класса
	HWND hWnd = CreateWindowEx(0, TEXT("MainWindowClass"), TEXT("Process"), WS_OVERLAPPEDWINDOW^WS_THICKFRAME^WS_MINIMIZEBOX^WS_MAXIMIZEBOX, 300, 300,
		wr.right - wr.left, wr.bottom - wr.top, NULL, NULL, hInstance, NULL);
	if (IsRectEmpty(&rect) == FALSE)
	{
		// изменяем положение окна
		SetWindowPos(hWnd, NULL, rect.left, rect.top, 0, 0, SWP_NOSIZE | SWP_SHOWWINDOW);
	} // if


	if (NULL == hWnd)
	{
		return -1; // завершаем работу приложения
	}

	ShowWindow(hWnd, nCmdShow); // отображаем главное окно

	MSG  msg;
	BOOL Ret;

	for (;;)
	{

		// извлекаем сообщение из очереди
		Ret = GetMessage(&msg, NULL, 0, 0);
		if (Ret == FALSE)
		{
			break; // получено WM_QUIT, выход из цикла
		}
		else if (!TranslateAccelerator(hWnd, hAccel, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}


	return (int)msg.wParam;
}

LRESULT CALLBACK MainWindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
		HANDLE_MSG(hwnd, WM_CREATE, OnCreate);
		HANDLE_MSG(hwnd, WM_COMMAND, OnCommand);
	case WM_SIZE:
	{
		HWND hwndCtl = GetDlgItem(hwnd,	IDC_EDIT_TEXT);
		MoveWindow(hwndCtl, 0, 0, LOWORD(lParam), HIWORD(lParam), TRUE); // изменяем размеры поля ввода
	}

	break;
	case WM_DESTROY:
	{
		PostQuitMessage(0); // отправляем сообщение WM_QUIT
	}break;
	case WM_CLOSE:

		DestroyWindow(hwnd); // уничтожаем окно
		break;
	}
	return DefWindowProc(hwnd, msg, wParam, lParam);
}



BOOL OnCreate(HWND hwnd, LPCREATESTRUCT lpCRStr)
{
	CreateWindowEx(0, TEXT("Edit"), NULL, WS_CHILD | WS_VISIBLE | WS_BORDER, 30, 10, 400, 20, hwnd, (HMENU)IDC_EDIT_FILENAME, lpCRStr->hInstance, NULL);

	HWND hwndLV = CreateWindowEx(0, TEXT("SysListView32"), NULL, WS_CHILD | WS_VISIBLE | WS_BORDER | LVS_REPORT | LVS_SHOWSELALWAYS, 30, 50, 400, 150, hwnd, (HMENU)IDC_LIST1, lpCRStr->hInstance, NULL);

	//значения атрибутов
	/*
	CreateWindowEx(0, TEXT("button"), TEXT("Только для чтения"), WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX, 30, 200, 400, 30, hwnd, (HMENU)IDC_ATTRIBUTE_READONLY, lpCRStr->hInstance, NULL);
	CreateWindowEx(0, TEXT("button"), TEXT("Скрытый"), WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX, 30, 230, 400, 30, hwnd, (HMENU)IDC_ATTRIBUTE_HIDDEN, lpCRStr->hInstance, NULL);
	CreateWindowEx(0, TEXT("button"), TEXT("Файл готов для архивирирования"), WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX, 30, 260, 400, 30, hwnd, (HMENU)IDC_ATTRIBUTE_ARCHIVE, lpCRStr->hInstance, NULL);
	CreateWindowEx(0, TEXT("button"), TEXT("Системный"), WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX, 30, 290, 400, 30, hwnd, (HMENU)IDC_ATTRIBUTE_SYSTEM, lpCRStr->hInstance, NULL);
	CreateWindowEx(0, TEXT("button"), TEXT("Временный"), WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX, 30, 320, 400, 30, hwnd, (HMENU)IDC_ATTRIBUTE_TEMPORARY, lpCRStr->hInstance, NULL);
	CreateWindowEx(0, TEXT("button"), TEXT("Сжимать для экономии места"), WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX, 30, 350, 400, 30, hwnd, (HMENU)IDC_ATTRIBUTE_COMPRESSED, lpCRStr->hInstance, NULL);
	CreateWindowEx(0,TEXT("button"), TEXT("Шифровать содержимое для защиты"), WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX,30, 380, 400, 30, hwnd,(HMENU)IDC_ATTRIBUTE_ENCRYPTED,lpCRStr->hInstance, NULL);
	*/
	// задем расширенный 
	ListView_SetExtendedListViewStyle(hwndLV, LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

	// вставляем три столбца в список просмотра

	LVCOLUMN lvColumns[] = {
		{ LVCF_WIDTH | LVCF_TEXT, 0, 100, (LPTSTR)TEXT("Тип") },
		{ LVCF_WIDTH | LVCF_TEXT, 0, 100, (LPTSTR)TEXT("Имя") },
		{ LVCF_WIDTH | LVCF_TEXT, 0, 100, (LPTSTR)TEXT("Применять к:") },

	};

	for (int i = 0; i < _countof(lvColumns); ++i)
	{
		// вставляем столбец
		ListView_InsertColumn(hwndLV, i, &lvColumns[i]);
	}
	
	CreateWindowEx(0, TEXT("Static"), TEXT("Владелец:"), WS_CHILD | WS_VISIBLE | SS_SIMPLE,
		30, 220, 80, 20, hwnd, NULL, lpCRStr->hInstance, NULL);
	CreateWindowEx(0, TEXT("Edit"), NULL, WS_CHILD | WS_VISIBLE | WS_BORDER, 110, 220, 200, 20, hwnd, (HMENU)IDC_EDIT_OWNER, lpCRStr->hInstance, NULL);

	//if (FileName != NULL) //если путь не пустой, то заполнить список
	//{
	//	ListViewInit(FileName, hwnd);
	//}

	return TRUE;
}

void OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify)
{
	static HWND hEdit = GetDlgItem(hwnd, IDC_EDIT_TEXT);

	switch (id)
	{
	case ID_OPEN_FILE: // Открыть
	{
		BROWSEINFO bi;//structure for open special box with folder in treview
		LPITEMIDLIST pidl;
		ZeroMemory(&bi, sizeof(bi));
		bi.hwndOwner = NULL;
		bi.pszDisplayName = FileName;
		bi.lpszTitle = TEXT("Select file");
		bi.ulFlags = BIF_BROWSEINCLUDEFILES; 
		pidl = SHBrowseForFolder(&bi);//open window for select
		if (pidl)
		{
			SHGetPathFromIDList(pidl, FileName);//get path
			//SetDlgItemText(hwnd, IDC_EDIT_FILENAME, FileName);
			if (!(ListViewInit(FileName, hwnd)))
			{
				GetLastError();
				break;
			}
		}
	}
	break;
	case ID_OPEN_DIR://Открыть папку
	{
		BROWSEINFO bi;//structure for open special box with folder in treview
		LPITEMIDLIST            pidl;
		LPMALLOC  pMalloc = NULL;

		ZeroMemory(&bi, sizeof(bi));
		bi.hwndOwner = NULL;
		bi.pszDisplayName = FileName;
		bi.lpszTitle = TEXT("Select folder");
		bi.ulFlags = BIF_RETURNONLYFSDIRS;

		pidl = SHBrowseForFolder(&bi);//open window for select
		if (pidl)
		{
			SHGetPathFromIDList(pidl, FileName);//get path

			if (!(ListViewInit(FileName, hwnd)))
			{
				GetLastError();
				break;
			}
		}
	} break;
	case ID_CHANGE_ATR://Изменение атрибутов
	{
		/*
		TCHAR NewFileName[MAX_PATH]; // новое имя файла/каталога
		GetDlgItemText(hwnd, IDC_EDIT_FILENAME, NewFileName, _countof(NewFileName));//это имя и его к указателю lpszFileName

			// найдём имя в пути к файлу/каталогу
		lpszFileName = PathFindFileName(FileName);

		// вычисляем длину пути к файлу/каталогу
		cchPath = (DWORD)(lpszFileName - FileName) - 1;
		// разделяем нуль-символом путь и имя файла/каталога
		FileName[cchPath] = _T('\0');

		if (CompareString(LOCALE_USER_DEFAULT, 0, lpszFileName, -1, NewFileName, -1) != CSTR_EQUAL) // (!) изменилось имя файла/каталога
		{
			TCHAR ExistingFileName[MAX_PATH]; // старое имя файла/каталога
			StringCchPrintf(ExistingFileName, _countof(ExistingFileName), TEXT("%s\\%s"), FileName, lpszFileName);

			// формируем новый путь к файлу/каталогу
			PathAppend(FileName, NewFileName);
			// переименовываем файл/каталог
			MoveFile(ExistingFileName, FileName);
		} // if
		else
		{
			// заменим нуль-символ, разделяющий путь и имя файла/каталога
			FileName[cchPath] = _T('\\');
		} // else

		// массив атрибутов
		constexpr DWORD attr[] = {
			FILE_ATTRIBUTE_READONLY, FILE_ATTRIBUTE_HIDDEN, FILE_ATTRIBUTE_ARCHIVE,
			FILE_ATTRIBUTE_SYSTEM, FILE_ATTRIBUTE_TEMPORARY, FILE_ATTRIBUTE_COMPRESSED, FILE_ATTRIBUTE_ENCRYPTED
		};

		// массив идентификаторов флажков для атрибутов
		constexpr DWORD ids[] = {
			IDC_ATTRIBUTE_READONLY, IDC_ATTRIBUTE_HIDDEN, IDC_ATTRIBUTE_ARCHIVE,
			IDC_ATTRIBUTE_SYSTEM, IDC_ATTRIBUTE_TEMPORARY, IDC_ATTRIBUTE_COMPRESSED, IDC_ATTRIBUTE_ENCRYPTED
		};

		DWORD dwFileAttributes = 0; // атрибуты файла/каталога

		// определяем, какие атрибуты выбраны

		for (int i = 0; i < _countof(attr); ++i)
		{
			if (IsDlgButtonChecked(hwnd, ids[i]) == BST_CHECKED) // флажок установлен
			{
				dwFileAttributes |= attr[i]; // добавим соответствующий атрибут
			} // if
		} // for

		// зададим атрибуты
		SetFileAttributes(FileName, dwFileAttributes);

		ListViewInit(FileName, hwnd);*/
	}

	case ID_RENAME://переименование без сохранения атрибутов
	{
		/*
		TCHAR NewFileName[MAX_PATH]; // новое имя файла/каталога
		GetDlgItemText(hwnd, IDC_EDIT_FILENAME, NewFileName, _countof(NewFileName));//это имя и его к указателю lpszFileName

			// найдём имя в пути к файлу/каталогу
		lpszFileName = PathFindFileName(FileName);

		// вычисляем длину пути к файлу/каталогу
		cchPath = (DWORD)(lpszFileName - FileName) - 1;
		// разделяем нуль-символом путь и имя файла/каталога
		FileName[cchPath] = _T('\0');

		if (CompareString(LOCALE_USER_DEFAULT, 0, lpszFileName, -1, NewFileName, -1) != CSTR_EQUAL) // (!) изменилось имя файла/каталога
		{
			TCHAR ExistingFileName[MAX_PATH]; // старое имя файла/каталога
			StringCchPrintf(ExistingFileName, _countof(ExistingFileName), TEXT("%s\\%s"), FileName, lpszFileName);

			// формируем новый путь к файлу/каталогу
			PathAppend(FileName, NewFileName);
			// переименовываем файл/каталог
			MoveFile(ExistingFileName, FileName);
		}
		else
		{
			FileName[cchPath] = _T('\\');// заменим нуль-символ, разделяющий путь и имя файла/каталога
		}
		// запомним размер и положение окна
		GetWindowRect(hwnd, &rect);

		ListViewInit(FileName, hwnd);*/
	}
	break;

	case ID_EXIT:
		SendMessage(hwnd, WM_CLOSE, 0, 0);
		RegCloseKey(hKey);
		break;

	}
}

BOOL ListViewInit(LPTSTR path, HWND hwnd)
{
	// массив возможных значений поля grfInheritance
	constexpr DWORD dwInherit[7] = {
		NO_INHERITANCE,
		SUB_CONTAINERS_AND_OBJECTS_INHERIT,
		SUB_CONTAINERS_ONLY_INHERIT,
		SUB_OBJECTS_ONLY_INHERIT,
		INHERIT_ONLY | SUB_CONTAINERS_AND_OBJECTS_INHERIT,
		INHERIT_ONLY | SUB_CONTAINERS_ONLY_INHERIT,
		INHERIT_ONLY | SUB_OBJECTS_ONLY_INHERIT
	};

	// массив строк, содержащие описание для возможных значений поля grfInheritance
	constexpr LPCTSTR szInheritText[7] = {
		TEXT("Только для этого каталога"),
		TEXT("Для этого каталога, его подкаталогов и файлов"),
		TEXT("Для этого каталога и его подкаталогов"),
		TEXT("Для этого каталога и его файлов"),
		TEXT("Только для подкаталогов и файлов"),
		TEXT("Только для подкаталогов"),
		TEXT("Только для файлов")
	};

	WIN32_FILE_ATTRIBUTE_DATA bhfi;
	TCHAR TimeBuffer[100], Buffer[100];
	if (!GetFileAttributesEx(path, GetFileExInfoStandard, &bhfi))
	{
		GetLastError();
	}
	/*Добавление имени*/
	LPTSTR lpFN = PathFindFileNameW(path);
	SetDlgItemText(hwnd, IDC_EDIT_FILENAME, lpFN);

	/*Инициализация списка*/
	HWND hwndLV = GetDlgItem(hwnd, IDC_LIST1);
	ListView_DeleteAllItems(hwndLV); //очистка списка просмотра

	// освобождаем выделенную память
	if (NULL != Sec_Descriptor) LocalFree(Sec_Descriptor), Sec_Descriptor = NULL;
	// получим дескриптор безопасности
	BOOL RetRes = GetFileSecurityDescriptor(FileName, OWNER_SECURITY_INFORMATION | DACL_SECURITY_INFORMATION, &Sec_Descriptor);

	if (RetRes != FALSE)
	{
		LPWSTR Owner = NULL; 
		RetRes = GetOwnerName_W(Sec_Descriptor, &Owner);
		SetDlgItemText(hwnd, IDC_EDIT_OWNER, Owner);// копируем имя учетной записи владельца в поле "Текущий владелец"
		// освобождаем выделенную память
		LocalFree(Owner);
		
	}

	ULONG uCount = 0; // количество элементов в массиве ACE
	PEXPLICIT_ACCESS pEA = NULL; // массив ACE

	RetRes = GetItemFromDACL(Sec_Descriptor, &uCount, &pEA);

	if (RetRes != FALSE)
	{
		for (ULONG i = 0; i < uCount; ++i)
		{
			LVITEM lvItem = { LVIF_TEXT | LVIF_PARAM };

			lvItem.iItem = (int)i;

			// определим права доступа
			lvItem.lParam = (LPARAM)pEA[i].grfAccessPermissions;

			// определим тип ACE
			switch (pEA[i].grfAccessMode)
			{
			case GRANT_ACCESS:
				lvItem.pszText = (LPTSTR)TEXT("Разрешить");
				break;
			case DENY_ACCESS:
				lvItem.pszText = (LPTSTR)TEXT("Запретить");
				break;
			} // switch

			// добавляем новый элемент в список просмотра DACL
			int iItem = ListView_InsertItem(hwndLV, &lvItem);
			if (iItem == -1) continue; // не удалось добавить новый элемент

			 // определим имя учетной записи
			if (TRUSTEE_IS_SID == pEA[i].Trustee.TrusteeForm)
			{
				LPTSTR lpszName = NULL; // имя учетной записи

				// получим имя учетной записи
				GetAccountName_W(pEA[i].Trustee.ptstrName, &lpszName);

				if (NULL != lpszName)
				{
					// копируем имя учетной записи в ячейку списка просмотра DACL
					ListView_SetItemText(hwndLV, iItem, 1, lpszName);
					// освобождаем выделенную память
					LocalFree(lpszName);
				} // if
			} // if
			// определим к каким объектам применяются права доступа
			DWORD grfInheritance = pEA[i].grfInheritance & (~INHERIT_NO_PROPAGATE);

			for (int j = 0; j < _countof(dwInherit); ++j)
			{
				if (grfInheritance == dwInherit[j]) // найдено значение
				{
					// копируем в ячейку списка просмотра DACL описание для значения поля grfInheritance
					ListView_SetItemText(hwndLV, iItem, 2, (LPTSTR)szInheritText[j]);

					break; // выходим из цикла
				} // if
			} // for
		}
	}

	//получение информации о размере файла
	//get info about size of file
	//LARGE_INTEGER LI_Size;
	//ULARGE_INTEGER sizeDir = { 0 };
	//hFile = CreateFile(
	//	FileName,   // имя файла
	//	GENERIC_READ,          // чтение из файла
	//	FILE_SHARE_READ,       // совместный доступ к файлу
	//	NULL,                  // защиты нет
	//	OPEN_EXISTING,         // открываем существующий файл
	//	FILE_ATTRIBUTE_NORMAL,  // асинхронный ввод
	//	NULL                   // шаблона нет
	//);
	//if (!GetFileSizeEx(hFile, &LI_Size))
	//{
	//	//обработка ошибки
	//}
	//if (bhfi.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
	//{
	//	//расчет для папки 
	//	CalculateSize(path, &bhfi, &sizeDir);
	//	ConvertDirectSize(Buffer, _countof(Buffer), sizeDir);
	//}
	//else
	//{

	//	ConvertFileSize(Buffer, _countof(Buffer), LI_Size);
	//	
	//}

	/*Определим имя текущего владельца*/






	/*

	LPTSTR lpFN = PathFindFileNameW(path);
	SetDlgItemText(hwnd, IDC_EDIT_FILENAME, lpFN);

	//Добавление найденных атрибутов в список просмотра
	HWND hwndLV = GetDlgItem(hwnd, IDC_LIST1);
	// добавляем новый элемент в список просмотра
	ListView_DeleteAllItems(hwndLV);
	LVITEM lvItem = { LVIF_TEXT | LVIF_PARAM };
	lvItem.iItem = ListView_GetItemCount(hwndLV);
	//Размер
	lvItem.pszText = (LPWSTR)(L"ТЕКУЩИЙ ВЛАДЕЛЕЦ:");
	lvItem.iItem = ListView_InsertItem(hwndLV, &lvItem);
	if ((lvItem.iItem != -1))
	{
		ListView_SetItemText(hwndLV, lvItem.iItem, 1, Buffer);
	}
	//третий параметр
	lvItem.pszText = (LPWSTR)(L"Время изменения:");
	lvItem.iItem = ListView_InsertItem(hwndLV, &lvItem);
	if ((lvItem.iItem != -1))
	{
		GetFileTimeFormat(&bhfi.ftCreationTime, TimeBuffer, _countof(TimeBuffer));//время создания
		ListView_SetItemText(hwndLV, lvItem.iItem, 1, TimeBuffer);
	}
	//второй параметр
	lvItem.pszText = (LPWSTR)(L"Время последнего обращения:");
	lvItem.iItem = ListView_InsertItem(hwndLV, &lvItem);
	if ((lvItem.iItem != -1))
	{
		GetFileTimeFormat(&bhfi.ftLastAccessTime, TimeBuffer, _countof(TimeBuffer));//время последнего обращения
		ListView_SetItemText(hwndLV, lvItem.iItem, 1, TimeBuffer);
	}
	//первый параметр
	lvItem.pszText = (LPWSTR)(L"Время создания:");
	lvItem.iItem = ListView_InsertItem(hwndLV, &lvItem);
	if ((lvItem.iItem != -1))
	{
		GetFileTimeFormat(&bhfi.ftLastWriteTime, TimeBuffer, _countof(TimeBuffer));//время изменения
		ListView_SetItemText(hwndLV, lvItem.iItem, 1, TimeBuffer);
	}
	//Расположение
	lvItem.pszText = (LPWSTR)(L"Расположение:");
	lvItem.iItem = ListView_InsertItem(hwndLV, &lvItem);
	if ((lvItem.iItem != -1))
	{
		ListView_SetItemText(hwndLV, lvItem.iItem, 1, path);
	}
	//Тип
	lvItem.pszText = (LPWSTR)(L"Тип:");
	lvItem.iItem = ListView_InsertItem(hwndLV, &lvItem);
	if ((lvItem.iItem != -1))
	{
		if (!(bhfi.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY))
		{
			ListView_SetItemText(hwndLV, lvItem.iItem, 1, (LPWSTR)(L"Файл"));
		}
		else
			if (bhfi.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
					ListView_SetItemText(hwndLV, lvItem.iItem, 1, (LPWSTR)(L"Папка с файлами"));
			}

	}

	// массив атрибутов
	constexpr DWORD attr[] = {
		FILE_ATTRIBUTE_READONLY, FILE_ATTRIBUTE_HIDDEN, FILE_ATTRIBUTE_ARCHIVE,
		FILE_ATTRIBUTE_SYSTEM, FILE_ATTRIBUTE_TEMPORARY, FILE_ATTRIBUTE_COMPRESSED, FILE_ATTRIBUTE_ENCRYPTED
	};
	// массив идентификаторов флажков для атрибутов
	constexpr DWORD ids[] = {
		IDC_ATTRIBUTE_READONLY, IDC_ATTRIBUTE_HIDDEN, IDC_ATTRIBUTE_ARCHIVE,
		IDC_ATTRIBUTE_SYSTEM, IDC_ATTRIBUTE_TEMPORARY, IDC_ATTRIBUTE_COMPRESSED, IDC_ATTRIBUTE_ENCRYPTED
	};

	// раставим флажки соответственно с установленными атрибутами файла/каталога

	for (int i = 0; i < _countof(attr); ++i)
	{
		UINT uCheck = (bhfi.dwFileAttributes & attr[i]) ? BST_CHECKED : BST_UNCHECKED;
		CheckDlgButton(hwnd, ids[i], uCheck);
	}

	// закрываем дескриптор файла */
	CloseHandle(hFile);
	return TRUE;
}

BOOL __stdcall CalculateSize(LPCTSTR lpszFileName, const LPWIN32_FILE_ATTRIBUTE_DATA lpFileAttributeData, LPVOID lpvParam)
{
	if (lpFileAttributeData->dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
	{
		// продолжим поиск внутри каталога
		return FileSearch(TEXT("*"), lpszFileName, CalculateSize, lpvParam);
	} // if
	return TRUE; // возвращаем TRUE, чтобы продолжить поиск
}

BOOL FileSearch(LPCTSTR lpszFileName, LPCTSTR path, LPSEARCHFUNC lpSearchFunc, LPVOID lpvParam)
{
	WIN32_FIND_DATA ffd;
	LARGE_INTEGER filesize;
	LARGE_INTEGER size;
	TCHAR szDir[MAX_PATH];
	size_t length_of_arg;
	HANDLE hFind = INVALID_HANDLE_VALUE;
	StringCchCopy(szDir, MAX_PATH, path);
	StringCchCat(szDir, MAX_PATH, TEXT("\\*"));
	hFind = FindFirstFile(szDir, &ffd);

	if (INVALID_HANDLE_VALUE == hFind)
	{
		//error. terminator . later.
	}
	do
	{
		if (!(ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
		{
			filesize.LowPart = ffd.nFileSizeLow;
			filesize.HighPart = ffd.nFileSizeHigh;
			((ULARGE_INTEGER *)lpvParam)->QuadPart += filesize.QuadPart;
		}
	} while (FindNextFile(hFind, &ffd) != 0);

	FindClose(hFind);
	return TRUE;
}

BOOL GetFileTimeFormat(const LPFILETIME lpFileTime, LPTSTR lpszFileTime, DWORD cchFileTime)
{
	SYSTEMTIME st;

	// преобразуем дату и время из FILETIME в SYSTEMTIME
	BOOL bRet = FileTimeToSystemTime(lpFileTime, &st);

	// приведем дату и время к текущему часовому поясу
	if (FALSE != bRet)
		bRet = SystemTimeToTzSpecificLocalTime(NULL, &st, &st);

	if (FALSE != bRet)
	{
		// скопируем дату в результирующую строку
		GetDateFormat(LOCALE_USER_DEFAULT, DATE_LONGDATE, &st, NULL, lpszFileTime, cchFileTime);

		// добавим время в результирующую строку

		StringCchCat(lpszFileTime, cchFileTime, TEXT(", "));
		DWORD len = _tcslen(lpszFileTime);

		if (len < cchFileTime)
			GetTimeFormat(LOCALE_USER_DEFAULT, TIME_FORCE24HOURFORMAT, &st, NULL, lpszFileTime + len, cchFileTime - len);
	} // if

	return bRet;
}
void ConvertFileSize(LPTSTR lpszBuffer, DWORD cch, LARGE_INTEGER size)
{
	if (size.QuadPart >= 0x40000000ULL)
	{
		StringCchPrintf(lpszBuffer, cch, TEXT("%.1f ГБ"), (size.QuadPart / (float)0x40000000ULL));
	} // if
	else if (size.QuadPart >= 0x100000ULL)
	{
		StringCchPrintf(lpszBuffer, cch, TEXT("%.1f МБ"), (size.QuadPart / (float)0x100000ULL));
	} // if
	else if (size.QuadPart >= 0x0400ULL)
	{
		StringCchPrintf(lpszBuffer, cch, TEXT("%.1f КБ"), (size.QuadPart / (float)0x0400ULL));
	} // if
	else
	{
		StringCchPrintf(lpszBuffer, cch, TEXT("%u байт"), size.LowPart);
	} // else

	size_t len = _tcslen(lpszBuffer);

	if (len < cch)
	{
		StringCchPrintf((lpszBuffer + len), (cch - len), TEXT(" (%llu байт)"), size.QuadPart);
	} // if
} // StringCchPrintFileSize

void ConvertDirectSize(LPTSTR lpszBuffer, DWORD cch, ULARGE_INTEGER size)
{
	if (size.QuadPart >= 0x40000000ULL)
	{
		StringCchPrintf(lpszBuffer, cch, TEXT("%.1f ГБ"), (size.QuadPart / (float)0x40000000ULL));
	} // if
	else if (size.QuadPart >= 0x100000ULL)
	{
		StringCchPrintf(lpszBuffer, cch, TEXT("%.1f МБ"), (size.QuadPart / (float)0x100000ULL));
	} // if
	else if (size.QuadPart >= 0x0400ULL)
	{
		StringCchPrintf(lpszBuffer, cch, TEXT("%.1f КБ"), (size.QuadPart / (float)0x0400ULL));
	} // if
	else
	{
		StringCchPrintf(lpszBuffer, cch, TEXT("%u байт"), size.LowPart);
	} // else

	size_t len = _tcslen(lpszBuffer);

	if (len < cch)
	{
		StringCchPrintf((lpszBuffer + len), (cch - len), TEXT(" (%llu байт)"), size.QuadPart);
	} // if
} // StringCchPrintFileSize


//work wit regist
LSTATUS RegGetValueSZ(HKEY hKey, LPCTSTR lpValueName, LPTSTR lpszData, DWORD cch, LPDWORD lpcchNeeded)
{
	// определяем тип получаемого значения параметра
	DWORD RegType;
	LSTATUS retCode = RegQueryValueEx(hKey, lpValueName, NULL, &RegType, NULL, NULL);
	if (ERROR_SUCCESS == retCode && REG_SZ == RegType)
	{
		DWORD DataBuffer = cch * sizeof(TCHAR);
		// получаем значение параметра
		retCode = RegQueryValueEx(hKey, lpValueName, NULL, NULL, (LPBYTE)lpszData, &DataBuffer);
	}
	else if (ERROR_SUCCESS == retCode)
	{
		retCode = ERROR_UNSUPPORTED_TYPE; // неверный тип данных
	}

	return retCode;
}

LSTATUS RegGetValueBinary(HKEY hKey, LPCTSTR lpValueName, LPBYTE lpData, DWORD cb, LPDWORD lpcbNeeded)
{
	DWORD dwType;
	// определяем тип получаемого значения параметра
	LSTATUS lStatus = RegQueryValueEx(hKey, lpValueName, NULL, &dwType, NULL, NULL);

	if (ERROR_SUCCESS == lStatus && REG_BINARY == dwType)
	{
		// получаем значение параметра
		lStatus = RegQueryValueEx(hKey, lpValueName, NULL, NULL, lpData, &cb);

		if (NULL != lpcbNeeded) *lpcbNeeded = cb;
	} // if
	else if (ERROR_SUCCESS == lStatus)
	{
		lStatus = ERROR_UNSUPPORTED_TYPE; // неверный тип данных
	} // if

	return lStatus;
} // RegGetValueBinary

/*Дескрипторы*/
BOOL GetFileSecurityDescriptor(LPCWSTR lpFileName, SECURITY_INFORMATION RequestedInformation, PSECURITY_DESCRIPTOR *ppSD)
{
	DWORD cb = 0;

	// определим размер дескриптора безопасности
	GetFileSecurity(lpFileName, RequestedInformation, NULL, 0, &cb);

	// выделим память для дескриптора безопасности
	PSECURITY_DESCRIPTOR pSD = (PSECURITY_DESCRIPTOR)LocalAlloc(LMEM_FIXED, cb);
	if (NULL == pSD) return FALSE;

	// получим дескриптор безопасности
	BOOL bRet = GetFileSecurity(lpFileName, RequestedInformation, pSD, cb, &cb);

	if (FALSE != bRet)
	{
		*ppSD = pSD; // возвращаем полученный дескриптор безопасности
	} // if
	else
	{
		//LocalFree(pSD); // освобождаем выделенную память
	} // else

	return bRet;
} // GetFileSecurityDescriptor

BOOL GetItemFromDACL(PSECURITY_DESCRIPTOR Sec_Descriptor, PULONG pcCountOfEntries, PEXPLICIT_ACCESS *pListOfEntries)
{
	PACL pDacl = NULL;
	BOOL bDaclPresent = FALSE, bDaclDefaulted = FALSE;

	// получаем DACL
	BOOL bRet = GetSecurityDescriptorDacl(pSD, &bDaclPresent, &pDacl, &bDaclDefaulted);

	if (FALSE != bRet && FALSE != bDaclPresent)
	{
		// извлекаем элементы из DACL
		DWORD dwResult = GetExplicitEntriesFromAcl(pDacl, pcCountOfEntries, pListOfEntries);
		bRet = (ERROR_SUCCESS == dwResult) ? TRUE : FALSE;
	} // if
	else
	{
		*pcCountOfEntries = 0; // возвращаем 0 элементов
	} // else

	return bRet;
}

/*Получение имени аккаунта по его SID*/
BOOL GetAccountName_W(PSID psid, LPWSTR* AccountName)
{
	BOOL RetRes = FALSE;
	SID_NAME_USE SidType;//переменная перечисляемого типа, сюда сохраним определенный тип SID
	/*Переменные для вывода*/
	LPWSTR Name = NULL;
	DWORD cch = 0, cchRefDomainName = 0;

	if (IsValidSid(psid) == FALSE)
	{
		return FALSE;
	}

	LookupAccountSid(NULL, psid, NULL, &cch, NULL, &cchRefDomainName, NULL);//определим размеры буферов

	DWORD cb = (cch + cchRefDomainName) * sizeof(TCHAR);

	if (cb > 0)
	{

		Name = (LPWSTR)LocalAlloc(LMEM_FIXED, cb);//выделение памяти из локальной кучи процесса
	}

	if (Name != NULL)
	{
		RetRes = LookupAccountSid(NULL, psid, Name + cchRefDomainName, &cch, Name, &cchRefDomainName, &SidType);
	}

	if (RetRes != FALSE)
	{
		if (SidTypeDomain != SidType)
		{
			if (cchRefDomainName > 0)
			{
				Name[cchRefDomainName] = '\\';
			}
			else
			{
				StringCbCopy(Name, cb, Name + 1);//копирование для вовзрата в основую функциию
			}
		}
		*AccountName = Name; //вернем полученнное имя в программу
	}
	else
	{
		ConvertSidToStringSid(psid, AccountName);//если не получилось получить имя, то вернем SID
		if (Name != NULL)
		{
			LocalFree(Name);
		}
	}
	return RetRes;
}

/*Получение имени пользователя по дескриптору безопасности*/
BOOL GetOwnerName_W(PSECURITY_DESCRIPTOR Sec_Descriptor, LPWSTR *OwnerName)
{
	PSID psid;
	BOOL bDefaulted;

	// получаем SID владельца
	BOOL bRet = GetSecurityDescriptorOwner(Sec_Descriptor, &psid, &bDefaulted);

	if (FALSE != bRet)
	{
		// определяем имя учетной записи владельца
		bRet = GetAccountName_W(psid, lpName);
	} // if

	return bRet;
}