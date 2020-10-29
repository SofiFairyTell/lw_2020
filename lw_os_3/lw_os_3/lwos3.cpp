#include <Windows.h>
#include <WindowsX.h>
#include <tchar.h>
#include <strsafe.h>
#include "DialogWork.h"
#include "resource.h"
#include <string>
#include <Psapi.h> // для GetModuleBaseName 

#define IDC_LB_PROCESSES        2001
#define IDC_LB_MODULES          2002

HANDLE hJob = NULL; // дескриптор задания
// оконная процедура главного окна
LRESULT CALLBACK MainWindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
/*Обработчики сообщений WM_CREATE WM_DESTROY WM_SIZE WM_COMMAND */

BOOL OnCreate(HWND hwnd, LPCREATESTRUCT lpCreateStruct);
void OnDestroy(HWND hwnd);
void OnSize(HWND hwnd, UINT state, int cx, int cy);
void OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify);

/*Для ожидания завершения процесса*/
BOOL WaitProcessById(DWORD PID, DWORD WAITTIME, LPDWORD lpExitCode);

/*Диалоговое окно для изменения приоритета процесса*/
INT_PTR CALLBACK DialProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);// процедура диалогового окна

BOOL Dialog_InitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam);//инициализация диалогового окна

void Dialog_Close(HWND hwnd);//закрытие диалогового окна
void Dialog_Command(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify);//когда надо изменить приоритет

/*Загрузка процессов и модулей этих процессов в LISTBOX*/
void ToLB_LoadProcesses(HWND hwndCtl);//процессы в LB

void ToLB_LoadModules(HWND hwndCtl, DWORD PID);//перечисление модулей в LB

void ToLB_LoadProcessesInJob(HWND hwndCtl, HANDLE hJob = NULL);//  процессы в задании

// функция, запускающая группу процессов в одном задании
BOOL StartGroupProcessesAsJob(HANDLE hJob, LPCTSTR lpszCmdLine[], DWORD nCount, BOOL bInheritHandles, DWORD CreationFlags);
// функция, которая возвращает список идентификаторов включенных в задание процессов
BOOL EnumProcessesInJob(HANDLE hJob, DWORD* lpPID, DWORD cb, LPDWORD lpcbNeeded);

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
	CreateWindowEx(0, TEXT("Static"), TEXT("Процессы:"), WS_CHILD | WS_VISIBLE | SS_SIMPLE,
		10, 10, 400, 20, hwnd, NULL, lpCreateStruct->hInstance, NULL);

	CreateWindowEx(0, TEXT("Static"), TEXT("Модули процессов:"), WS_CHILD | WS_VISIBLE | SS_SIMPLE,
		420, 10, 400, 20, hwnd, NULL, lpCreateStruct->hInstance, NULL);

	// создаем список для перечисления процессов
	HWND hwndCtl = CreateWindowEx(0, TEXT("ListBox"), TEXT(""), WS_CHILD | WS_VISIBLE | LBS_STANDARD,
		10, 30, 400, 400, hwnd, (HMENU)IDC_LB_PROCESSES, lpCreateStruct->hInstance, NULL);

	// получаем список процессов активных сейчас
	ToLB_LoadProcesses(hwndCtl);

	// создаем список для перечисления загруженных модулей
	CreateWindowEx(0, TEXT("ListBox"), TEXT(""), WS_CHILD | WS_VISIBLE | WS_HSCROLL | WS_VSCROLL | WS_BORDER,
		420, 30, 400, 400, hwnd, (HMENU)IDC_LB_MODULES, lpCreateStruct->hInstance, NULL);

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

		// изменяем высоту списка для перечисления загруженных модулей
		MoveWindow(GetDlgItem(hwnd, IDC_LB_MODULES), 420, 30, cx - 430, cy - 40, TRUE);
	} // if
} // OnSize

void OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify)
{
	switch (id)
	{
	case IDC_LB_PROCESSES:
	{
		/*При выборе строки из списка процессов, для процесса определяются модули*/
		if (LBN_SELCHANGE == codeNotify) // выбран другой элемент в списке процессов
		{
				int iItem = ListBox_GetCurSel(hwndCtl);//выделенная строка

		if (iItem != -1)
			{		
			UINT PID = (UINT)ListBox_GetItemData(hwndCtl, iItem);// определение ID процесса 	
			ToLB_LoadModules(GetDlgItem(hwnd, IDC_LB_MODULES), PID);// получаем список загруженных модулей
			} 
		} 
	}
		break;

	case ID_RELOADPROCESS: // Обновление списка процессов
	{
		HWND hwndList = GetDlgItem(hwnd, IDC_LB_PROCESSES);

		ToLB_LoadProcesses(hwndList);// получаем список процессов
		
		ListBox_ResetContent(GetDlgItem(hwnd, IDC_LB_MODULES));// очистим список модулей
	}
	break;
	/*Программа приостановит свою работу пока не завершит процесс. 
	Если завершение не произойдет по истечению заданного времени, то вернет сообщение об ошибке*/
	case ID_WAITPROCCESS: // Ожидание процесса
	{
		UINT Milliseconds = INFINITE;

		HWND hwndList = GetDlgItem(hwnd, IDC_LB_PROCESSES);

		// определяем индекс выбранного элемента в списке процессов
		int iItem = ListBox_GetCurSel(hwndList);

		if (iItem != -1)
		{
			TCHAR Text[] = TEXT("Программа приостановит работу до завершения выбранного процесса если нажмете ДА\n Программа приостановит работу до истечения 10 секунд если нажмете НЕТ\n Нажмите ОТМЕНА для выхода");

			int mes = MessageBox(hwnd, Text, TEXT("Ожидание завершения процесса"), MB_ICONQUESTION | MB_YESNOCANCEL | MB_DEFBUTTON1);

			switch (mes)
			{
			case IDYES: 
				Milliseconds = INFINITE;
				break;

			case IDNO: 
				Milliseconds = 10000;
				break;

			case IDCANCEL: 
				iItem = -1;
				break;
			} 
		} 

		if (iItem != -1)
		{
			UINT PID = (DWORD)ListBox_GetItemData(hwndList, iItem); // определяем идентификатор процесса
			
			DWORD ExitCode;// ожидаем завершения работы процесса
			BOOL bRet = WaitProcessById(PID, Milliseconds, &ExitCode);// ожидаем завершения работы процесса

			if ((FALSE != bRet) && (STILL_ACTIVE != ExitCode)) // если процесс был завершен
			{
				MessageBox(hwnd, TEXT("Процесс завершен"), TEXT("Ожидание завершения процесса"), MB_ICONINFORMATION | MB_OK);

				/*После заврешения процесса удалить его и его модули из списка*/
				ListBox_DeleteString(hwndList, iItem);
				ListBox_ResetContent(GetDlgItem(hwnd, IDC_LB_MODULES));
			} 
			else if (FALSE != bRet) // если истекло время ожидания
			{
				MessageBox(hwnd, TEXT("Истекло время ожидания"), TEXT("Ожидание завершения процесса"), MB_ICONWARNING | MB_OK);
			} 
			else
			{
				MessageBox(hwnd, TEXT("Возникла ошибка"), NULL, MB_ICONERROR | MB_OK);
			} 
		} 
	}
	break;
	/*Завершение без приостановки работы*/
	case ID_ENDPROCESS: // Завершение процесса
	{
		HWND hwndLB = GetDlgItem(hwnd, IDC_LB_PROCESSES);//дескпритор списка с процессами
				
		int SelItem = ListBox_GetCurSel(hwndLB);//  индекс выбранного элемента 

		if (SelItem != -1)
		{
			int mess = MessageBox(hwnd, TEXT("Завершить процесс?"), TEXT("Завершение процесса"), MB_ICONQUESTION | MB_YESNO | MB_DEFBUTTON2);
			if (IDYES != mess) SelItem = -1;
		} 

		if (SelItem != -1)
		{
			
			UINT PID = (DWORD)ListBox_GetItemData(hwndLB, SelItem);// определяем идентификатор процесса
			HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, FALSE, PID);
			BOOL bRet = FALSE;

			if (NULL != hProcess)
			{
				bRet = TerminateProcess(hProcess, 0); // завершаем процесс
				CloseHandle(hProcess);
			} 

			if (FALSE != bRet)
			{
				MessageBox(hwnd, TEXT("Процесс завершен"), TEXT("Завершение процесса"), MB_ICONINFORMATION | MB_OK);
				/*После завершения процесса удалить его и его модули из списка*/
				ListBox_DeleteString(hwndLB, SelItem);
				ListBox_ResetContent(GetDlgItem(hwnd, IDC_LB_MODULES));
			} 
			else
			{
				MessageBox(hwnd, TEXT("Неудалось завершить процесс"), TEXT("Завершение процесса"), MB_ICONWARNING | MB_OK);
			} 
		} 
	}
	break;
	
	case ID_NEWPROCESS: // Создание новых процессов в задании
	{
		TCHAR szFileName[1024] = TEXT("");
		OPENFILENAME ofn = { sizeof(OPENFILENAME) };

		ofn.hwndOwner = hwnd;
		ofn.hInstance = GetWindowInstance(hwnd);
		ofn.lpstrFilter = TEXT("Программы (*.exe)\0*.exe\0");
		ofn.lpstrFile = szFileName;
		ofn.nMaxFile = _countof(szFileName);
		ofn.lpstrTitle = TEXT("Запустить программу");
		ofn.Flags = OFN_EXPLORER | OFN_ENABLESIZING | OFN_FILEMUSTEXIST | OFN_ALLOWMULTISELECT;
		ofn.lpstrDefExt = TEXT("exe");

		if (GetOpenFileName(&ofn) == TRUE) 
		{
			BOOL bRet = FALSE;

			//(?Кажется не работает)
			UINT FileCount = 0;// определяем количество выбранных файлов
			LPCTSTR filename = ofn.lpstrFile;
			while ((*filename)!= 0)
			{
				filename += _tcslen(filename) + 1;
				++FileCount;
			}

			if (FileCount-- > 1) // если выбрано несколько файлов
			{
				LPCTSTR lpszName = szFileName + _tcslen(szFileName) + 1;
				LPTSTR *aCmdLine = new LPTSTR[FileCount];// создаём массив строк для нескольких файлов

				for (UINT i = 0; i < FileCount; ++i)
				{
					
					aCmdLine[i] = new TCHAR[MAX_PATH];// выделяем память для командной строки
									
					StringCchPrintf(aCmdLine[i], MAX_PATH, TEXT("%s\\%s"), szFileName, lpszName);// формируем командную строку

					lpszName += _tcslen(lpszName) + 1;
				} // for

				
				bRet = StartGroupProcessesAsJob(hJob, (LPCTSTR *)aCmdLine, FileCount, FALSE, 0);// создаём группу процессов в одном задании

				/* освобождаем выделенную память*/
				for (UINT i = 0; i < FileCount; ++i) 
					delete[] aCmdLine[i];
				delete[] aCmdLine;
			} 
			else
			{
				LPCTSTR aCmdLine[1] = { szFileName };
				
				bRet = StartGroupProcessesAsJob(hJob, aCmdLine, 1, FALSE, 0);// создаём процессы в одном задании
			} 

			if ( FALSE != bRet)
			{
				HWND hwndList = GetDlgItem(hwnd, IDC_LB_PROCESSES);
				
				ToLB_LoadProcessesInJob(hwndList, hJob);// получаем список процессов в созданном задании
				
				ListBox_ResetContent(GetDlgItem(hwnd, IDC_LB_MODULES));// очистим список модулей
			} 
			else
			{
				MessageBox(hwnd, TEXT("Ошибка"), NULL, MB_ICONERROR | MB_OK);
			}
		} 
	}
	break;
	
	case ID_CURRENT_WORKING_PROCESS : // Процессы в текущем задании
	{
		HWND hwndList = GetDlgItem(hwnd, IDC_LB_PROCESSES);
		
		ToLB_LoadProcessesInJob(hwndList);// получаем список процессов в текущем задании

		ListBox_ResetContent(GetDlgItem(hwnd, IDC_LB_MODULES));
	}
	break;


	case ID_GROUP_PROCESS_IN_TASK: // Процессы, сгрупированные в задание
	{
		HWND hwndList = GetDlgItem(hwnd, IDC_LB_PROCESSES);

	
		ToLB_LoadProcessesInJob(hwndList, hJob);	// получаем список процессов в созданном задании
	
		ListBox_ResetContent(GetDlgItem(hwnd, IDC_LB_MODULES));	// очистим список модулей
	}
	break;
	
	case ID_PRIORCHANGE: // Изменение приоритета
	{
		/*Создание диалогового окна для изменения приоритета с использование HANDLE от приложения*/
		HINSTANCE hInstance = GetWindowInstance(hwnd);// получим дескриптор экземпляра приложения
		
		DialogBox(hInstance, MAKEINTRESOURCE(IDD_DIALOG1), hwnd, DialProc);
	}
	break; 
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

	//HWND hwndPriorityClass = GetDlgItem(hwnd, IDC_COMBO_PRIOR_CLASS);

	//ComboBox_AddString(hwndPriorityClass, TEXT("No limit"));
	//ComboBox_AddString(hwndPriorityClass, TEXT("Низкий"));
	//ComboBox_AddString(hwndPriorityClass, TEXT("Ниже среднего"));
	//ComboBox_AddString(hwndPriorityClass, TEXT("Средний"));
	//ComboBox_AddString(hwndPriorityClass, TEXT("Выше среднего"));
	//ComboBox_AddString(hwndPriorityClass, TEXT("Высокий"));
	//ComboBox_AddString(hwndPriorityClass, TEXT("Реального времени"));
	//ComboBox_SetCurSel(hwndPriorityClass, 3); // Default to "Средний"
	   	  

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