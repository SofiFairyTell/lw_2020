#include <Windows.h>
#include <WindowsX.h>
#include <tchar.h>
#include <strsafe.h>
#include "DialogWork.h"
#include "resource.h"
#include <Psapi.h> // для GetModuleBaseName 

#define IDC_LIST_PROCESSES        2001
#define IDC_LIST_MODULES          2002

HANDLE hSampleJob = NULL; // дескриптор задания

// оконная процедура главного окна
LRESULT CALLBACK MainWindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

// обработчик сообщения WM_CREATE
BOOL OnCreate(HWND hwnd, LPCREATESTRUCT lpCreateStruct);
// обработчик сообщения WM_DESTROY
void OnDestroy(HWND hwnd);
// обработчик сообщения WM_SIZE
void OnSize(HWND hwnd, UINT state, int cx, int cy);
// обработчик сообщения WM_COMMAND
void OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify);


// процедура диалогового окна
INT_PTR CALLBACK DialProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);

// обработчик сообщения WM_INITDIALOG
BOOL Dialog_InitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam);
// обработчик сообщения WM_CLOSE
void Dialog_Close(HWND hwnd);
// обработчик сообщения WM_COMMAND

void Dialog_Command(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify);

// функция, перечисляющая процессы системы
void LoadProcessesToListBox(HWND hwndCtl);
// функция, перечисляющая загруженный модули
void LoadModulesToListBox(HWND hwndCtl, DWORD dwProcessId);
// функция, перечисляющая процессы в задании
void LoadProcessesInJobToListBox(HWND hwndCtl, HANDLE hJob = NULL);

// функция, ожидающая завершения процесса
BOOL WaitProcessById(DWORD dwProcessId, DWORD dwMilliseconds, LPDWORD lpExitCode);
// функция, запускающая группу процессов в одном задании
BOOL StartGroupProcessesAsJob(HANDLE hJob, LPCTSTR lpszCmdLine[], DWORD nCount, BOOL bInheritHandles, DWORD dwCreationFlags);
// функция, которая возвращает список идентификаторов включенных в задание процессов
BOOL EnumProcessesInJob(HANDLE hJob, DWORD* lpidProcess, DWORD cb, LPDWORD lpcbNeeded);

// ------------------------------------------------------------------------------------------------
int WINAPI _tWinMain(HINSTANCE hInstance, HINSTANCE, LPTSTR lpszCmdLine, int nCmdShow)
{
	// регистрируем оконный класс главного окна...

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

	// загружаем таблицу быстрых клавиш
	HACCEL hAccel = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDR_ACCELERATOR1));

	// создаем главное окно на основе нового оконного класса

	HWND hWnd = CreateWindowEx(0, TEXT("MainWindowClass"), TEXT("Process"), WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

	if (NULL == hWnd)
	{
		// не удалось создать главное окно
		return -1; // завершаем работу приложения
	} // if

	
	// создаем задание
	hSampleJob = CreateJobObject(NULL, TEXT("FirstJob"));

	ShowWindow(hWnd, nCmdShow); // отображаем главное окно

	MSG  msg;
	BOOL bRet;

	while ((bRet = GetMessage(&msg, NULL, 0, 0)) != FALSE)
	{
		if (bRet == -1)
		{
			
		} // if
		else if (!TranslateAccelerator(hWnd, hAccel, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		} // if
	} // while

	// закрываем дескриптор задания
	CloseHandle(hSampleJob);

	return (int)msg.wParam;
} 

// ------------------------------------------------------------------------------------------------
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

	CreateWindowEx(0, TEXT("Static"), TEXT("Модули:"), WS_CHILD | WS_VISIBLE | SS_SIMPLE,
		420, 10, 400, 20, hwnd, NULL, lpCreateStruct->hInstance, NULL);

	// создаем список для перечисления процессов
	HWND hwndCtl = CreateWindowEx(0, TEXT("ListBox"), TEXT(""), WS_CHILD | WS_VISIBLE | LBS_STANDARD,
		10, 30, 400, 400, hwnd, (HMENU)IDC_LIST_PROCESSES, lpCreateStruct->hInstance, NULL);

	// получаем список процессов активных сейчас
	LoadProcessesToListBox(hwndCtl);

	// создаем список для перечисления загруженных модулей
	CreateWindowEx(0, TEXT("ListBox"), TEXT(""), WS_CHILD | WS_VISIBLE | WS_HSCROLL | WS_VSCROLL | WS_BORDER,
		420, 30, 400, 400, hwnd, (HMENU)IDC_LIST_MODULES, lpCreateStruct->hInstance, NULL);

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
		MoveWindow(GetDlgItem(hwnd, IDC_LIST_PROCESSES), 10, 30, 400, cy - 40, TRUE);

		// изменяем высоту списка для перечисления загруженных модулей
		MoveWindow(GetDlgItem(hwnd, IDC_LIST_MODULES), 420, 30, cx - 430, cy - 40, TRUE);
	} // if
} // OnSize

void OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify)
{
	switch (id)
	{
	case IDC_LIST_PROCESSES:
	{
		/*При выборе строки из списка процессов, для процесса определяются модули*/
		if (LBN_SELCHANGE == codeNotify) // выбран другой элемент в списке процессов
		{
	
		int iItem = ListBox_GetCurSel(hwndCtl);//выделенная строка

		if (iItem != -1)
			{
			
			DWORD dwProcessId = (DWORD)ListBox_GetItemData(hwndCtl, iItem);// определяем идентификатор 
		
			LoadModulesToListBox(GetDlgItem(hwnd, IDC_LIST_MODULES), dwProcessId);// получаем список загруженных модулей
			} 
		} 
	}
		break;

	case ID_RELOADPROCESS: // Обновление списка процессов
	{
		HWND hwndList = GetDlgItem(hwnd, IDC_LIST_PROCESSES);

		LoadProcessesToListBox(hwndList);// получаем список процессов
		
		ListBox_ResetContent(GetDlgItem(hwnd, IDC_LIST_MODULES));// очистим список модулей
	}
	break;
	/*
	case ID_WAITPROCCESS: // Ожидание процесса
	{
		DWORD dwMilliseconds = INFINITE;

		HWND hwndList = GetDlgItem(hwnd, IDC_LIST_PROCESSES);

		// определяем индекс выбранного элемента в списке процессов
		int iItem = ListBox_GetCurSel(hwndList);

		if (iItem != -1)
		{
			TCHAR szText[] = TEXT("* Для ожидания завершения работы процесса нажмите \"Да\".\r\n \
* Для того, ожидать завершения работы процесса в течении 30 сек. -- \"Нет\".\r\n \
* Для прекращения нажмите \"Отмена\".");

			int mr = MessageBox(hwnd, szText, TEXT("Ожидание процесса"), MB_ICONQUESTION | MB_YESNOCANCEL | MB_DEFBUTTON1);

			switch (mr)
			{
			case IDYES: // ожидание завершения работы процесса
				dwMilliseconds = INFINITE;
				break;

			case IDNO: // ожидание завершения работы процесса в течении 30 сек
				dwMilliseconds = 30000;
				break;

			case IDCANCEL: // отмена
				iItem = -1;
				break;
			} // switch
		} // if

		if (iItem != -1)
		{
			DWORD dwProcessId = (DWORD)ListBox_GetItemData(hwndList, iItem); // определяем идентификатор процесса

			// ожидаем завершения работы процесса
			DWORD dwExitCode;
			BOOL bRet = WaitProcessById(dwProcessId, dwMilliseconds, &dwExitCode);

			if ((FALSE != bRet) && (STILL_ACTIVE != dwExitCode)) // если процесс был завершен
			{
				MessageBox(hwnd, TEXT("Процесс завершен"), TEXT("Ожидание процесса"), MB_ICONINFORMATION | MB_OK);

				// удаляем процесс из списка
				ListBox_DeleteString(hwndList, iItem);
				// очистим список модулей
				ListBox_ResetContent(GetDlgItem(hwnd, IDC_LIST_MODULES));
			} // if
			else if (FALSE != bRet) // если истекло время ожидания
			{
				MessageBox(hwnd, TEXT("Истекло время ожидания"), TEXT("Ожидание процесса"), MB_ICONWARNING | MB_OK);
			} // if
			else
			{
				MessageBox(hwnd, TEXT("Возникла ошибка"), NULL, MB_ICONERROR | MB_OK);
			} // else
		} // if
	}
	break;

	case ID_ENDPROCESS: // Завершение процесса
	{
		HWND hwndList = GetDlgItem(hwnd, IDC_LIST_PROCESSES);

		// определяем индекс выбранного элемента в списке процессов
		int iItem = ListBox_GetCurSel(hwndList);

		if (iItem != -1)
		{
			int mr = MessageBox(hwnd, TEXT("Завершить процесс?"), TEXT("Завершение процесса"), MB_ICONQUESTION | MB_YESNO | MB_DEFBUTTON2);
			if (IDYES != mr) iItem = -1;
		} // if

		if (iItem != -1)
		{
			// определяем идентификатор процесса
			DWORD dwProcessId = (DWORD)ListBox_GetItemData(hwndList, iItem);
			// открываем процесс
			HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, FALSE, dwProcessId);

			BOOL bRet = FALSE;

			if (NULL != hProcess)
			{
				bRet = TerminateProcess(hProcess, 0); // завершаем процесс
				CloseHandle(hProcess);
			} // if

			if (FALSE != bRet)
			{
				MessageBox(hwnd, TEXT("Процесс завершен"), TEXT("Завершение процесса"), MB_ICONINFORMATION | MB_OK);

				// удаляем процесс из списка
				ListBox_DeleteString(hwndList, iItem);
				// очистим список модулей
				ListBox_ResetContent(GetDlgItem(hwnd, IDC_LIST_MODULES));
			} // if
			else
			{
				MessageBox(hwnd, TEXT("Неудалось завершить процесс"), TEXT("Завершение процесса"), MB_ICONWARNING | MB_OK);
			} // else
		} // if
	}
	break;
	*/
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

		if (GetOpenFileName(&ofn) != FALSE) 
		{
			BOOL bRet = FALSE;

			//(?Кажется не работает)
			UINT nCount = 0;// определяем количество выбранных файлов
			for (LPCTSTR p = szFileName; (*p) != 0; p += _tcslen(p) + 1) ++nCount;

			if (nCount-- > 1) // если выбрано несколько файлов
			{
				LPCTSTR lpszName = szFileName + _tcslen(szFileName) + 1;

				
				LPTSTR *aCmdLine = new LPTSTR[nCount];// создаём массив строк для нескольких файлов

				for (UINT i = 0; i < nCount; ++i)
				{
					
					aCmdLine[i] = new TCHAR[MAX_PATH];// выделяем память для командной строки

					
					StringCchPrintf(aCmdLine[i], MAX_PATH, TEXT("%s\\%s"), szFileName, lpszName);// формируем командную строку

					lpszName += _tcslen(lpszName) + 1;
				} // for

				
				bRet = StartGroupProcessesAsJob(hSampleJob, (LPCTSTR *)aCmdLine, nCount, FALSE, 0);// создаём группу процессов в одном задании

				// освобождаем выделенную память
				for (UINT i = 0; i < nCount; ++i) delete[] aCmdLine[i];
				// удаляем массив строк
				delete[] aCmdLine;
			} // if
			else
			{
				LPCTSTR aCmdLine[1] = { szFileName };

				// создаём процесс в одном задании
				bRet = StartGroupProcessesAsJob(hSampleJob, aCmdLine, 1, FALSE, 0);
			} // else

			if (FALSE != bRet)
			{
				HWND hwndList = GetDlgItem(hwnd, IDC_LIST_PROCESSES);

				// получаем список процессов в созданном задании
				LoadProcessesInJobToListBox(hwndList, hSampleJob);

				// очистим список модулей
				ListBox_ResetContent(GetDlgItem(hwnd, IDC_LIST_MODULES));
			} // if
			else
			{
				MessageBox(hwnd, TEXT("Возникла ошибка"), NULL, MB_ICONERROR | MB_OK);
			} // else
		} // if
	}
	break;
	
	case ID_CURRENT_WORKING_PROCESS : // Процессы в текущем задании
	{
		HWND hwndList = GetDlgItem(hwnd, IDC_LIST_PROCESSES);
		// получаем список процессов в текущем задании
		LoadProcessesInJobToListBox(hwndList);
		// очистим список модулей
		ListBox_ResetContent(GetDlgItem(hwnd, IDC_LIST_MODULES));
	}
	break;


	case ID_GROUP_PROCESS_IN_TASK: // Процессы, сгрупированные в задание
	{
		HWND hwndList = GetDlgItem(hwnd, IDC_LIST_PROCESSES);

		// получаем список процессов в созданном задании
		LoadProcessesInJobToListBox(hwndList, hSampleJob);

		// очистим список модулей
		ListBox_ResetContent(GetDlgItem(hwnd, IDC_LIST_MODULES));
	}
	break;
	
	case ID_PRIORCHANGE: // Изменение приоритета
	{
		// получим дескриптор экземпляра приложения
		HINSTANCE hInstance = GetWindowInstance(hwnd);
		// создаем модальное диалоговое окно для изменения приоритета
		DialogBox(hInstance, MAKEINTRESOURCE(IDD_DIALOG1), hwnd, DialProc);
	}
	break; 
	} // switch
} // OnCommand

