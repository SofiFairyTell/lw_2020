
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

// ----------------------------------------------------------------------------------------------
INT_PTR CALLBACK DialogAceProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_INITDIALOG:
	{
		BOOL bRet = HANDLE_WM_INITDIALOG(hwndDlg, wParam, lParam, DialogAce_OnInitDialog);
		return SetDlgMsgResult(hwndDlg, uMsg, bRet);
	}

	case WM_CLOSE:
		EndDialog(hwndDlg, IDCLOSE);
		return TRUE;

	case WM_COMMAND:
		HANDLE_WM_COMMAND(hwndDlg, wParam, lParam, DialogAce_OnCommand);
		return TRUE;
	} // switch

	return FALSE;
} // DialogAceProc

BOOL OnCreate(HWND hwnd, LPCREATESTRUCT lpCRStr)
{
	CreateWindowEx(0, TEXT("Edit"), NULL, WS_CHILD | WS_VISIBLE | WS_BORDER, 30, 10, 400, 20, hwnd, (HMENU)IDC_EDIT_FILENAME, lpCRStr->hInstance, NULL);

	HWND hwndLV = CreateWindowEx(0, TEXT("SysListView32"), NULL, WS_CHILD | WS_VISIBLE| WS_BORDER | LVS_REPORT | LVS_SHOWSELALWAYS, 30, 50, 400, 150, hwnd, (HMENU)IDC_LIST1, lpCRStr->hInstance, NULL);

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
	CreateWindowEx(0, TEXT("Edit"), NULL, WS_CHILD | WS_VISIBLE | WS_BORDER, 110, 220, 310, 20, hwnd, (HMENU)IDC_EDIT_OWNER, lpCRStr->hInstance, NULL);

	CreateWindowEx(0, TEXT("Static"), TEXT("Изменить на:"), WS_CHILD | WS_VISIBLE | SS_SIMPLE,
		30, 250, 80, 20, hwnd, NULL, lpCRStr->hInstance, NULL);
	CreateWindowEx(0, TEXT("Edit"), NULL, WS_CHILD | WS_VISIBLE | WS_BORDER, 110, 250, 310, 20, hwnd, (HMENU)IDC_NEW_OWNER, lpCRStr->hInstance, NULL);


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
		
	}

	case ID_CHANGE_OWNER://переименование без сохранения атрибутов
	{
		/*//Скроет?
		HWND hHide = GetDlgItem(hwnd, IDC_LIST1);
		ShowWindow(hHide, SW_HIDE);
		/*да!*/

		LPWSTR Owner = NULL;
		//RetRes = GetOwnerName_W(Sec_Descriptor, &Owner);//полуение имени владельца
		GetDlgItemText(hwnd, IDC_EDIT_OWNER, Owner, sizeof(Owner));// копируем имя учетной записи владельца в поле "Текущий владелец"

		TCHAR NewOwner[UNLEN + 1]; // новое имя владельца

		GetDlgItemText(hwnd, IDC_NEW_OWNER, NewOwner, _countof(NewOwner));//это имя и его к указателю lpszFileName
		/*новый код*/
		PSID psid;
		BOOL bret = GetAccountSID_W(NewOwner, &psid);
		if (bret != FALSE)
		{			
			if (SetFileSecurityInfo(FileName, NewOwner, 0, NULL, FALSE) == TRUE)
			{
				SetDlgItemText(hwnd, IDC_NEW_OWNER, NULL);
			}
			else
			{
				DWORD dwRetCode = GetLastError();

				if (dwRetCode == ERROR_INVALID_OWNER)
				{
					MessageBox(hwnd, TEXT("Этот пользователь не может быть владельцем файла или каталога"), NULL, MB_OK | MB_ICONERROR);
					//Тогда добавим этого пользователя в список DALC. Он будет без прав делать что либо
					EXPLICIT_ACCESS ea;
					ea.grfAccessPermissions = 0;
					ea.grfAccessMode = GRANT_ACCESS;
					ea.grfInheritance = NO_INHERITANCE;
					ea.Trustee.TrusteeForm = TRUSTEE_IS_SID;
					ea.Trustee.ptstrName = (LPWSTR)psid;
					if (SetFileSecurityInfo(FileName, NULL, 1, &ea, TRUE) == TRUE)
					{
						SetDlgItemText(hwnd, IDC_NEW_OWNER, NULL);
					}
					else
					{
						MessageBox(hwnd, TEXT("Не удалось сменить владельца. Проверьте правильность введенного имени пользователя."), NULL, MB_OK | MB_ICONERROR);
					}
				}
				else
				{
					MessageBox(hwnd, TEXT("Не удалось сменить владельца. Проверьте правильность введенного имени пользователя."), NULL, MB_OK | MB_ICONERROR);
				}
				
			}
			LocalFree(psid);
			Edit_SetText(GetDlgItem(hwnd, IDC_NEW_OWNER), TEXT(""));
			//BOOL RetRes = SetFileSecurityInfo(FileName, NewOwner, 0, NULL, FALSE);
		}
		ListViewInit(FileName, hwnd);
	}	
	break;

	case  ID_ADD_ACE:
	{
		int mdRes = DialogBox(GetWindowInstance(hwnd), MAKEINTRESOURCE(IDD_DIALOG_ACE), hwnd, DialogAceProc);// открытие диалоговое окно "Элемент разрешения"

		if (IDOK == mdRes)
		{	
			ListViewInit(FileName, hwnd);// выведем список с разрешениями
		} 
	}
	break;
	case ID_DELETE_ACE:
	{
		// список просмотра DACL - список разграничиваемого контроля доступа
		HWND hwndLV = GetDlgItem(hwnd, IDC_DACL);
		BOOL RetRes = FALSE;
		PACL lpDacl;
		BOOL bDaclPresent, bDaclDefaulted;
		DWORD dwRetCode;       // код возврата
		/*for (;;)
		{*/
			
		int item = ListView_GetNextItem(hwndLV, -1, LVNI_SELECTED);// находим выделенный элемент в списке просмотра DACL

		if (item == -1) // если нет выделенных элементов
		{
			break; 
		} 

		// удаление ACE - элементы контроля доступа из дескриптора безопасности
		//DeleteEntryFromDalc(Sec_Descriptor, iItem);

		// получаем DACL
		//RetRes = GetSecurityDescriptorDacl(Sec_Descriptor, &bDaclPresent, &lpDacl, &bDaclDefaulted);

		// получаем список DACL из дескриптора безопасности
		if (!GetSecurityDescriptorDacl(
			Sec_Descriptor,              // адрес дескриптора безопасности
			&bDaclPresent,     // признак присутствия списка DACL
			&lpDacl,           // адрес указателя на DACL
			&bDaclDefaulted))  // признак списка DACL по умолчанию
				{
					dwRetCode = GetLastError();
					MessageBox(hwnd, TEXT("Не удалось сменить владельца. Проверьте правильность введенного имени пользователя.", dwRetCode), NULL, MB_OK | MB_ICONERROR);
					//printf("Get security descriptor DACL failed.\n");
					//printf("Error code: %d\n", dwRetCode);
					break;
				}
		else
		{
				RetRes = DeleteAce(lpDacl, item);// удаляем элемент из DACL
		}

		
		ListView_DeleteItem(hwndLV, item);// удаляем элемент из списка просмотра DACL
		//} // for

		SetFileSecurity(FileName, DACL_SECURITY_INFORMATION, Sec_Descriptor); 		// изменяем дескриптор безопасности в файле/каталоге
	}
		break;
	case ID_LOAD:
		ListViewInit(FileName, hwnd);
		break;

	case ID_EXIT:
		SendMessage(hwnd, WM_CLOSE, 0, 0);
		RegCloseKey(hKey);
		break;
	}
}

