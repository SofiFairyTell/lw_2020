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
#define IDC_PRIVILEGE_CHECK				2009



HANDLE hJob = NULL; // дескриптор задания
// оконная процедура главного окна
LRESULT CALLBACK MainWindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
/*Обработчики сообщений WM_CREATE WM_DESTROY WM_SIZE WM_COMMAND */

BOOL OnCreate(HWND hwnd, LPCREATESTRUCT lpCreateStruct);
void OnDestroy(HWND hwnd);
void OnSize(HWND hwnd, UINT state, int cx, int cy);
void OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify);



/*На удаление через какое-то время*/
		/*Для ожидания завершения процесса*/
		BOOL WaitProcessById(DWORD PID, DWORD WAITTIME, LPDWORD lpExitCode);

		/*Диалоговое окно для изменения приоритета процесса*/
		INT_PTR CALLBACK DialProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);// процедура диалогового окна

		BOOL Dialog_InitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam);//инициализация диалогового окна

		void Dialog_Close(HWND hwnd);//закрытие диалогового окна
		void Dialog_Command(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify);//когда надо изменить приоритет
		
		void ToLB_LoadModules(HWND hwndCtl, DWORD PID);//перечисление модулей в LB

		void ToLB_LoadProcessesInJob(HWND hwndCtl, HANDLE hJob = NULL);//  процессы в задании

		// функция, запускающая группу процессов в одном задании
		BOOL StartGroupProcessesAsJob(HANDLE hJob, LPCTSTR lpszCmdLine[], DWORD nCount, BOOL bInheritHandles, DWORD CreationFlags);
		// функция, которая возвращает список идентификаторов включенных в задание процессов
		BOOL EnumProcessesInJob(HANDLE hJob, DWORD* lpPID, DWORD cb, LPDWORD lpcbNeeded);

/*На удаление через какое-то время*/
// ------------------------------------------------------------------------------------------------

		/*для токенов*/

		HANDLE	OpenProcessTokenPID(UINT PID, DWORD AccessRIGHT);//открыть процесс по его PID
		BOOL	GetAccountName_W(PSID psid, LPWSTR* AccountName);//получить имя пользователя по его SID

		BOOL	GetTokenUser(HANDLE token, PSID* psid);//узнать маркер доступа пользователя по его SID
		BOOL	GetTokenGR(HANDLE token, PTOKEN_GROUPS *tk_groups);//узнать список ГРУПП по маркеру доступа
		BOOL	GetTokenPR(HANDLE token, PTOKEN_PRIVILEGES *tk_PR);//узнать список ПРИВИЛЕГИЙ по маркеру доступа
		/*Загрузка процессов и модулей этих процессов в LISTBOX*/
		void	ToLB_LoadProcesses(HWND hwndCtl);//процессы в LB
		void	ToLB_LoadTokenGroup(HWND hwndListGroups, HANDLE token);//группы привелегий в LB
		void	ToLB_LoadTokenPrivil(HWND hwndListPrivileges, HANDLE token);