#pragma region Dialog Box
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
	} // switch

	return FALSE;
} // DialogProc

// ------------------------------------------------------------------------------------------------
BOOL Dialog_InitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam)
{
	HWND hwndCtl;

	// заполняем выпадающий список "Классы приоритета процесса"
	hwndCtl = GetDlgItem(hwnd, IDC_COMBO_PRIOR_CLASS);

	constexpr LPCTSTR szPriorityClasses[6] = {
		TEXT("Реального времени"),
		TEXT("Высокий"),
		TEXT("Выше среднего"),
		TEXT("Средний"),
		TEXT("Ниже среднего"),
		TEXT("Низкий")
	};

	constexpr DWORD dwPriorityClasses[6] = {
		REALTIME_PRIORITY_CLASS,
		HIGH_PRIORITY_CLASS,
		ABOVE_NORMAL_PRIORITY_CLASS,
		NORMAL_PRIORITY_CLASS,
		BELOW_NORMAL_PRIORITY_CLASS,
		IDLE_PRIORITY_CLASS
	};

	DWORD dwPriorityClass = GetPriorityClass(GetCurrentProcess());

	for (int i = 0; i < _countof(dwPriorityClasses); ++i)
	{
		int iItem = ComboBox_AddString(hwndCtl, szPriorityClasses[i]);
		ComboBox_SetItemData(hwndCtl, iItem, dwPriorityClasses[i]);

		if (dwPriorityClasses[i] == dwPriorityClass)
		{
			ComboBox_SetCurSel(hwndCtl, iItem);
		} // if
	} // for

	// заполняем выпадающий список "Относительные приоритеты потоков"
	hwndCtl = GetDlgItem(hwnd, IDC_COMBO_PRIOR);

	constexpr LPCTSTR szPriorities[7] = {
		TEXT("Критичный по времени"),
		TEXT("Максимальный"),
		TEXT("Выше среднего"),
		TEXT("Средний"),
		TEXT("Ниже среднего"),
		TEXT("Минимальный"),
		TEXT("Простаивающий")
	};

	constexpr DWORD dwPriorities[7] = {
		THREAD_PRIORITY_TIME_CRITICAL,
		THREAD_PRIORITY_HIGHEST,
		THREAD_PRIORITY_ABOVE_NORMAL,
		THREAD_PRIORITY_NORMAL,
		THREAD_PRIORITY_BELOW_NORMAL,
		THREAD_PRIORITY_LOWEST,
		THREAD_PRIORITY_IDLE
	};

	DWORD dwPriority = GetThreadPriority(GetCurrentThread());

	for (int i = 0; i < _countof(dwPriorities); ++i)
	{
		int iItem = ComboBox_AddString(hwndCtl, szPriorities[i]);
		ComboBox_SetItemData(hwndCtl, iItem, dwPriorities[i]);

		if (dwPriorities[i] == dwPriority)
		{
			ComboBox_SetCurSel(hwndCtl, iItem);
		} // if
	} // for

	return TRUE;
} // Dialog_InitDialog