BOOL ListViewInit(LPTSTR path, HWND hwnd)
{
	WIN32_FILE_ATTRIBUTE_DATA bhfi;
	BOOL RetRes;

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
	if (NULL != Sec_Descriptor) 
		LocalFree(Sec_Descriptor), 
		Sec_Descriptor = NULL;

	
	RetRes = GetFileSecurityDescriptor(FileName, OWNER_SECURITY_INFORMATION | DACL_SECURITY_INFORMATION, &Sec_Descriptor);// получение дескриптора безопасности

	if (RetRes != FALSE)
	{
		LPWSTR Owner = NULL; 
		RetRes = GetOwnerName_W(Sec_Descriptor, &Owner);//полуение имени владельца
		SetDlgItemText(hwnd, IDC_EDIT_OWNER, Owner);// копируем имя учетной записи владельца в поле "Текущий владелец"
		LocalFree(Owner);	
	}

	ULONG uCount = 0; // количество элементов в массиве ACE
	PEXPLICIT_ACCESS pEA = NULL; // массив ACE

	RetRes = GetItemFromDACL(Sec_Descriptor, &uCount, &pEA);//узнаем количество элементов в DACL

	if (RetRes != FALSE)
	{
		for (ULONG i = 0; i < uCount; ++i)
		{
			LVITEM lvItem = { LVIF_TEXT | LVIF_PARAM };

			lvItem.iItem = (int)i;

			lvItem.lParam = (LPARAM)pEA[i].grfAccessPermissions;			// определим права доступа

			// определим тип ACE
			switch (pEA[i].grfAccessMode)
			{
			case GRANT_ACCESS:
				lvItem.pszText = (LPTSTR)TEXT("Разрешить");
				break;
			case DENY_ACCESS:
				lvItem.pszText = (LPTSTR)TEXT("Запретить");
				break;
			} 

			// добавляем новый элемент в список просмотра DACL
			int iItem = ListView_InsertItem(hwndLV, &lvItem);
			if (iItem == -1) 
				continue; // не удалось добавить новый элемент

			 // определим имя учетной записи
			if (TRUSTEE_IS_SID == pEA[i].Trustee.TrusteeForm)
			{
				LPTSTR lpszName = NULL; // имя учетной записи
				
				GetAccountName_W(pEA[i].Trustee.ptstrName, &lpszName);// получим имя учетной записи

				if (NULL != lpszName)
				{
					
					ListView_SetItemText(hwndLV, iItem, 1, lpszName);// копируем имя учетной записи в ячейку списка просмотра DACL

					LocalFree(lpszName);
				} 
			} 
			
			DWORD grfInheritance = pEA[i].grfInheritance & (~INHERIT_NO_PROPAGATE);// определим к каким объектам применяются права доступа

			for (int j = 0; j < _countof(dwInherit); ++j)
			{
				if (grfInheritance == dwInherit[j]) // найдено значение
				{
					// копируем в ячейку списка просмотра DACL описание для значения поля grfInheritance
					ListView_SetItemText(hwndLV, iItem, 2, (LPTSTR)szInheritText[j]);

					break; // выходим из цикла
				} 
			} 
		}
	}

	return TRUE;
}