// ------------------------------------------------------------------------------------------------


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


	// создаем задание
	hJob = CreateJobObject(NULL, TEXT("FirstJob"));

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

	CloseHandle(hJob);// закрываем дескриптор задания
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
		420, 80, 400, 20, hwnd, NULL, lpCreateStruct->hInstance, NULL);

	CreateWindowEx(0, TEXT("ListBox"), TEXT(""), WS_CHILD | WS_VISIBLE | WS_HSCROLL | WS_VSCROLL | WS_BORDER,
		420, 30, 400, 400, hwnd, (HMENU)IDC_LB_GROUPS, lpCreateStruct->hInstance, NULL);

	//Поле "Пользователь"
	CreateWindowEx(0, TEXT("Static"), TEXT("Пользователь:"), WS_CHILD | WS_VISIBLE | SS_SIMPLE,
		420, 30, 110, 20, hwnd, NULL, lpCreateStruct->hInstance, NULL);

	CreateWindowEx(0, TEXT("Edit"), TEXT(""), WS_CHILD | WS_VISIBLE | ES_LEFT | ES_READONLY | ES_AUTOHSCROLL,
		570, 30, 400, 20, hwnd, (HMENU)IDC_EDIT_ACCOUNT, lpCreateStruct->hInstance, NULL);

	// Поле "SID"
	CreateWindowEx(0, TEXT("Static"), TEXT("SID:"), WS_CHILD | WS_VISIBLE | SS_SIMPLE,
		420, 50, 110, 20, hwnd, NULL, lpCreateStruct->hInstance, NULL);

	CreateWindowEx(0, TEXT("Edit"), TEXT(""), WS_CHILD | WS_VISIBLE | ES_LEFT | ES_READONLY | ES_AUTOHSCROLL,
		570, 50, 400, 20, hwnd, (HMENU)IDC_EDIT_SID, lpCreateStruct->hInstance, NULL);

	//список для перечисления привилегий
	CreateWindowEx(0, TEXT("Static"), TEXT("Привилегии:"), WS_CHILD | WS_VISIBLE | SS_SIMPLE,
		420, 310, 400, 20, hwnd, NULL, lpCreateStruct->hInstance, NULL);

	CreateWindowEx(0, TEXT("ListBox"), TEXT(""), WS_CHILD | WS_VISIBLE | LBS_STANDARD,
		420, 330, 400, 200, hwnd, (HMENU)IDC_LB_PRIVILEGES, lpCreateStruct->hInstance, NULL);

	// Кнопки Вкл/Выкл
	hwndCtl = CreateWindowEx(0, TEXT("Button"), TEXT("Включить"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
		420, 535, 110, 30, hwnd, (HMENU)IDC_BUTTON_PRIVILEGE_ENABLE, lpCreateStruct->hInstance, NULL);
	EnableWindow(hwndCtl, FALSE);	// отключение "Включить"

	hwndCtl = CreateWindowEx(0, TEXT("Button"), TEXT("Выключить"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
		530, 535, 110, 30, hwnd, (HMENU)IDC_BUTTON_PRIVILEGE_DISABLE, lpCreateStruct->hInstance, NULL);
	EnableWindow(hwndCtl, FALSE);	// отключение "Выключить"

	//ComboBox для привелегий
	hwndCtl = CreateWindowEx(0, TEXT("ComboBox"), TEXT(""), WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST,
		420, 580, 400, 30, hwnd, (HMENU)IDC_PRIVILEGES, lpCreateStruct->hInstance, NULL);

	constexpr LPCTSTR Privilege[5] =
	{
		SE_LOAD_DRIVER_NAME, SE_LOCK_MEMORY_NAME,
		SE_MACHINE_ACCOUNT_NAME, SE_MANAGE_VOLUME_NAME,
		SE_PROF_SINGLE_PROCESS_NAME
	};
	for (int i = 0; i < _countof(Privilege); ++i)
	{
		int iItem = ComboBox_AddString(hwndCtl, Privilege[i]);
	}
	ComboBox_SetCurSel(hwndCtl, 0);

	// кнопка "Проверить"
	CreateWindowEx(0, TEXT("Button"), TEXT("Проверка"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
		420, 615, 110, 30, hwnd, (HMENU)IDC_PRIVILEGE_CHECK, lpCreateStruct->hInstance, NULL);

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

		// изменяем высоту списка для перечисления ГРУПП
		MoveWindow(GetDlgItem(hwnd, IDC_LB_GROUPS), 420, 30, cx - 430, cy - 40, TRUE);

		// изменяем высоту списка для перечисления загруженных модулей
		MoveWindow(GetDlgItem(hwnd, IDC_LB_GROUPS), 420, 30, cx - 430, cy - 40, TRUE);

		// изменяем ширину списка привилегий
		MoveWindow(GetDlgItem(hwnd, IDC_LB_PRIVILEGES), 420, 330, cx - 430, 200, TRUE);

		// изменяем ширину поля "Пользователь"
		MoveWindow(GetDlgItem(hwnd, IDC_EDIT_ACCOUNT), 570, 30, cx - 580, 20, TRUE);
		// изменяем ширину поля "SID"
		MoveWindow(GetDlgItem(hwnd, IDC_EDIT_SID), 570, 50, cx - 580, 20, TRUE);

		// изменяем ширину выпадающего списка "Привилегии"
		MoveWindow(GetDlgItem(hwnd, IDC_PRIVILEGES), 420, 580, cx - 430, 30, TRUE);

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

			/*Кнопки*/
			EnableWindow(GetDlgItem(hwnd, IDC_BUTTON_PRIVILEGE_ENABLE), FALSE);      // отключим кнопку "Включить"
			EnableWindow(GetDlgItem(hwnd, IDC_BUTTON_PRIVILEGE_DISABLE), FALSE);	// отключим кнопку "Выключить"

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
						
							LocalFree(psid), psid = NULL;
						}
					ToLB_LoadTokenGroup(hwndListGroups, token);
					ToLB_LoadTokenPrivil(hwndListPrivileges,token);
					CloseHandle(token);
				}
			}//if
		}//if
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
	
	/*
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

				LPTSTR priv = _tcschr(NamePrivil, TEXT(' ')); //для удаления суффикса состояния привилегии
				if (priv != NULL)
				{
					*priv = TEXT('\0');
				}

				BOOL RetRes = EnablePrivilege(token, NamePrivil, (IDC_BUTTON_PRIVILEGE_ENABLE == id) ? TRUE : FALSE);

				//
			}
		}
		
	}break; */

	/*В задании ничего не сказано про включение и выключение привилегий. Нужно лишь проверить факт ее наличия и статус. 
	А значит зачем эти кнопки? Оставим только то, что нужно по заданию*/
	
	
	}
}