// ------------------------------------------------------------------------------------------------
void Dialog_Close(HWND hwnd)
{
	EndDialog(hwnd, IDCLOSE);
} // Dialog_Close

// ------------------------------------------------------------------------------------------------
void Dialog_Command(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify)
{
	switch (id)
	{
	case IDOK:
	{
		HWND hwndCtl;
		int iItem;

		// изменим класс приоритета

		hwndCtl = GetDlgItem(hwnd, IDC_COMBO_PRIOR_CLASS);

		// получим выбранный класс приоритета
		iItem = ComboBox_GetCurSel(hwndCtl);
		DWORD dwPriorityClass = (iItem != -1) ? (DWORD)ComboBox_GetItemData(hwndCtl, iItem) : NORMAL_PRIORITY_CLASS;

		// задаем класс приоритета
		SetPriorityClass(GetCurrentProcess(), dwPriorityClass);


		// изменим относительный приоритет для главного потока

		hwndCtl = GetDlgItem(hwnd, IDC_COMBO_PRIOR);

		// получим относительный приоритет потока
		iItem = ComboBox_GetCurSel(hwndCtl);
		DWORD dwPriority = (iItem != -1) ? (DWORD)ComboBox_GetItemData(hwndCtl, iItem) : THREAD_PRIORITY_NORMAL;

		// задаем относительный приоритет для главного потока
		SetThreadPriority(GetCurrentThread(), dwPriority);

		EndDialog(hwnd, IDOK);
	}
	break;

	case IDCANCEL:
		EndDialog(hwnd, IDCANCEL);
		break;
	} // switch
} // Dialog_Command
#pragma endregion



