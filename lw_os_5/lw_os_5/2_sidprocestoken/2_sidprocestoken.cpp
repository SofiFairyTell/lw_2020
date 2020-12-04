#include <Windows.h>
#include <WindowsX.h>
#include <tchar.h>
#include <strsafe.h>
#include "resource.h"
#include <string>
#include <Psapi.h> // для GetModuleBaseName
#include <NTSecAPI.h>//for las_handle
#include <AclAPI.h>
#include <winnt.h>
#include <sddl.h>//for ConvertSidToStringSidW

#define IDC_LB_PROCESSES				2001
#define IDC_LB_GROUPS					2002
#define IDC_LB_PRIVILEGES				2003

#define IDC_EDIT_ACCOUNT				2004
#define IDC_EDIT_SID					2005

#define IDC_BUTTON_PRIVILEGE_ENABLE		2006
#define IDC_BUTTON_PRIVILEGE_DISABLE	2007
#define IDC_PRIVILEGES					2008





LRESULT CALLBACK MainWindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
/*Обработчики сообщений WM_CREATE WM_DESTROY WM_SIZE WM_COMMAND */

BOOL OnCreate(HWND hwnd, LPCREATESTRUCT lpCreateStruct);
void OnDestroy(HWND hwnd);
void OnSize(HWND hwnd, UINT state, int cx, int cy);
void OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify);

// ------------------------------------------------------------------------------------------------

		/*для токенов*/

		HANDLE	OpenProcessTokenPID(UINT PID, DWORD AccessRIGHT);//открыть процесс по его PID
		BOOL	GetAccountName_W(PSID psid, LPWSTR* AccountName);//получить имя пользователя по его SID

		BOOL	GetTokenUser(HANDLE token, PSID* psid);//узнать маркер доступа пользователя по его SID
		BOOL	GetTokenGR(HANDLE token, PTOKEN_GROUPS *tk_groups);//узнать список ГРУПП по маркеру доступа
		BOOL	GetTokenPR(HANDLE token, PTOKEN_PRIVILEGES *tk_PR);//узнать список ПРИВИЛЕГИЙ по маркеру доступа
		BOOL	ExistPR(HWND hwnd, HANDLE token, LPCWSTR PrivilName);//узнать если ли искомая привлегия и у процесса
		
		/*Загрузка процессов и модулей этих процессов в LISTBOX*/
		void	ToLB_LoadProcesses(HWND hwndCtl);//процессы в LB
		void	ToLB_LoadTokenGroup(HWND hwndListGroups, HANDLE token);//группы привелегий в LB
		void	ToLB_LoadTokenPrivil(HWND hwndListPrivileges, HANDLE token);
// ------------------------------------------------------------------------------------------------

		/*Кнопки*/
		BOOL AdjustPrivileges(HANDLE hToken, LPCTSTR names[], DWORD NewState[], DWORD dwCount);
		BOOL SwitchOnPrivil(HANDLE hToken, LPCWSTR PrivilName, BOOL bEnable);