/*Дескрипторы*/
BOOL GetFileSecurityDescriptor(LPCWSTR lpFileName, SECURITY_INFORMATION RequestedInformation, PSECURITY_DESCRIPTOR *ppSD)
{
	DWORD lpnLengthNeeded = 0;

	GetFileSecurity(lpFileName, RequestedInformation, NULL, 0, &lpnLengthNeeded);	// определим размер дескриптора безопасности
	
	PSECURITY_DESCRIPTOR pSD = (PSECURITY_DESCRIPTOR)LocalAlloc(LMEM_FIXED, lpnLengthNeeded);// выделение памяти для дескриптора безопасности
	if (NULL == pSD) return FALSE;
		
	BOOL RetRes = GetFileSecurity(lpFileName, RequestedInformation, pSD, lpnLengthNeeded, &lpnLengthNeeded);// получим дескриптор безопасности

	if (FALSE != RetRes)
	{
		*ppSD = pSD; // возвращаем полученный дескриптор безопасности
	} 
	else
	{
		LocalFree(pSD); // освобождаем выделенную память
	} 

	return RetRes;
} 

BOOL GetItemFromDACL(PSECURITY_DESCRIPTOR pSecurityDescriptor, PULONG pcCountOfEntries, PEXPLICIT_ACCESS *pListOfEntries)
{
	PACL pDacl = NULL;
	BOOL lpbDaclPresent = FALSE,  //текущий
		lpbDaclDefaulted = FALSE;//по умолчанию?
	BOOL RetRes;
	DWORD Result; //код результата извлечения элментов из DALC
	
	// получаем DACL
	RetRes = GetSecurityDescriptorDacl(pSecurityDescriptor, &lpbDaclPresent, &pDacl, &lpbDaclDefaulted);

	if (RetRes != FALSE && lpbDaclPresent != FALSE)
	{
		// извлекаем элементы из DACL
		Result = GetExplicitEntriesFromAcl(pDacl, pcCountOfEntries, pListOfEntries);
		RetRes = (ERROR_SUCCESS == Result) ? TRUE : FALSE;
	} 
	else
	{
		*pcCountOfEntries = 0; // возвращаем 0 элементов
	} 

	return RetRes;
}
/*Получение имени аккаунта по его SID и по имени аккаунта*/
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