void LoadProcessesToListBox(HWND hwndCtl)
{
	ListBox_ResetContent(hwndCtl);//очистка списка

	DWORD aProcessIds[1024], cbNeeded = 0;//массив для ID созданных процессов
	BOOL bRet = EnumProcesses(aProcessIds, sizeof(aProcessIds), &cbNeeded);//получение списка ID созданных процессоров
	
	if (FALSE != bRet)
	{
		TCHAR szName[MAX_PATH], szBuffer[300];

		for (DWORD i = 0,
			n = cbNeeded / sizeof(DWORD); i < n; ++i)
		{
			DWORD dwProcessId = aProcessIds[i], cch = 0;
			if (0 == dwProcessId) continue;

			
			HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, dwProcessId);//получение дескриптора процесса по его ID

			if (NULL != hProcess)
			{
				cch = GetModuleBaseName(hProcess, NULL, szName, _countof(szName));// получаем имя главного модуля процесса

				CloseHandle(hProcess); // закрываем объект ядра
			} 

			if (0 == cch)
				StringCchCopy(szName, MAX_PATH, TEXT("Имя процесса не определено"));

			StringCchPrintf(szBuffer, _countof(szBuffer), TEXT("%s (PID: %u)"), szName, dwProcessId);

			
			int iItem = ListBox_AddString(hwndCtl, szBuffer);

			ListBox_SetItemData(hwndCtl, iItem, dwProcessId);//запись в ListBox имени процесса
		} // for
	} // if
} // LoadProcessesToListBox