int WINAPI _tWinMain(HINSTANCE hInstance, HINSTANCE, LPTSTR lpszCmdLine, int nCmdShow)
{
	LoadLibrary(TEXT("ComCtl32.dll"));//для элементов общего пользования								

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

	HACCEL hAccel = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDR_ACCELERATOR1));

	// создаем главное окно на основе нового оконного класса

	HWND hWnd = CreateWindowEx(0, TEXT("MainWindowClass"), TEXT("Process"), WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

	if (NULL == hWnd)
	{
		return -1; // завершаем работу приложения
	}


	ShowWindow(hWnd, nCmdShow); // отображаем главное окно

	MSG  msg;
	BOOL bRet;

	while ((bRet = GetMessage(&msg, NULL, 0, 0)) != FALSE)
	{
		if (!TranslateAccelerator(hWnd, hAccel, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	
	return (int)msg.wParam;
}

LRESULT CALLBACK MainWindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
		HANDLE_MSG(hWnd, WM_CREATE, OnCreate);
		HANDLE_MSG(hWnd, WM_DESTROY, OnDestroy);
		HANDLE_MSG(hWnd, WM_SIZE, OnSize);
		HANDLE_MSG(hWnd, WM_COMMAND, OnCommand);
	}
	return DefWindowProc(hWnd, uMsg, wParam, lParam);	// передача необработанного сообщения
}

/*Создание оконного приложения с 2-мя listbox и меню*/
BOOL OnCreate(HWND hwnd, LPCREATESTRUCT lpCreateStruct)
{
	//Создаем список открытых процессов
	CreateWindowEx(0, TEXT("Static"), TEXT("Процессы:"), WS_CHILD | WS_VISIBLE | SS_SIMPLE,
		10, 10, 400, 20, hwnd, NULL, lpCreateStruct->hInstance, NULL);
	HWND hwndCtl = CreateWindowEx(0, TEXT("ListBox"), TEXT(""), WS_CHILD | WS_VISIBLE | LBS_STANDARD,
		10, 30, 400, 400, hwnd, (HMENU)IDC_LB_PROCESSES, lpCreateStruct->hInstance, NULL);
	ToLB_LoadProcesses(hwndCtl);// получаем список процессов активных сейчас

	//список для перечисления групп
	CreateWindowEx(0, TEXT("Static"), TEXT("Группы"), WS_CHILD | WS_VISIBLE | SS_SIMPLE,
		420, 10, 300, 20, hwnd, NULL, lpCreateStruct->hInstance, NULL);

	CreateWindowEx(0, TEXT("ListBox"), TEXT(""), WS_CHILD | WS_VISIBLE | WS_HSCROLL | WS_VSCROLL | WS_BORDER,
		420, 30, 300, 300, hwnd, (HMENU)IDC_LB_GROUPS, lpCreateStruct->hInstance, NULL);

	
	//Поле "Пользователь"
	CreateWindowEx(0, TEXT("Static"), TEXT("Пользователь:"), WS_CHILD | WS_VISIBLE | SS_SIMPLE,
		420, 330, 200, 20, hwnd, NULL, lpCreateStruct->hInstance, NULL);

	CreateWindowEx(0, TEXT("Edit"), TEXT(""), WS_CHILD | WS_VISIBLE | ES_LEFT | ES_READONLY | ES_AUTOHSCROLL,
		630, 330, 400, 20, hwnd, (HMENU)IDC_EDIT_ACCOUNT, lpCreateStruct->hInstance, NULL);

	// Поле "SID"
	CreateWindowEx(0, TEXT("Static"), TEXT("SID:"), WS_CHILD | WS_VISIBLE | SS_SIMPLE,
		420, 380, 200, 20, hwnd, NULL, lpCreateStruct->hInstance, NULL);

	CreateWindowEx(0, TEXT("Edit"), TEXT(""), WS_CHILD | WS_VISIBLE | ES_LEFT | ES_READONLY | ES_AUTOHSCROLL,
		630, 380, 400, 20, hwnd, (HMENU)IDC_EDIT_SID, lpCreateStruct->hInstance, NULL);

	//список для перечисления привилегий
	CreateWindowEx(0, TEXT("Static"), TEXT("Привилегии:"), WS_CHILD | WS_VISIBLE | SS_SIMPLE,
		730, 10, 300, 20, hwnd, NULL, lpCreateStruct->hInstance, NULL);

	CreateWindowEx(0, TEXT("ListBox"), TEXT(""), WS_CHILD | WS_VISIBLE | LBS_STANDARD,
		730, 30, 300, 300, hwnd, (HMENU)IDC_LB_PRIVILEGES, lpCreateStruct->hInstance, NULL);

	
	// Кнопки Вкл/Выкл
	hwndCtl = CreateWindowEx(0, TEXT("Button"), TEXT("Включить"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
		630, 480, 110, 30, hwnd, (HMENU)IDC_BUTTON_PRIVILEGE_ENABLE, lpCreateStruct->hInstance, NULL);
	EnableWindow(hwndCtl, FALSE);	// отключение "Включить"

	hwndCtl = CreateWindowEx(0, TEXT("Button"), TEXT("Выключить"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
		730, 480, 110, 30, hwnd, (HMENU)IDC_BUTTON_PRIVILEGE_DISABLE, lpCreateStruct->hInstance, NULL);
	EnableWindow(hwndCtl, FALSE);	// отключение "Выключить"



	
	//список привелегий для поиска
	CreateWindowEx(0, TEXT("Static"), TEXT("Для проверки:"), WS_CHILD | WS_VISIBLE | SS_SIMPLE,
		420, 400, 300, 20, hwnd, NULL, lpCreateStruct->hInstance, NULL);
	hwndCtl = CreateWindowEx(0, TEXT("ListBox"), TEXT(""), WS_CHILD | WS_VISIBLE | LBS_STANDARD,
		420, 430, 300, 50, hwnd, (HMENU)IDC_PRIVILEGES, lpCreateStruct->hInstance, NULL);


	constexpr LPCTSTR Privilege[5] =
	{
		SE_LOAD_DRIVER_NAME, SE_LOCK_MEMORY_NAME,
		SE_MACHINE_ACCOUNT_NAME, SE_MANAGE_VOLUME_NAME,
		SE_PROF_SINGLE_PROCESS_NAME
	};
	for (int i = 0; i < _countof(Privilege); ++i)
	{
		int iItem = ListBox_AddString(hwndCtl, Privilege[i]);
	}
	ListBox_SetCurSel(hwndCtl, 0);

/*	// кнопка "Проверить"
	CreateWindowEx(0, TEXT("Button"), TEXT("Проверка"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
		420, 615, 110, 30, hwnd, (HMENU)IDC_PRIVILEGE_CHECK, lpCreateStruct->hInstance, NULL);*/

	return TRUE;
}

/*При завершении работы с приложением*/
void OnDestroy(HWND hwnd)
{
	PostQuitMessage(0); // отправляем сообщение WM_QUIT
}

/*Список меняет ширину и высоту*/
void OnSize(HWND hwnd, UINT state, int cx, int cy)
{
	if (state != SIZE_MINIMIZED)
	{
		// изменяем высоту списка для перечисления процессов
		MoveWindow(GetDlgItem(hwnd, IDC_LB_PROCESSES), 10, 30, 400, cy - 40, TRUE);

		//// изменяем высоту списка для перечисления ГРУПП
		//MoveWindow(GetDlgItem(hwnd, IDC_LB_GROUPS), 420, 30, 250, cy - 40, TRUE);

		//// изменяем ширину списка привилегий
		//MoveWindow(GetDlgItem(hwnd, IDC_LB_PRIVILEGES), 420, 330, cx - 430, 200, TRUE);

		//// изменяем ширину поля "Пользователь"
		//MoveWindow(GetDlgItem(hwnd, IDC_EDIT_ACCOUNT), 570, 30, 100, 20, TRUE);
		//// изменяем ширину поля "SID"
		//MoveWindow(GetDlgItem(hwnd, IDC_EDIT_SID), 570, 50, cx - 580, 20, TRUE);

		//// изменяем ширину выпадающего списка "Привилегии"
		//MoveWindow(GetDlgItem(hwnd, IDC_PRIVILEGES), 420, 580, cx - 430, 30, TRUE);

	}
}

void OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify)
{
	switch (id)
	{
	case IDC_LB_PROCESSES:
	{

		if (LBN_SELCHANGE == codeNotify) // выбран другой элемент в списке процессов
		{

			/*Очистка полей*/
			SetDlgItemText(hwnd, IDC_EDIT_ACCOUNT, NULL);// очистим поле "Пользователь"
			SetDlgItemText(hwnd, IDC_EDIT_SID, NULL);// очистим поле "SID"

			/*Получение hwnd для групп*/
			HWND hwndListGroups = GetDlgItem(hwnd, IDC_LB_GROUPS);
			ListBox_ResetContent(hwndListGroups);// очистим список групп

			/*Получение hwnd для привилегий*/
			HWND hwndListPrivileges = GetDlgItem(hwnd, IDC_LB_PRIVILEGES);
			ListBox_ResetContent(hwndListPrivileges);// очистим список  привилегий


			int iItem = ListBox_GetCurSel(hwndCtl);//номер выделенной строки процесса
			if (iItem != -1)
			{
				UINT PID = (UINT)ListBox_GetItemData(hwndCtl, iItem);// определение ID процесса 	
				/*Маркеры доступа процесса*/
				HANDLE token = OpenProcessTokenPID(PID, TOKEN_QUERY);

				if (token != NULL)
				{
					//Получим SID пользователя
					PSID psid = NULL;
					BOOL RetRes = GetTokenUser(token, &psid);//узнать SID по маркеру доступа пользователя
						if (RetRes != FALSE)
						{
							LPWSTR Name = NULL;
							GetAccountName_W(psid, &Name);//получить имя пользователя

							if (Name != NULL)
							{
								SetDlgItemText(hwnd, IDC_EDIT_ACCOUNT, Name);//записать имя пользователя в поле
								LocalFree(Name), Name = NULL;
							}

							ConvertSidToStringSid(psid, &Name);//преобразование SID в строку

							if (Name != NULL)
							{
								SetDlgItemText(hwnd, IDC_EDIT_SID, Name);
								LocalFree(Name), Name = NULL;
							}
						
							//LocalFree(psid), psid = NULL;
						}
					ToLB_LoadTokenGroup(hwndListGroups, token);
					ToLB_LoadTokenPrivil(hwndListPrivileges,token);
					CloseHandle(token);
				}
			}
		}
	} break;

	case IDC_LB_PRIVILEGES: // Обновление списка процессов
	{
		if (LBN_SELCHANGE == codeNotify)
		{
			
			int iItem = ListBox_GetCurSel(hwndCtl);//номер выбранной привилегии

			if (iItem != -1)
			{
			
				DWORD atrb = (DWORD)ListBox_GetItemData(hwndCtl, iItem);// определяем атрибуты привилегии

				if (atrb & SE_PRIVILEGE_ENABLED)
				{
					
					EnableWindow(GetDlgItem(hwnd, IDC_BUTTON_PRIVILEGE_ENABLE), FALSE);// отключим кнопку "Включить"			
					EnableWindow(GetDlgItem(hwnd, IDC_BUTTON_PRIVILEGE_DISABLE), TRUE);// включим кнопку "Выключить"
				} 
				else
				{				
					EnableWindow(GetDlgItem(hwnd, IDC_BUTTON_PRIVILEGE_ENABLE), TRUE);// включим кнопку "Включить"		
					EnableWindow(GetDlgItem(hwnd, IDC_BUTTON_PRIVILEGE_DISABLE), FALSE);// отключим кнопку "Выключить"
				} 
			} 
		}
	}	break;
	
	case IDC_BUTTON_PRIVILEGE_ENABLE: // включить привилегию
	case IDC_BUTTON_PRIVILEGE_DISABLE: // выключить привилегию
	{
		HWND hwnd_ProcessList = GetDlgItem(hwnd, IDC_LB_PROCESSES);
		HWND hwnd_PrivilegList = GetDlgItem(hwnd, IDC_LB_PRIVILEGES);
		
		UINT PID;

		int item = ListBox_GetCurSel(hwnd_ProcessList);
		if (item != -1)
		{
			PID = (UINT)ListBox_GetItemData(hwnd_ProcessList, item);//выбранный процесс
			item = ListBox_GetCurSel(hwnd_PrivilegList); //выбранная привилегеия
		}

		if (item != -1)
		{
			HANDLE token = OpenProcessTokenPID(PID, TOKEN_QUERY);//получить маркер доступа процесса 
			if (token != NULL)
			{
				TCHAR NamePrivil[256];
				ListBox_GetText(hwnd_PrivilegList, item, NamePrivil);//получение имени привилегии

				LPTSTR priv = _tcschr(NamePrivil, L' '); //для удаления суффикса состояния привилегии
				
				if (priv != NULL)
				{
					*priv = TEXT('\0');
				}

				BOOL RetRes = SwitchOnPrivil(token, NamePrivil, (IDC_BUTTON_PRIVILEGE_ENABLE == id) ? TRUE : FALSE);
				if (FALSE != RetRes)
				{
					// загрузим список привилегий
					ToLB_LoadTokenPrivil(hwnd_PrivilegList, token);

					// задаём текущий элемент списка привилегий
					ListBox_SetCurSel(hwnd_PrivilegList, item);
					// передаём фокус клавиатуры списку привилегий
					SetFocus(hwnd_PrivilegList);

					if (IDC_BUTTON_PRIVILEGE_ENABLE == id) // привилегия была включена
					{
						// отключим кнопку "Включить"
						EnableWindow(GetDlgItem(hwnd, IDC_BUTTON_PRIVILEGE_ENABLE), FALSE);
						// включим кнопку "Выключить"
						EnableWindow(GetDlgItem(hwnd, IDC_BUTTON_PRIVILEGE_DISABLE), TRUE);
					} // if
					else // привилегия была выключена
					{
						// включим кнопку "Включить"
						EnableWindow(GetDlgItem(hwnd, IDC_BUTTON_PRIVILEGE_ENABLE), TRUE);
						// отключим кнопку "Выключить"
						EnableWindow(GetDlgItem(hwnd, IDC_BUTTON_PRIVILEGE_DISABLE), FALSE);
					} // else
				} // if
				else
				{
					MessageBox(hwnd, TEXT("Не удалось изменить состояние привилегии"), NULL, MB_OK);
				}
				// закрываем дескриптор маркера доступа
				CloseHandle(token);
			}
		}
		
	}break; 


	case IDC_PRIVILEGE_CHECK:
	{
		HWND hwnd_ProcessList = GetDlgItem(hwnd, IDC_LB_PROCESSES);
		HWND hwnd_PrivilegList = GetDlgItem(hwnd, IDC_PRIVILEGES);

		UINT PID;
		BOOL RetRes = FALSE;
		int item = ListBox_GetCurSel(hwnd_ProcessList);
		if (item != -1)
		{
			PID = (DWORD)ListBox_GetItemData(hwnd_ProcessList, item);
			item = ListBox_GetCurSel(hwnd_PrivilegList);
		}
		if (item != -1)
		{
			HANDLE token = OpenProcessTokenPID(PID, TOKEN_QUERY);
			if (token != NULL)
			{
				WCHAR PrivilName[256];
				ListBox_GetText(hwnd_PrivilegList, item, PrivilName);
				RetRes = ExistPR(hwnd,token, PrivilName);
				if (RetRes == -1)
				{
					MessageBox(hwnd, L"Наличие привилегии в маркере не определено", L"НЕПОПРАВИМАЯ ОШИБКА" ,MB_OK);
				}
				else 
					if (RetRes == FALSE)
					{
						MessageBox(hwnd, L"Такой привилегии у процесса нет ",L"Результат" ,MB_OK);
					}
					/*else 
						if (RetRes == TRUE)
							{
								MessageBox(hwnd, L"Такая привелегия у процесса есть и она включена", L"Результат", MB_OK);
							}*/			
				CloseHandle(token);
			}		
			else
			{
				MessageBox(hwnd, L"Маркер доступа открыть не удалось", L"Результат", MB_OK);
			}
	}
		

	}break;
	
	}
}


/*Определение токена по идентификатору процесса*/
HANDLE OpenProcessTokenPID(UINT PID, DWORD AccessRIGHT)
{
	HANDLE token = NULL;
	HANDLE process = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, PID);

	if (process != NULL)
	{
		OpenProcessToken(process, AccessRIGHT, &token);//определение маркера доступа по PID
		CloseHandle(process);
	}
	return token;
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


/*To ListBox load's*/

void ToLB_LoadProcesses(HWND hwndCtl)
{
	ListBox_ResetContent(hwndCtl);//очистка списка

	DWORD PIDarray[1024], cbNeeded = 0;//массив для ID созданных процессов
	BOOL bRet = EnumProcesses(PIDarray, sizeof(PIDarray), &cbNeeded);//получение списка ID созданных процессоров

	if (FALSE != bRet)
	{
		TCHAR ProcessName[MAX_PATH], szBuffer[300];

		for (DWORD i = 0,
			n = cbNeeded / sizeof(DWORD); i < n; ++i)
		{
			DWORD PID = PIDarray[i], cch = 0;
			if (0 == PID) continue;


			HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, PID);//получение дескриптора процесса по его ID

			if (NULL != hProcess)
			{
				cch = GetModuleBaseName(hProcess, NULL, ProcessName, _countof(ProcessName));// получаем имя главного модуля процесса

				CloseHandle(hProcess); // закрываем объект ядра
			}

			if (0 == cch)
				StringCchCopy(ProcessName, MAX_PATH, TEXT("Имя процесса не определено"));

			StringCchPrintf(szBuffer, _countof(szBuffer), TEXT("%s (PID: %u)"), ProcessName, PID);


			int iItem = ListBox_AddString(hwndCtl, szBuffer);

			ListBox_SetItemData(hwndCtl, iItem, PID);//запись в ListBox имени процесса
		}
	}
}

void ToLB_LoadTokenGroup(HWND hwndListGroups, HANDLE token)
{
	ListBox_ResetContent(hwndListGroups);
	TOKEN_GROUPS* tk_groups = NULL;

	if (GetTokenGR(token, &tk_groups) != FALSE)
	{
		for (UINT i = 0; i < tk_groups->GroupCount; ++i)
		{
			LPWSTR Name = NULL;
			GetAccountName_W(tk_groups->Groups[i].Sid, &Name);

			if (Name != NULL)
			{
				ListBox_AddString(hwndListGroups, Name);
				LocalFree(Name), Name = NULL;
			}		
		}
		LocalFree(tk_groups);
	}
}

void ToLB_LoadTokenPrivil(HWND hwndListPrivileges, HANDLE token)
{
	ListBox_ResetContent(hwndListPrivileges);
	TOKEN_PRIVILEGES* tk_PR = NULL;

	if (GetTokenPR(token, &tk_PR) != FALSE)
	{
		TCHAR Name[256];
		for (UINT i = 0; i < tk_PR->PrivilegeCount; ++i)
		{
			//LUID -   structure is an opaque structure that specifies an 
			// identifier that is guaranteed to be unique on the local machine
			LUID Luid = tk_PR->Privileges[i].Luid;
			
			DWORD count_char = _countof(Name);//определение имени привилегии 
			LookupPrivilegeName(NULL, &Luid, Name, &count_char);

			DWORD atrb = tk_PR->Privileges[i].Attributes;
			
			if (atrb & SE_PRIVILEGE_ENABLED)
			{
				StringCchCat(Name, _countof(Name), TEXT("(включена)")); //добавление суффикса для отображения состояния привилегии
			}
				int item = ListBox_AddString(hwndListPrivileges, Name);//запись имени привилегии
				ListBox_SetItemData(hwndListPrivileges, item, atrb);//запись полученных атрибутов
		}
		LocalFree(tk_PR);
	}
}


/*Токены*/
BOOL GetTokenGR(HANDLE token, PTOKEN_GROUPS *tk_groups)
{
	DWORD tkSize;
	BOOL RetRes = FALSE;
	GetTokenInformation(token, TokenGroups, NULL, 0, &tkSize);//определение размера блока памяти

	PTOKEN_GROUPS Groups = (PTOKEN_GROUPS)LocalAlloc(LPTR, tkSize);

	if (Groups == NULL) return FALSE;

	if (GetTokenInformation(token, TokenGroups, Groups, tkSize, &tkSize))//получение списка привелегий в маркере доступа
	{
		RetRes = TRUE;
		*tk_groups = Groups;
	}
	else
	{
		LocalFree(Groups);
	}
	return RetRes;
}

BOOL GetTokenUser(HANDLE token, PSID *psid)
{
	DWORD tkSize;
	TOKEN_USER *tkUser;
	DWORD sidLeng;
	BOOL RetRes = FALSE;

	GetTokenInformation(token, TokenUser, NULL, 0, &tkSize);//определение размера
	tkUser = (TOKEN_USER*)malloc(tkSize); //выделение блока памяти
	if (tkUser == NULL)
	{
		return E_OUTOFMEMORY; 
	}
	
	if (GetTokenInformation(token, TokenUser, tkUser, tkSize, &tkSize))
	{
		sidLeng = GetLengthSid(tkUser->User.Sid);
		PSID npsid = NULL;
		npsid = (PSID)malloc(sidLeng);
		if (npsid == NULL)
		{
			return E_OUTOFMEMORY;
		}

		RetRes = CopySid(sidLeng, npsid, tkUser->User.Sid); //копируем опеределнный SID перед возвратом

		if (RetRes != FALSE)
		{
			*psid = npsid;
		}
		else
		{
			LocalFree(npsid);
			//free(tkUser)
		}
	}

	return RetRes;
}

BOOL GetTokenPR(HANDLE token, PTOKEN_PRIVILEGES *tk_PR)
{
	DWORD tkSize;
	BOOL RetRes = FALSE;
	GetTokenInformation(token, TokenPrivileges, NULL, 0, &tkSize);//определение размера блока памяти

	PTOKEN_PRIVILEGES privil = (PTOKEN_PRIVILEGES)LocalAlloc(LPTR, tkSize);

	if (privil == NULL) return FALSE;

	if (GetTokenInformation(token, TokenPrivileges, privil, tkSize, &tkSize))//получение списка привелегий в маркере доступа
	{
		RetRes = TRUE;
		*tk_PR = privil;
	}
	else
	{
		LocalFree(privil);
	}
	return RetRes;
}

BOOL ExistPR(HWND hwnd, HANDLE token, LPCWSTR PrivilName)
{
	LUID luid;
	BOOL RetRes, ResSetPriv, result;

	PTOKEN_PRIVILEGES tk_PR = NULL;
	PRIVILEGE_SET privil_set;//проверим состояние в которое установлена привилегия

	privil_set.PrivilegeCount = 1;//количество привилегий
	privil_set.Control = PRIVILEGE_SET_ALL_NECESSARY;//if all of the privileges must be enabled;
	//or set it to zero if it is sufficient that any one of the privileges be enabled.

	RetRes = LookupPrivilegeValueW(NULL,PrivilName,&luid); 
	ResSetPriv = LookupPrivilegeValueW(NULL, PrivilName, &privil_set.Privilege[0].Luid); //а они случайно одно и то же не делают?

	if (RetRes != FALSE)
	{
		RetRes = GetTokenPR(token, &tk_PR);
	}
	if (RetRes != FALSE)
	{
		for (UINT i = 0; i < tk_PR->PrivilegeCount; ++i)
		{
			int res = memcmp(&tk_PR->Privileges[i].Luid, &luid, sizeof(LUID));
			if (res == 0)
			{
				RetRes = TRUE;
				//выясним состояние привилегии -> включена/выключена
				if (ResSetPriv != FALSE)
				{
					ResSetPriv = PrivilegeCheck(token, &privil_set, &result);
					if (result != FALSE)
					{
						MessageBox(hwnd, L"Такая привелегия у процесса есть и она включена", L"Результат", MB_OK);
					}
					else
					{
						MessageBox(hwnd, L"Такая привелегия у процесса есть и она выключена", L"Результат", MB_OK);
					}
					
				}			
				
			}
		}
	}
	else {	return -1; }

	LocalFree(tk_PR);//освобождение памяти

	return RetRes;
}

/*Кнопки*/
BOOL SwitchOnPrivil(HANDLE hToken, LPCWSTR PrivilName, BOOL bEnable)
{
	TOKEN_PRIVILEGES Privileges;
	Privileges.PrivilegeCount = 1; // количество привилегий

	// установим новое состояние указанной привилегии
	Privileges.Privileges[0].Attributes = (FALSE != bEnable) ? SE_PRIVILEGE_ENABLED : 0;

	// определяем LUID указанной привилегии
	BOOL bRet = LookupPrivilegeValue(NULL, PrivilName, &Privileges.Privileges[0].Luid);

	if (FALSE != bRet)
	{
		// изменяем состояние указанной привилегии
		bRet = AdjustTokenPrivileges(hToken, FALSE, &Privileges, sizeof(TOKEN_PRIVILEGES), NULL, NULL);//ошибка здесь
	} // if

	return bRet;
} 

BOOL AdjustPrivileges(HANDLE hToken, LPCTSTR names[], DWORD NewState[], DWORD dwCount)
{

	DWORD cb = sizeof(TOKEN_PRIVILEGES) + (dwCount - 1) * sizeof(LUID_AND_ATTRIBUTES);// определеям размер блока памяти (в байтах)

	PTOKEN_PRIVILEGES Privileges = (PTOKEN_PRIVILEGES)LocalAlloc(LPTR, cb);	// выделяем блок памяти
	if (NULL == Privileges) return FALSE;

	BOOL bRet = TRUE;
	// задаем количество указанных привилегий
	Privileges->PrivilegeCount = dwCount;

	for (DWORD i = 0; i < dwCount && FALSE != bRet; ++i)
	{
		Privileges->Privileges[i].Attributes = NewState[i];
		bRet = LookupPrivilegeValue(NULL, names[i], &Privileges->Privileges[i].Luid);
	} 

	if (FALSE != bRet)
	{
		// изменяем состояние указанных привилегий
		bRet = AdjustTokenPrivileges(hToken, FALSE, Privileges, cb, NULL, NULL);
	} 
	// освобождаем выделенную память
	LocalFree(Privileges);
	return bRet;
} 