#pragma region Dialog Box for change priority 
INT_PTR CALLBACK DialProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_INITDIALOG:
	{
		BOOL bRet = HANDLE_WM_INITDIALOG(hwndDlg, wParam, lParam, Dialog_InitDialog);
		return SetDlgMsgResult(hwndDlg, uMsg, bRet);
	}

	case WM_CLOSE:
		HANDLE_WM_CLOSE(hwndDlg, wParam, lParam, Dialog_Close);
		return TRUE;

	case WM_COMMAND:
		HANDLE_WM_COMMAND(hwndDlg, wParam, lParam, Dialog_Command);
		return TRUE;
	}

	return FALSE;
}

// ------------------------------------------------------------------------------------------------
BOOL Dialog_InitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam)
{
	HWND hwndCtl;
	hwndCtl = GetDlgItem(hwnd, IDC_COMBO_PRIOR_CLASS);

	/*Классы приоритета процесса*/
	constexpr LPCTSTR PriorityClassesArr[6] = {
		TEXT("Реального времени"),
		TEXT("Высокий"),
		TEXT("Выше среднего"),
		TEXT("Средний"),
		TEXT("Ниже среднего"),
		TEXT("Низкий")
	};

	constexpr UINT PriorityClasses[6] = {
		REALTIME_PRIORITY_CLASS,
		HIGH_PRIORITY_CLASS,
		ABOVE_NORMAL_PRIORITY_CLASS,
		NORMAL_PRIORITY_CLASS,
		BELOW_NORMAL_PRIORITY_CLASS,
		IDLE_PRIORITY_CLASS
	};

	UINT PriorityClass = GetPriorityClass(GetCurrentProcess());


	for (int i = 0; i < _countof(PriorityClasses); ++i)
	{
		int iItem = ComboBox_AddString(hwndCtl, PriorityClassesArr[i]);
		ComboBox_SetItemData(hwndCtl, iItem, PriorityClasses[i]);

		if (PriorityClasses[i] == PriorityClass)
		{
			ComboBox_SetCurSel(hwndCtl, iItem);
		}
	}

	hwndCtl = GetDlgItem(hwnd, IDC_COMBO_PRIOR);
	/*Относительные приоритеты потока*/
	constexpr LPCTSTR PrioritiesArr[7] = {
		TEXT("Критичный по времени"),
		TEXT("Максимальный"),
		TEXT("Выше среднего"),
		TEXT("Средний"),
		TEXT("Ниже среднего"),
		TEXT("Минимальный"),
		TEXT("Простаивающий")
	};

	constexpr UINT Priorities[7] = {
		THREAD_PRIORITY_TIME_CRITICAL,
		THREAD_PRIORITY_HIGHEST,
		THREAD_PRIORITY_ABOVE_NORMAL,
		THREAD_PRIORITY_NORMAL,
		THREAD_PRIORITY_BELOW_NORMAL,
		THREAD_PRIORITY_LOWEST,
		THREAD_PRIORITY_IDLE
	};

	UINT Priority = GetThreadPriority(GetCurrentThread());

	for (int i = 0; i < _countof(Priorities); ++i)
	{
		int SelItem = ComboBox_AddString(hwndCtl, PrioritiesArr[i]);
		ComboBox_SetItemData(hwndCtl, SelItem, Priorities[i]);

		if (Priorities[i] == Priority)
		{
			ComboBox_SetCurSel(hwndCtl, SelItem);
		}
	}

	return TRUE;
}