void LoadModulesToListBox(HWND hwndCtl, DWORD dwProcessId)
{
	// удалим все строки из списка
	ListBox_ResetContent(hwndCtl);

	// открываем объект ядра "процесс"
	HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, dwProcessId);

	if (NULL != hProcess)
	{
		// определяем размер (в байтах) списка модулей
		DWORD cb = 0;
		EnumProcessModulesEx(hProcess, NULL, 0, &cb, LIST_MODULES_ALL);

		// вычисляем количество модулей
		DWORD nCount = cb / sizeof(HMODULE);

		// выделяем память для списка модулей
		HMODULE *hModule = new HMODULE[nCount];

		// получаем список модулей
		cb = nCount * sizeof(HMODULE);
		BOOL bRet = EnumProcessModulesEx(hProcess, hModule, cb, &cb, LIST_MODULES_ALL);

		if (FALSE != bRet)
		{
			TCHAR szFileName[MAX_PATH];

			for (DWORD i = 0; i < nCount; ++i)
			{
				// получаем имя загруженного модуля
				bRet = GetModuleFileNameEx(hProcess, hModule[i], szFileName, MAX_PATH);
				if (FALSE != bRet) ListBox_AddString(hwndCtl, szFileName); // добавляем в список новую строку
			} // for
		} // if

		delete[]hModule; // освобождаем память
		CloseHandle(hProcess); // закрываем объект ядра
	} // if
} // LoadModulesToListBox

void LoadProcessesInJobToListBox(HWND hwndCtl, HANDLE hJob)
{
	// удалим все строки из списка
	ListBox_ResetContent(hwndCtl);

	// получим список идентификаторов всех созданных процессов
	DWORD aProcessIds[1024], cbNeeded = 0;
	BOOL bRet = EnumProcessesInJob(hJob, aProcessIds, sizeof(aProcessIds), &cbNeeded);

	if (FALSE != bRet)
	{
		TCHAR szName[MAX_PATH], szBuffer[300];

		for (DWORD i = 0,
			n = cbNeeded / sizeof(DWORD); i < n; ++i)
		{
			DWORD dwProcessId = aProcessIds[i], cch = 0;

			// открываем объект ядра процесса
			HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, dwProcessId);

			if (NULL != hProcess)
			{
				// получаем имя главного модуля процесса
				cch = GetModuleBaseName(hProcess, NULL, szName, _countof(szName));
				CloseHandle(hProcess); // закрываем объект ядра
			} // if

			if (0 == cch)
				StringCchCopy(szName, MAX_PATH, TEXT("Неизвестный процесс"));//в каких случаях появляется?

			// формируем строку для списка
			StringCchPrintf(szBuffer, _countof(szBuffer), TEXT("%s (PID: %u)"), szName, dwProcessId);

			// добавляем в список новую строку
			int iItem = ListBox_AddString(hwndCtl, szBuffer);
			// сохраняем в новой строке идентификатор процесса
			ListBox_SetItemData(hwndCtl, iItem, dwProcessId);
		} // for
	} // if
} // LoadProcessesInJobToListBox