/*Определения функций определяющих SID*/
BOOL GetAccountSID_W(LPCWSTR AccountName, PSID *ppsid)
{
	BOOL RetRes = FALSE;
	SID_NAME_USE SidType;//переменная перечисляемого типа, сюда сохраним определенный тип SID

	/*Переменные для определения имени и SID*/
	LPWSTR RefDomainName = NULL;
	PSID psid = NULL;
	DWORD cbSID = 0, cchRefDomainName = 0;

	LookupAccountNameW(NULL, AccountName, NULL, &cbSID, NULL, &cchRefDomainName, NULL);//определение размеров буфера под имена

	if ((cbSID > 0) && (cchRefDomainName > 0))
	{
		psid = (PSID)LocalAlloc(LMEM_FIXED, cbSID); //выделение памяти из локальной кучи процесса
		RefDomainName = (LPWSTR)LocalAlloc(LMEM_FIXED, cchRefDomainName * sizeof(WCHAR));// -||- для имени домена
	}

	if ((psid != NULL) && (RefDomainName != NULL))
	{
		RetRes = LookupAccountName(NULL, AccountName, psid, &cbSID, RefDomainName, &cchRefDomainName, &SidType);
	}

	if (RetRes != FALSE)
	{
		*ppsid = psid;
	}
	else
	{
		if (psid != NULL)
		{
			LocalFree(psid);//освобождаем память
		}
	}

	if (RefDomainName != NULL)
	{
		LocalFree(RefDomainName);//освобождаем память
	}

	return RetRes;
}


/*Получение имени пользователя по дескриптору безопасности*/
BOOL GetOwnerName_W(PSECURITY_DESCRIPTOR Sec_Descriptor, LPWSTR *OwnerName)
{
	PSID psid;
	BOOL bDefaulted;
		
	BOOL bRet = GetSecurityDescriptorOwner(Sec_Descriptor, &psid, &bDefaulted);// получаем SID владельца

	if (FALSE != bRet)
	{
		bRet = GetAccountName_W(psid, OwnerName);// определяем имя учетной записи владельца
	} 
	return bRet;
}