void Dialog_Close(HWND hwnd)
{
	EndDialog(hwnd, IDCLOSE);
}

void Dialog_Command(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify)
{
	switch (id)
	{
	case IDOK:
	{
		HWND hwndCtl;
		int SelItem;

		hwndCtl = GetDlgItem(hwnd, IDC_COMBO_PRIOR_CLASS);

		/*ПОЛУЧЕНИЕ КЛАССА ПРИОРИТЕТА*/

		SelItem = ComboBox_GetCurSel(hwndCtl);
		UINT PriorityClass = (SelItem != -1) ? (UINT)ComboBox_GetItemData(hwndCtl, SelItem) : NORMAL_PRIORITY_CLASS;

		/*ЗАДАНИЕ КЛАССА ПРИОРИТЕТА*/

		SetPriorityClass(GetCurrentProcess(), PriorityClass);

		/*ИЗМЕНЕНИЕ ОТНОСИТЕЛЬНОГО ПРИОРИТЕТА ДЛЯ ГЛАВНОГО ПОТОКА */

		hwndCtl = GetDlgItem(hwnd, IDC_COMBO_PRIOR);

		/*ПОЛУЧЕНИЕ ОТНОСИТЕЛЬНОГО ПРИОРИТЕТА ДЛЯ ГЛАВНОГО ПОТОКА*/
		SelItem = ComboBox_GetCurSel(hwndCtl);
		UINT Priority;
		if (SelItem != -1)
			Priority = (UINT)ComboBox_GetItemData(hwndCtl, SelItem);
		else Priority = THREAD_PRIORITY_NORMAL;

		//UINT Priority = (SelItem != -1) ? (UINT)ComboBox_GetItemData(hwndCtl, SelItem) : THREAD_PRIORITY_NORMAL; //тернарная операция

		/*ЗАДАНИЕ ОТНОСИТЕЛЬНОГО ПРИОРИТЕТА*/
		SetThreadPriority(GetCurrentThread(), Priority);

		EndDialog(hwnd, IDOK);
	}
	break;

	case IDCANCEL:
		EndDialog(hwnd, IDCANCEL);
		break;
	}
}
#pragma endregion



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

void ToLB_LoadModules(HWND hwndCtl, DWORD PID)
{
	ListBox_ResetContent(hwndCtl);

	HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, PID);

	if (NULL != hProcess)
	{
		/*определение размера списка модулей*/
		DWORD cMod = 0;
		EnumProcessModulesEx(hProcess, NULL, 0, &cMod, LIST_MODULES_ALL);

		DWORD NcMod = cMod / sizeof(HMODULE);//столько будет модулей

		HMODULE *hModule = new HMODULE[NcMod];//столько потребуется памяти

		// получаем список модулей
		cMod = NcMod * sizeof(HMODULE);
		BOOL bRet = EnumProcessModulesEx(hProcess, hModule, cMod, &cMod, LIST_MODULES_ALL);

		if (FALSE != bRet)
		{
			TCHAR ModuleName[MAX_PATH];

			for (DWORD i = 0; i < NcMod; ++i)
			{
				bRet = GetModuleFileNameEx(hProcess, hModule[i], ModuleName, MAX_PATH);
				if (FALSE != bRet) ListBox_AddString(hwndCtl, ModuleName); // добавляем в список новую строку
			}
		}

		/*Освобождение ресурсов*/
		delete[]hModule;
		CloseHandle(hProcess);
	}
}