BOOL EnumProcessesInJob(HANDLE hJob, DWORD* lpidProcess, DWORD cb, LPDWORD lpcbNeeded)
{
	// определяем максимальное число процессов,
	// на которое расчитан буфер lpidProcess
	DWORD nCount = cb / sizeof(ULONG_PTR);

	if (NULL != lpidProcess && nCount > 0)
	{
		// определеям размер блока памяти (в байтах)
		// для хранения идентификаторов и структуры
		DWORD cbJobPIL = sizeof(JOBOBJECT_BASIC_PROCESS_ID_LIST) + (nCount - 1) * sizeof(ULONG_PTR);

		// выделяем блок памяти
		JOBOBJECT_BASIC_PROCESS_ID_LIST *pJobPIL = (JOBOBJECT_BASIC_PROCESS_ID_LIST *)malloc(cbJobPIL);

		if (NULL != pJobPIL)
		{
			// указываем максимальное число процессов,
			// на которое расчитана выделенная память
			pJobPIL->NumberOfAssignedProcesses = nCount;

			// запрашиваем список идентификаторов процессов
			BOOL bRet = QueryInformationJobObject(hJob, JobObjectBasicProcessIdList, pJobPIL, cbJobPIL, NULL);

			if (FALSE != bRet)
			{
				// определяем количество идетификаторов
				nCount = pJobPIL->NumberOfProcessIdsInList;  

				// копируем в буфер список идентификаторов
				CopyMemory(lpidProcess, pJobPIL->ProcessIdList, nCount * sizeof(ULONG_PTR));

				// возвращаем размер блока памяти (в байтах),
				// в который скопирован список идентификаторов
				if (NULL != lpcbNeeded)
					*lpcbNeeded = nCount * sizeof(ULONG_PTR);
			} // if

			free(pJobPIL); // освобождаем память
			return bRet;
		} // if
	} // if

	return FALSE;
} // EnumProcessesInJob

BOOL StartGroupProcessesAsJob(HANDLE hJob, LPCTSTR lpszCmdLine[], DWORD nCount, BOOL bInheritHandles, DWORD dwCreationFlags)
{
	// определим, включен ли вызывающий процесс в задание
	BOOL bInJob = FALSE;
	IsProcessInJob(GetCurrentProcess(), NULL, &bInJob);

	if (FALSE != bInJob) // если да (!)
	{
		// определим разрешено ли порождать процессы,
		// которые не будут принадлежать этому заданию

		JOBOBJECT_BASIC_LIMIT_INFORMATION jobli = { 0 };

		QueryInformationJobObject(NULL,
			JobObjectBasicLimitInformation, &jobli, sizeof(jobli), NULL);

		DWORD dwLimitMask = JOB_OBJECT_LIMIT_BREAKAWAY_OK | JOB_OBJECT_LIMIT_SILENT_BREAKAWAY_OK;

		if ((jobli.LimitFlags & dwLimitMask) == 0)
		{
			/* все порожденные процессы
			   автоматически включаются в задание */
			return FALSE;
		} // if
	} // if

	// порождаем процессы...

	TCHAR szCmdLine[MAX_PATH];
	STARTUPINFO si = { sizeof(STARTUPINFO) };
	PROCESS_INFORMATION pi = { 0 };

	for (DWORD i = 0; i < nCount; ++i)
	{
		StringCchCopy(szCmdLine, MAX_PATH, lpszCmdLine[i]);

		// порождаем новый процесс,
		// приостанавливая работу его главного потока
		BOOL bRet = CreateProcess(NULL, szCmdLine, NULL, NULL,
			bInheritHandles, dwCreationFlags | CREATE_SUSPENDED | CREATE_BREAKAWAY_FROM_JOB, NULL, NULL, &si, &pi);

		if (FALSE != bRet)
		{
			// добавляем новый процесс в задание
			AssignProcessToJobObject(hJob, pi.hProcess);
			// возобновляем работу потока нового процесса
			ResumeThread(pi.hThread);
			// закрывает дескриптор потока нового процесса
			CloseHandle(pi.hThread);
			// закрывает дескриптор нового процесса
			CloseHandle(pi.hProcess);
		} // if
	} // for

	return TRUE;
} // StartGroupProcessesAsJob