/*Изменение информации в дескрипторе безопасности*/
BOOL SetFileSecurityInfo(LPCTSTR FileName, LPWSTR NewOwner,ULONG CountOfEntries, PEXPLICIT_ACCESS pListOfEntries, BOOL bMergeEntries)
{
	BOOL RetRes = FALSE;
	SECURITY_DESCRIPTOR secur_desc;
	
	/*Выделяем буферы для новых значений*/
	PSID psid_Owner = NULL;
	PACL pNewDacl = NULL;

	RetRes = InitializeSecurityDescriptor(&secur_desc, SECURITY_DESCRIPTOR_REVISION);

	if (RetRes != FALSE && NewOwner != NULL)
	{
		
		RetRes = GetAccountSID_W(NewOwner, &psid_Owner);//по имени владельца получим его SID
			

		if (RetRes != FALSE)
		{
			RetRes = SetSecurityDescriptorOwner(&secur_desc, psid_Owner, FALSE);//для связи дескриптора с SID
		}
	}

	if (RetRes != FALSE && CountOfEntries > 0 && pListOfEntries != NULL)
	{
		PSECURITY_DESCRIPTOR OldSD = NULL;
		PACL pOldDacl = NULL; // указатель на буфер для DACL

		BOOL DaclDefaulted = FALSE;
		BOOL DaclPresent;

		if (bMergeEntries != FALSE)
		{
			RetRes = GetFileSecurityDescriptor(FileName, DACL_SECURITY_INFORMATION, &OldSD);
			if (RetRes != FALSE)
			{
				GetSecurityDescriptorDacl(OldSD, &DaclPresent, &pOldDacl, &DaclDefaulted);
			}
		}

		DWORD result = SetEntriesInAcl(CountOfEntries, pListOfEntries, pOldDacl, &pNewDacl);
		RetRes = (ERROR_SUCCESS == result) ? TRUE:FALSE;
		if (RetRes != FALSE)
		{
			RetRes = SetSecurityDescriptorDacl(&secur_desc, TRUE, pNewDacl, DaclDefaulted);
		}
		if (OldSD != NULL)
			LocalFree(OldSD);
	}

	// проверяем структуру дескриптора безопасности
	if (!IsValidSecurityDescriptor(&secur_desc))
	{
		DWORD dwRetCode = GetLastError();

	}



	if (RetRes != NULL)
	{
		SECURITY_INFORMATION si = 0;
		if (psid_Owner != NULL) si |= OWNER_SECURITY_INFORMATION;
		if (pNewDacl != NULL) si |= DACL_SECURITY_INFORMATION;

		RetRes = SetFileSecurity(FileName, si, &secur_desc); //изменяем дескриптор безопасности для файла
	}

	/*Освобождение памяти*/
	if (psid_Owner != NULL)
	{
		LocalFree(psid_Owner);
	}
	if (pNewDacl != NULL)
	{
		LocalFree(pNewDacl);
	}

	return RetRes;
	
}