void ToLB_LoadProcessesInJob(HWND hwndCtl, HANDLE hJob)
{

	ListBox_ResetContent(hwndCtl);

	DWORD PIDarray[1024]; //массив для всех идентификаторов процессов
	DWORD cbNeeded = 0;//размер блока памяти с идентификаторами
	BOOL bRet = EnumProcessesInJob(hJob, PIDarray, sizeof(PIDarray), &cbNeeded);

	if (FALSE != bRet)
	{
		TCHAR ProcessName[MAX_PATH];//имя процесса
		TCHAR szBuffer[300];//строка в которую запишем имя процесса и его номер

		for (DWORD i = 0,
			n = cbNeeded / sizeof(DWORD); i < n; ++i)
		{
			DWORD PID = PIDarray[i], cch = 0;

			HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, PID);

			if (NULL != hProcess)
			{

				cch = GetModuleBaseName(hProcess, NULL, ProcessName, _countof(ProcessName));// получаем имя главного модуля процесса
				CloseHandle(hProcess); // закрываем объект ядра
			} // if

			if (0 == cch)
				StringCchCopy(ProcessName, MAX_PATH, TEXT("Неизвестный процесс"));

			StringCchPrintf(szBuffer, _countof(szBuffer), TEXT("%s (PID: %u)"), ProcessName, PID);// формируем строку для списка

			int iItem = ListBox_AddString(hwndCtl, szBuffer);

			ListBox_SetItemData(hwndCtl, iItem, PID);// сохраняем в новой строке идентификатор процесса
		}
	}
}


BOOL EnumProcessesInJob(HANDLE hJob, DWORD* lpPID, DWORD cb, LPDWORD lpcbNeeded)
{

	DWORD nCount = cb / sizeof(ULONG_PTR);//столько max процессов будет в буфере

	if (NULL != lpPID && nCount > 0)
	{
		DWORD jobProcessStructSize = sizeof(JOBOBJECT_BASIC_PROCESS_ID_LIST) + sizeof(ULONG_PTR) * 1024;//блок памяти для структуры
		JOBOBJECT_BASIC_PROCESS_ID_LIST* jobProcessIdList = static_cast<JOBOBJECT_BASIC_PROCESS_ID_LIST*>(malloc(jobProcessStructSize));

		if (NULL != jobProcessIdList)
		{
			jobProcessIdList->NumberOfAssignedProcesses = nCount;// MAX число процессов, на которое расчитана выделенная память
			// запрашиваем список идентификаторов процессов
			BOOL bRet = QueryInformationJobObject(hJob, JobObjectBasicProcessIdList, jobProcessIdList, jobProcessStructSize, NULL);

			if (FALSE != bRet)
			{
				nCount = jobProcessIdList->NumberOfProcessIdsInList;// определяем количество идетификаторов
				CopyMemory(lpPID, jobProcessIdList->ProcessIdList, nCount * sizeof(ULONG_PTR));//copies a block of memory from one location to another

				if (NULL != lpcbNeeded)
					*lpcbNeeded = nCount * sizeof(ULONG_PTR);//размер блока памяти с идентификаторами
			}

			free(jobProcessIdList); // освобождаем память
			return bRet;
		}
	}

	return FALSE;
}