BOOL DialogAce_OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam)
{
	/*Variable*/
	PEXPLICIT_ACCESS pEA = (PEXPLICIT_ACCESS)lParam; // элемент ACE
	DWORD grfAccessMode;// для получения режима доступа
	HWND hwndCtl;//дескриптор для списка

	BOOL fEditable = (NULL == pEA) ? TRUE : FALSE; // флаг, определяющий возможность редактировать элемент ACE


	grfAccessMode = (NULL != pEA) ? pEA->grfAccessMode : GRANT_ACCESS;

	// заполняем выпадающий список "Тип": разрешить или запретить
	hwndCtl = GetDlgItem(hwnd, IDC_ACCESS_MODE);
	EnableWindow(hwndCtl, fEditable);

	int iItem = ComboBox_AddString(hwndCtl, TEXT("Разрешить"));

	if (iItem != -1)
	{
		ComboBox_SetItemData(hwndCtl, iItem, GRANT_ACCESS);
		if (GRANT_ACCESS == grfAccessMode) ComboBox_SetCurSel(hwndCtl, iItem);
	} // if

	iItem = ComboBox_AddString(hwndCtl, TEXT("Запретить"));

	if (iItem != -1)
	{
		ComboBox_SetItemData(hwndCtl, iItem, DENY_ACCESS);
		if (DENY_ACCESS == grfAccessMode) ComboBox_SetCurSel(hwndCtl, iItem);
	} 

	hwndCtl = GetDlgItem(hwnd, IDC_EDIT_NAME);
	Edit_SetText(hwndCtl, NULL);

	if (pEA != NULL)
	{
		Edit_SetReadOnly(hwndCtl, TRUE);

		LPTSTR AccountName = NULL; // имя учетной записи

		GetAccountName_W(pEA->Trustee.ptstrName, &AccountName);	// получим имя учетной записи

		if (AccountName != NULL)
		{
			// копируем имя учетной записи в поле "Имя"
			Edit_SetText(hwndCtl, AccountName);

			LocalFree(AccountName);
		} 
	}

	EnableWindow(GetDlgItem(hwnd, IDC_BUTTON_NAME_TEST), fEditable);

	if (NULL != pEA)
	{
		for (int i = 0; i < 13; ++i)
		{
			if (pEA->grfAccessPermissions & dwPermissions[i])
			{
				CheckDlgButton(hwnd, idcPermissions[i], BST_CHECKED);
			} 

			EnableWindow(GetDlgItem(hwnd, idcPermissions[i]), fEditable);
		} 
	} 


	if (FALSE != fEditable) // разрешено редактировать элемент ACE
	{
		// получим атрибуты файла/каталога
		DWORD dwFileAttributes = GetFileAttributes(FileName);

		// определим можно, ли редактировать элемент ACE (только для каталогов)
		fEditable = ((INVALID_FILE_ATTRIBUTES != dwFileAttributes) && (dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) ? TRUE : FALSE;
	} 


	// заполняем выпадающий список "Применять"

	DWORD selInheritance = (NULL != pEA) ? (pEA->grfInheritance & (~INHERIT_NO_PROPAGATE)) : SUB_CONTAINERS_AND_OBJECTS_INHERIT;

	hwndCtl = GetDlgItem(hwnd, IDC_INHERIT);
	EnableWindow(hwndCtl, fEditable);

	for (int i = 0; i < 7; ++i)
	{
		int iItem = ComboBox_AddString(hwndCtl, szInheritText[i]);

		if (iItem != -1)
		{
			ComboBox_SetItemData(hwndCtl, iItem, dwInherit[i]);
			if (selInheritance == dwInherit[i]) ComboBox_SetCurSel(hwndCtl, iItem);
		} 
	} 

	if ((NULL != pEA) && (pEA->grfInheritance & INHERIT_NO_PROPAGATE))
	{
		CheckDlgButton(hwnd, IDC_CHECK_INHERIT_NO_PROPAGATE, BST_CHECKED);
	} 

	EnableWindow(GetDlgItem(hwnd, IDC_CHECK_INHERIT_NO_PROPAGATE), fEditable);


	return TRUE;
} 

// ----------------------------------------------------------------------------------------------
void DialogAce_OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify)
{
	switch (id)
	{
	case IDOK:
	{
		HWND hwndCtl = GetDlgItem(hwnd, IDC_EDIT_NAME);

		if (IsWindowEnabled(hwndCtl) != FALSE) // добавить новый ACE
		{
			PSID pSid = NULL; // указатель на дескриптор безопасности

			int cch = GetWindowTextLength(hwndCtl);

			if (cch > 0)
			{
				LPTSTR lpszName = new TCHAR[cch + 1]; // имя учётной записи

				// копируем имя учётной записи из поля ввода
				GetDlgItemText(hwnd, IDC_EDIT_NAME, lpszName, cch + 1);
				// получим SID учётной записи
				GetAccountSID_W(lpszName, &pSid);

				// освобождаем выделенную память
				delete[] lpszName, lpszName = NULL;
			} 

			if (NULL != pSid)
			{
				EXPLICIT_ACCESS ea;

				ea.Trustee.TrusteeForm = TRUSTEE_IS_SID; // доверенный объект задается с помощью SID
				ea.Trustee.ptstrName = (LPTSTR)pSid; // SID

				// формируем разрешения

				ea.grfAccessPermissions = 0;

				for (int i = 0; i < 13; ++i)
				{
					if (IsDlgButtonChecked(hwnd, idcPermissions[i]) == BST_CHECKED)
					{
						ea.grfAccessPermissions |= dwPermissions[i];
					} 
				} 

				// определим тип разрешения

				hwndCtl = GetDlgItem(hwnd, IDC_ACCESS_MODE);
				ea.grfAccessMode = (ACCESS_MODE)ComboBox_GetItemData(hwndCtl, ComboBox_GetCurSel(hwndCtl));


				hwndCtl = GetDlgItem(hwnd, IDC_INHERIT);
				ea.grfInheritance = (DWORD)ComboBox_GetItemData(hwndCtl, ComboBox_GetCurSel(hwndCtl));

				if (( ea.grfInheritance != NO_INHERITANCE) &&
					(IsDlgButtonChecked(hwnd, IDC_CHECK_INHERIT_NO_PROPAGATE) == BST_CHECKED))
				{
					ea.grfInheritance |= INHERIT_NO_PROPAGATE;
				} 


				if (0 != ea.grfAccessPermissions)
				{
					// изменяем информацию в дескрипторе безопасности
					BOOL RetRes = SetFileSecurityInfo(FileName, NULL, 1, &ea, TRUE);

					if (RetRes != FALSE)
					{					
						EndDialog(hwnd, IDOK);// закрываем диалоговое окно
					} 
				} 
				LocalFree(pSid);
			} 
			else
			{
				MessageBox(hwnd, TEXT("Не удалось найти учетную запись. Проверьте правильность введенного имени."), NULL, MB_OK | MB_ICONERROR);
			} 
		} 
		else
		{
			EndDialog(hwnd, IDOK);	// закрываем диалоговое окно
		} 
	}
	break;

	case IDCANCEL:
		EndDialog(hwnd, IDCANCEL);
		break;

	case IDC_BUTTON_NAME_TEST: // Проверить
	{
		int cch = GetWindowTextLength(GetDlgItem(hwnd, IDC_EDIT_NAME));

		if (cch > 0)
		{
			PSID pSid = NULL;

			LPTSTR AccountName = new TCHAR[cch + 1]; // имя учётной записи
			
			GetDlgItemText(hwnd, IDC_EDIT_NAME, AccountName, cch + 1);// копируем имя учётной записи из поля ввода
			
			GetAccountSID_W(AccountName, &pSid);		// получим SID учётной записи

			delete[] AccountName, AccountName = NULL;	// освобождаем выделенную память

			if ( pSid != NULL)
			{		
				GetAccountName_W(pSid, &AccountName);	// получим имя учётной записи
				
				SetDlgItemText(hwnd, IDC_EDIT_NAME, AccountName);	// копируем имя учётной записи в поле ввода
				
				if (AccountName != NULL)
					LocalFree(AccountName);// освобождаем выделенную память

				LocalFree(pSid);

				MessageBox(hwnd, TEXT("Учетная запись есть."), NULL, MB_OK | MB_ICONERROR);
			} 
			else
			{
				MessageBox(hwnd, TEXT("Не удалось найти учетную запись. Проверьте правильность введенного имени."), NULL, MB_OK | MB_ICONERROR);

				// удаляем текст из поля "Имя"
				SetDlgItemText(hwnd, IDC_EDIT_NAME, NULL);
			} 
		} 
	}
	break;
	} 
} // DialogAce_OnCommand




//// ----------------------------------------------------------------------------------------------
//BOOL DeleteEntryFromDalc(PSECURITY_DESCRIPTOR pSD, DWORD dwIndex)
//{
//	PACL pDacl;
//	BOOL bDaclPresent, bDaclDefaulted;
//
//	// получаем DACL
//	BOOL bRet = GetSecurityDescriptorDacl(pSD, &bDaclPresent, &pDacl, &bDaclDefaulted);
//
//	if (FALSE != bRet)
//	{
//		// удаляем элемент из DACL
//		bRet = DeleteAce(pDacl, dwIndex);
//	} // if
//
//	return bRet;
//} // DeleteEntryFromDalc