BOOL StartGroupProcessesAsJob(HANDLE hjob, LPCTSTR lpszCmdLine[], DWORD nCount, BOOL bInheritHandles, DWORD CreationFlags)
{

	BOOL bInJob = FALSE;
	IsProcessInJob(GetCurrentProcess(), NULL, &bInJob);// определим, включен ли вызывающий процесс в задание

	JOBOBJECT_BASIC_LIMIT_INFORMATION jobli = { 0 };//базовые ограничения

	QueryInformationJobObject(NULL, JobObjectBasicLimitInformation, &jobli, sizeof(jobli), NULL);
	JOBOBJECT_BASIC_UI_RESTRICTIONS jobuir;
	//процессу запрещено чтение из буфера обмена
	jobuir.UIRestrictionsClass |= JOB_OBJECT_UILIMIT_READCLIPBOARD;

	SetInformationJobObject(hjob, JobObjectBasicUIRestrictions, &jobuir, sizeof(jobuir));

	//ограничения если вызывающий процесс связан с заданием
	//первый флаг - сообщаем что новый процесс может выполнятся вне задания
	//второй флаг - сообщаем, что новый процесс не является частью задания

	DWORD dwLimitMask = JOB_OBJECT_LIMIT_BREAKAWAY_OK | JOB_OBJECT_LIMIT_SILENT_BREAKAWAY_OK;

	if ((jobli.LimitFlags & dwLimitMask) == 0)
	{
		/* все порожденные процессы   автоматически включаются в задание */
		return FALSE;
	}

	// порождаем процессы...
	STARTUPINFO si = { sizeof(STARTUPINFO) };
	PROCESS_INFORMATION pi = { 0 };
	TCHAR szCmdLine[MAX_PATH];


	for (DWORD i = 0; i < nCount; ++i)
	{
		StringCchCopy(szCmdLine, MAX_PATH, lpszCmdLine[i]);
		// порождаем новый процесс,
		// приостанавливая работу его главного потока
		BOOL bRet = CreateProcess(NULL, szCmdLine, NULL, NULL,
			bInheritHandles, CreationFlags | CREATE_SUSPENDED | CREATE_BREAKAWAY_FROM_JOB, NULL, NULL, &si, &pi);

		if (FALSE != bRet)
		{
			//Дочерние процессы, порождаемые этим процессом станут частью этого задания автоматически
			AssignProcessToJobObject(hjob, pi.hProcess);// добавляем новый процесс в задание

			ResumeThread(pi.hThread);// возобновляем работу потока нового процесса

			CloseHandle(pi.hThread);// закрывает дескриптор потока нового процесса

			CloseHandle(pi.hProcess);// закрывает дескриптор нового процесса
		}
	}
	return TRUE;
}

BOOL WaitProcessById(DWORD PID, DWORD WAITTIME, LPDWORD lpExitCode)
{
	HANDLE hProcess = OpenProcess(SYNCHRONIZE | PROCESS_QUERY_INFORMATION, FALSE, PID);// открываем процесс

	if (NULL == hProcess)
	{
		return FALSE;
	}

	WaitForSingleObject(hProcess, WAITTIME);// ожидаем завершения процесса

	if (NULL != lpExitCode)
	{
		GetExitCodeProcess(hProcess, lpExitCode);// получим код завершения процесса
	}

	CloseHandle(hProcess);// закрываем дескриптор процесса

	return TRUE;
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
	DWORD cb = (cch + cchRefDomainName) * sizeof(WCHAR);
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
				std::wcscpy(Name, Name + 1);//копирование для вовзрата в основую функциию
			}
		}
		*AccountName = Name; //вернем полученнное имя в программу
	}
	else
	{
		ConvertSidToStringSidW(psid, AccountName);//если не получилось получить имя, то вернем SID
		if (Name != NULL)
		{
			LocalFree(Name);
		}
	}
	return RetRes;
}


/*Load's*/
void ToLB_LoadTokenGroup(HWND hwndListGroups, HANDLE token)
{
	ListBox_ResetContent(hwndListGroups);
	TOKEN_GROUPS* tk_groups = NULL;

	if (GetTokenGR(token, &tk_groups) != FALSE)
	{
		for (int i = 0; i < tk_groups->GroupCount; ++i)
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
		for (int i = 0; i < tk_PR->PrivilegeCount; ++i)
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

BOOL GetTokenPR(HANDLE token, PTOKEN_PRIVILEGES *tk_PR)
{
	DWORD tkSize;
	BOOL RetRes = FALSE;
	GetTokenInformation(token, TokenPrivileges, NULL, 0, &tkSize);//определение размера блока памяти

	PTOKEN_PRIVILEGES privil = (PTOKEN_PRIVILEGES)LocalAlloc(LPTR, tkSize);

	if (privil == NULL) return FALSE;

	if (GetTokenInformation(token, TokenGroups, privil, tkSize, &tkSize))//получение списка привелегий в маркере доступа
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