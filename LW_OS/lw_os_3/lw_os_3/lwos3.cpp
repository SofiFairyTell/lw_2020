#include <Windows.h>
#include <WindowsX.h>
#include <tchar.h>
#include <strsafe.h>
#include "DialogWork.h"
#include "resource.h"
#include <string>
#include <Psapi.h> // ��� GetModuleBaseName 

#define IDC_LB_PROCESSES        2001
#define IDC_LB_MODULES          2002

HANDLE hJob = NULL; // ���������� �������
// ������� ��������� �������� ����
LRESULT CALLBACK MainWindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
/*����������� ��������� WM_CREATE WM_DESTROY WM_SIZE WM_COMMAND */

BOOL OnCreate(HWND hwnd, LPCREATESTRUCT lpCreateStruct);
void OnDestroy(HWND hwnd);
void OnSize(HWND hwnd, UINT state, int cx, int cy);
void OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify);

/*��� �������� ���������� ��������*/
BOOL WaitProcessById(DWORD PID, DWORD WAITTIME, LPDWORD lpExitCode);

/*���������� ���� ��� ��������� ���������� ��������*/
INT_PTR CALLBACK DialProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);// ��������� ����������� ����

BOOL Dialog_InitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam);//������������� ����������� ����

void Dialog_Close(HWND hwnd);//�������� ����������� ����
void Dialog_Command(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify);//����� ���� �������� ���������

/*�������� ��������� � ������� ���� ��������� � LISTBOX*/
void ToLB_LoadProcesses(HWND hwndCtl);//�������� � LB

void ToLB_LoadModules(HWND hwndCtl, DWORD PID);//������������ ������� � LB

void ToLB_LoadProcessesInJob(HWND hwndCtl, HANDLE hJob = NULL);//  �������� � �������

// �������, ����������� ������ ��������� � ����� �������
BOOL StartGroupProcessesAsJob(HANDLE hJob, LPCTSTR lpszCmdLine[], DWORD nCount, BOOL bInheritHandles, DWORD CreationFlags);
// �������, ������� ���������� ������ ��������������� ���������� � ������� ���������
BOOL EnumProcessesInJob(HANDLE hJob, DWORD* lpPID, DWORD cb, LPDWORD lpcbNeeded);

// ------------------------------------------------------------------------------------------------
int WINAPI _tWinMain(HINSTANCE hInstance, HINSTANCE, LPTSTR lpszCmdLine, int nCmdShow)
{
	LoadLibrary(TEXT("ComCtl32.dll"));//��� ��������� ������ �����������								

	WNDCLASSEX wcex = { sizeof(WNDCLASSEX) };

	wcex.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
	wcex.lpfnWndProc = MainWindowProc; // ������� ���������
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_BTNFACE + 2);
	wcex.lpszMenuName = MAKEINTRESOURCE(IDR_MENU1);
	wcex.lpszClassName = TEXT("MainWindowClass"); // ��� ������
	wcex.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
	


	if (0 == RegisterClassEx(&wcex)) // ������������ �����
	{
		return -1; // ��������� ������ ����������
	} 

	HACCEL hAccel = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDR_ACCELERATOR1));

	// ������� ������� ���� �� ������ ������ �������� ������

	HWND hWnd = CreateWindowEx(0, TEXT("MainWindowClass"), TEXT("Process"), WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

	if (NULL == hWnd)
	{
		return -1; // ��������� ������ ����������
	} 

	
	// ������� �������
	hJob = CreateJobObject(NULL, TEXT("FirstJob"));

	ShowWindow(hWnd, nCmdShow); // ���������� ������� ����

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

	CloseHandle(hJob);// ��������� ���������� �������
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
	return DefWindowProc(hWnd, uMsg, wParam, lParam);	// �������� ��������������� ���������
}

/*�������� �������� ���������� � 2-�� listbox � ����*/
BOOL OnCreate(HWND hwnd, LPCREATESTRUCT lpCreateStruct)
{
	CreateWindowEx(0, TEXT("Static"), TEXT("��������:"), WS_CHILD | WS_VISIBLE | SS_SIMPLE,
		10, 10, 400, 20, hwnd, NULL, lpCreateStruct->hInstance, NULL);

	CreateWindowEx(0, TEXT("Static"), TEXT("������ ���������:"), WS_CHILD | WS_VISIBLE | SS_SIMPLE,
		420, 10, 400, 20, hwnd, NULL, lpCreateStruct->hInstance, NULL);

	// ������� ������ ��� ������������ ���������
	HWND hwndCtl = CreateWindowEx(0, TEXT("ListBox"), TEXT(""), WS_CHILD | WS_VISIBLE | LBS_STANDARD,
		10, 30, 400, 400, hwnd, (HMENU)IDC_LB_PROCESSES, lpCreateStruct->hInstance, NULL);

	// �������� ������ ��������� �������� ������
	ToLB_LoadProcesses(hwndCtl);

	// ������� ������ ��� ������������ ����������� �������
	CreateWindowEx(0, TEXT("ListBox"), TEXT(""), WS_CHILD | WS_VISIBLE | WS_HSCROLL | WS_VSCROLL | WS_BORDER,
		420, 30, 400, 400, hwnd, (HMENU)IDC_LB_MODULES, lpCreateStruct->hInstance, NULL);

	return TRUE;
} 

/*��� ���������� ������ � �����������*/
void OnDestroy(HWND hwnd)
{
	PostQuitMessage(0); // ���������� ��������� WM_QUIT
} 

/*������ ������ ������ � ������*/
void OnSize(HWND hwnd, UINT state, int cx, int cy)
{
	if (state != SIZE_MINIMIZED)
	{
		// �������� ������ ������ ��� ������������ ���������
		MoveWindow(GetDlgItem(hwnd, IDC_LB_PROCESSES), 10, 30, 400, cy - 40, TRUE);

		// �������� ������ ������ ��� ������������ ����������� �������
		MoveWindow(GetDlgItem(hwnd, IDC_LB_MODULES), 420, 30, cx - 430, cy - 40, TRUE);
	} // if
} // OnSize

void OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify)
{
	switch (id)
	{
	case IDC_LB_PROCESSES:
	{
		/*��� ������ ������ �� ������ ���������, ��� �������� ������������ ������*/
		if (LBN_SELCHANGE == codeNotify) // ������ ������ ������� � ������ ���������
		{
				int iItem = ListBox_GetCurSel(hwndCtl);//���������� ������

		if (iItem != -1)
			{		
			UINT PID = (UINT)ListBox_GetItemData(hwndCtl, iItem);// ����������� ID �������� 	
			ToLB_LoadModules(GetDlgItem(hwnd, IDC_LB_MODULES), PID);// �������� ������ ����������� �������
			} 
		} 
	}
		break;

	case ID_RELOADPROCESS: // ���������� ������ ���������
	{
		HWND hwndList = GetDlgItem(hwnd, IDC_LB_PROCESSES);

		ToLB_LoadProcesses(hwndList);// �������� ������ ���������
		
		ListBox_ResetContent(GetDlgItem(hwnd, IDC_LB_MODULES));// ������� ������ �������
	}
	break;
	/*��������� ������������ ���� ������ ���� �� �������� �������. 
	���� ���������� �� ���������� �� ��������� ��������� �������, �� ������ ��������� �� ������*/
	case ID_WAITPROCCESS: // �������� ��������
	{
		UINT Milliseconds = INFINITE;

		HWND hwndList = GetDlgItem(hwnd, IDC_LB_PROCESSES);

		// ���������� ������ ���������� �������� � ������ ���������
		int iItem = ListBox_GetCurSel(hwndList);

		if (iItem != -1)
		{
			TCHAR Text[] = TEXT("��������� ������������ ������ �� ���������� ���������� �������� ���� ������� ��\n ��������� ������������ ������ �� ��������� 10 ������ ���� ������� ���\n ������� ������ ��� ������");

			int mes = MessageBox(hwnd, Text, TEXT("�������� ���������� ��������"), MB_ICONQUESTION | MB_YESNOCANCEL | MB_DEFBUTTON1);

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
			UINT PID = (DWORD)ListBox_GetItemData(hwndList, iItem); // ���������� ������������� ��������
			
			DWORD ExitCode;// ������� ���������� ������ ��������
			BOOL bRet = WaitProcessById(PID, Milliseconds, &ExitCode);// ������� ���������� ������ ��������

			if ((FALSE != bRet) && (STILL_ACTIVE != ExitCode)) // ���� ������� ��� ��������
			{
				MessageBox(hwnd, TEXT("������� ��������"), TEXT("�������� ���������� ��������"), MB_ICONINFORMATION | MB_OK);

				/*����� ���������� �������� ������� ��� � ��� ������ �� ������*/
				ListBox_DeleteString(hwndList, iItem);
				ListBox_ResetContent(GetDlgItem(hwnd, IDC_LB_MODULES));
			} 
			else if (FALSE != bRet) // ���� ������� ����� ��������
			{
				MessageBox(hwnd, TEXT("������� ����� ��������"), TEXT("�������� ���������� ��������"), MB_ICONWARNING | MB_OK);
			} 
			else
			{
				MessageBox(hwnd, TEXT("�������� ������"), NULL, MB_ICONERROR | MB_OK);
			} 
		} 
	}
	break;
	/*���������� ��� ������������ ������*/
	case ID_ENDPROCESS: // ���������� ��������
	{
		HWND hwndLB = GetDlgItem(hwnd, IDC_LB_PROCESSES);//���������� ������ � ����������
				
		int SelItem = ListBox_GetCurSel(hwndLB);//  ������ ���������� �������� 

		if (SelItem != -1)
		{
			int mess = MessageBox(hwnd, TEXT("��������� �������?"), TEXT("���������� ��������"), MB_ICONQUESTION | MB_YESNO | MB_DEFBUTTON2);
			if (IDYES != mess) SelItem = -1;
		} 

		if (SelItem != -1)
		{
			
			UINT PID = (DWORD)ListBox_GetItemData(hwndLB, SelItem);// ���������� ������������� ��������
			HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, FALSE, PID);
			BOOL bRet = FALSE;

			if (NULL != hProcess)
			{
				bRet = TerminateProcess(hProcess, 0); // ��������� �������
				CloseHandle(hProcess);
			} 

			if (FALSE != bRet)
			{
				MessageBox(hwnd, TEXT("������� ��������"), TEXT("���������� ��������"), MB_ICONINFORMATION | MB_OK);
				/*����� ���������� �������� ������� ��� � ��� ������ �� ������*/
				ListBox_DeleteString(hwndLB, SelItem);
				ListBox_ResetContent(GetDlgItem(hwnd, IDC_LB_MODULES));
			} 
			else
			{
				MessageBox(hwnd, TEXT("��������� ��������� �������"), TEXT("���������� ��������"), MB_ICONWARNING | MB_OK);
			} 
		} 
	}
	break;
	
	case ID_NEWPROCESS: // �������� ����� ��������� � �������
	{
		TCHAR szFileName[1024] = TEXT("");
		OPENFILENAME ofn = { sizeof(OPENFILENAME) };

		ofn.hwndOwner = hwnd;
		ofn.hInstance = GetWindowInstance(hwnd);
		ofn.lpstrFilter = TEXT("��������� (*.exe)\0*.exe\0");
		ofn.lpstrFile = szFileName;
		ofn.nMaxFile = _countof(szFileName);
		ofn.lpstrTitle = TEXT("��������� ���������");
		ofn.Flags = OFN_EXPLORER | OFN_ENABLESIZING | OFN_FILEMUSTEXIST | OFN_ALLOWMULTISELECT;
		ofn.lpstrDefExt = TEXT("exe");

		if (GetOpenFileName(&ofn) == TRUE) 
		{
			BOOL bRet = FALSE;

			//(?������� �� ��������)
			UINT FileCount = 0;// ���������� ���������� ��������� ������
			LPCTSTR filename = ofn.lpstrFile;
			while ((*filename)!= 0)
			{
				filename += _tcslen(filename) + 1;
				++FileCount;
			}

			if (FileCount-- > 1) // ���� ������� ��������� ������
			{
				LPCTSTR lpszName = szFileName + _tcslen(szFileName) + 1;
				LPTSTR *aCmdLine = new LPTSTR[FileCount];// ������ ������ ����� ��� ���������� ������

				for (UINT i = 0; i < FileCount; ++i)
				{
					
					aCmdLine[i] = new TCHAR[MAX_PATH];// �������� ������ ��� ��������� ������
									
					StringCchPrintf(aCmdLine[i], MAX_PATH, TEXT("%s\\%s"), szFileName, lpszName);// ��������� ��������� ������

					lpszName += _tcslen(lpszName) + 1;
				} // for

				
				bRet = StartGroupProcessesAsJob(hJob, (LPCTSTR *)aCmdLine, FileCount, FALSE, 0);// ������ ������ ��������� � ����� �������

				/* ����������� ���������� ������*/
				for (UINT i = 0; i < FileCount; ++i) 
					delete[] aCmdLine[i];
				delete[] aCmdLine;
			} 
			else
			{
				LPCTSTR aCmdLine[1] = { szFileName };
				
				bRet = StartGroupProcessesAsJob(hJob, aCmdLine, 1, FALSE, 0);// ������ �������� � ����� �������
			} 

			if ( FALSE != bRet)
			{
				HWND hwndList = GetDlgItem(hwnd, IDC_LB_PROCESSES);
				
				ToLB_LoadProcessesInJob(hwndList, hJob);// �������� ������ ��������� � ��������� �������
				
				ListBox_ResetContent(GetDlgItem(hwnd, IDC_LB_MODULES));// ������� ������ �������
			} 
			else
			{
				MessageBox(hwnd, TEXT("������"), NULL, MB_ICONERROR | MB_OK);
			}
		} 
	}
	break;
	
	case ID_CURRENT_WORKING_PROCESS : // �������� � ������� �������
	{
		HWND hwndList = GetDlgItem(hwnd, IDC_LB_PROCESSES);
		
		ToLB_LoadProcessesInJob(hwndList);// �������� ������ ��������� � ������� �������

		ListBox_ResetContent(GetDlgItem(hwnd, IDC_LB_MODULES));
	}
	break;


	case ID_GROUP_PROCESS_IN_TASK: // ��������, �������������� � �������
	{
		HWND hwndList = GetDlgItem(hwnd, IDC_LB_PROCESSES);

	
		ToLB_LoadProcessesInJob(hwndList, hJob);	// �������� ������ ��������� � ��������� �������
	
		ListBox_ResetContent(GetDlgItem(hwnd, IDC_LB_MODULES));	// ������� ������ �������
	}
	break;
	
	case ID_PRIORCHANGE: // ��������� ����������
	{
		/*�������� ����������� ���� ��� ��������� ���������� � ������������� HANDLE �� ����������*/
		HINSTANCE hInstance = GetWindowInstance(hwnd);// ������� ���������� ���������� ����������
		
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

	/*������ ���������� ��������*/
	constexpr LPCTSTR PriorityClassesArr[6] = {
		TEXT("��������� �������"),
		TEXT("�������"),
		TEXT("���� ��������"),
		TEXT("�������"),
		TEXT("���� ��������"),
		TEXT("������")
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
	//ComboBox_AddString(hwndPriorityClass, TEXT("������"));
	//ComboBox_AddString(hwndPriorityClass, TEXT("���� ��������"));
	//ComboBox_AddString(hwndPriorityClass, TEXT("�������"));
	//ComboBox_AddString(hwndPriorityClass, TEXT("���� ��������"));
	//ComboBox_AddString(hwndPriorityClass, TEXT("�������"));
	//ComboBox_AddString(hwndPriorityClass, TEXT("��������� �������"));
	//ComboBox_SetCurSel(hwndPriorityClass, 3); // Default to "�������"
	   	  

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
	/*������������� ���������� ������*/
	constexpr LPCTSTR PrioritiesArr[7] = {
		TEXT("��������� �� �������"),
		TEXT("������������"),
		TEXT("���� ��������"),
		TEXT("�������"),
		TEXT("���� ��������"),
		TEXT("�����������"),
		TEXT("�������������")
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

		/*��������� ������ ����������*/
	
		SelItem = ComboBox_GetCurSel(hwndCtl);
		UINT PriorityClass = (SelItem != -1) ? (UINT)ComboBox_GetItemData(hwndCtl, SelItem) : NORMAL_PRIORITY_CLASS;

		/*������� ������ ����������*/

		SetPriorityClass(GetCurrentProcess(), PriorityClass);

		/*��������� �������������� ���������� ��� �������� ������ */

		hwndCtl = GetDlgItem(hwnd, IDC_COMBO_PRIOR);

		/*��������� �������������� ���������� ��� �������� ������*/
		SelItem = ComboBox_GetCurSel(hwndCtl);
		UINT Priority;
		if (SelItem != -1)
			Priority = (UINT)ComboBox_GetItemData(hwndCtl, SelItem);
		else Priority = THREAD_PRIORITY_NORMAL;

		//UINT Priority = (SelItem != -1) ? (UINT)ComboBox_GetItemData(hwndCtl, SelItem) : THREAD_PRIORITY_NORMAL; //��������� ��������

		/*������� �������������� ����������*/
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
	ListBox_ResetContent(hwndCtl);//������� ������

	DWORD PIDarray[1024], cbNeeded = 0;//������ ��� ID ��������� ���������
	BOOL bRet = EnumProcesses(PIDarray, sizeof(PIDarray), &cbNeeded);//��������� ������ ID ��������� �����������
	
	if (FALSE != bRet)
	{
		TCHAR ProcessName[MAX_PATH], szBuffer[300];

		for (DWORD i = 0,
			n = cbNeeded / sizeof(DWORD); i < n; ++i)
		{
			DWORD PID = PIDarray[i], cch = 0;
			if (0 == PID) continue;

			
			HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, PID);//��������� ����������� �������� �� ��� ID

			if (NULL != hProcess)
			{
				cch = GetModuleBaseName(hProcess, NULL, ProcessName, _countof(ProcessName));// �������� ��� �������� ������ ��������

				CloseHandle(hProcess); // ��������� ������ ����
			} 

			if (0 == cch)
				StringCchCopy(ProcessName, MAX_PATH, TEXT("��� �������� �� ����������"));

			StringCchPrintf(szBuffer, _countof(szBuffer), TEXT("%s (PID: %u)"), ProcessName, PID);

			
			int iItem = ListBox_AddString(hwndCtl, szBuffer);

			ListBox_SetItemData(hwndCtl, iItem, PID);//������ � ListBox ����� ��������
		} 
	} 
} 

void ToLB_LoadModules(HWND hwndCtl, DWORD PID)
{
	ListBox_ResetContent(hwndCtl);

	HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, PID);

	if (NULL != hProcess)
	{
		/*����������� ������� ������ �������*/
		DWORD cMod = 0;
		EnumProcessModulesEx(hProcess, NULL, 0, &cMod, LIST_MODULES_ALL);

		DWORD NcMod = cMod / sizeof(HMODULE);//������� ����� �������

		HMODULE *hModule = new HMODULE[NcMod];//������� ����������� ������

		// �������� ������ �������
		cMod = NcMod * sizeof(HMODULE);
		BOOL bRet = EnumProcessModulesEx(hProcess, hModule, cMod, &cMod, LIST_MODULES_ALL);

		if (FALSE != bRet)
		{
			TCHAR ModuleName[MAX_PATH];

			for (DWORD i = 0; i < NcMod; ++i)
			{
				bRet = GetModuleFileNameEx(hProcess, hModule[i], ModuleName, MAX_PATH);
				if (FALSE != bRet) ListBox_AddString(hwndCtl, ModuleName); // ��������� � ������ ����� ������
			} 
		} 

		/*������������ ��������*/
		delete[]hModule; 
		CloseHandle(hProcess); 
	} 
} 

void ToLB_LoadProcessesInJob(HWND hwndCtl, HANDLE hJob)
{

	ListBox_ResetContent(hwndCtl);

	DWORD PIDarray[1024]; //������ ��� ���� ��������������� ���������
	DWORD cbNeeded = 0;//������ ����� ������ � ����������������
	BOOL bRet = EnumProcessesInJob(hJob, PIDarray, sizeof(PIDarray), &cbNeeded);

	if (FALSE != bRet)
	{
		TCHAR ProcessName[MAX_PATH];//��� ��������
		TCHAR szBuffer[300];//������ � ������� ������� ��� �������� � ��� �����

		for (DWORD i = 0,
			n = cbNeeded / sizeof(DWORD); i < n; ++i)
		{
			DWORD PID = PIDarray[i], cch = 0;

			HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, PID);

			if (NULL != hProcess)
			{
				
				cch = GetModuleBaseName(hProcess, NULL, ProcessName, _countof(ProcessName));// �������� ��� �������� ������ ��������
				CloseHandle(hProcess); // ��������� ������ ����
			} // if

			if (0 == cch)
				StringCchCopy(ProcessName, MAX_PATH, TEXT("����������� �������"));

			StringCchPrintf(szBuffer, _countof(szBuffer), TEXT("%s (PID: %u)"), ProcessName, PID);// ��������� ������ ��� ������
						
			int iItem = ListBox_AddString(hwndCtl, szBuffer);
			
			ListBox_SetItemData(hwndCtl, iItem, PID);// ��������� � ����� ������ ������������� ��������
		} 
	} 
} 


BOOL EnumProcessesInJob(HANDLE hJob, DWORD* lpPID, DWORD cb, LPDWORD lpcbNeeded)
{

	DWORD nCount = cb / sizeof(ULONG_PTR);//������� max ��������� ����� � ������

	if (NULL != lpPID && nCount > 0)
	{
		DWORD jobProcessStructSize = sizeof(JOBOBJECT_BASIC_PROCESS_ID_LIST) + sizeof(ULONG_PTR) * 1024;//���� ������ ��� ���������
		JOBOBJECT_BASIC_PROCESS_ID_LIST* jobProcessIdList = static_cast<JOBOBJECT_BASIC_PROCESS_ID_LIST*>(malloc(jobProcessStructSize));
		
		if (NULL != jobProcessIdList)
		{		
			jobProcessIdList->NumberOfAssignedProcesses = nCount;// MAX ����� ���������, �� ������� ��������� ���������� ������
			// ����������� ������ ��������������� ���������
			BOOL bRet = QueryInformationJobObject(hJob, JobObjectBasicProcessIdList, jobProcessIdList, jobProcessStructSize, NULL);

			if (FALSE != bRet)
			{		
				nCount = jobProcessIdList->NumberOfProcessIdsInList;// ���������� ���������� ��������������
				CopyMemory(lpPID, jobProcessIdList->ProcessIdList, nCount * sizeof(ULONG_PTR));//copies a block of memory from one location to another

				if (NULL != lpcbNeeded)
					*lpcbNeeded = nCount * sizeof(ULONG_PTR);//������ ����� ������ � ����������������
			} 

			free(jobProcessIdList); // ����������� ������
			return bRet;
		} 
	} 

	return FALSE;
} 

BOOL StartGroupProcessesAsJob(HANDLE hjob, LPCTSTR lpszCmdLine[], DWORD nCount, BOOL bInheritHandles, DWORD CreationFlags)
{
	
	BOOL bInJob = FALSE;
	IsProcessInJob(GetCurrentProcess(), NULL, &bInJob);// ���������, ������� �� ���������� ������� � �������
	
		JOBOBJECT_BASIC_LIMIT_INFORMATION jobli = { 0 };//������� �����������
		 
		QueryInformationJobObject(NULL, JobObjectBasicLimitInformation, &jobli, sizeof(jobli), NULL);
		JOBOBJECT_BASIC_UI_RESTRICTIONS jobuir;
		//�������� ��������� ������ �� ������ ������
		jobuir.UIRestrictionsClass |= JOB_OBJECT_UILIMIT_READCLIPBOARD;
		
		SetInformationJobObject(hjob, JobObjectBasicUIRestrictions, &jobuir, sizeof(jobuir));

		//����������� ���� ���������� ������� ������ � ��������
		//������ ���� - �������� ��� ����� ������� ����� ���������� ��� �������
		//������ ���� - ��������, ��� ����� ������� �� �������� ������ �������

		DWORD dwLimitMask = JOB_OBJECT_LIMIT_BREAKAWAY_OK | JOB_OBJECT_LIMIT_SILENT_BREAKAWAY_OK;

		if ((jobli.LimitFlags & dwLimitMask) == 0)
		{
			/* ��� ����������� ��������   ������������� ���������� � ������� */
			return FALSE;
		} 

	// ��������� ��������...
	STARTUPINFO si = { sizeof(STARTUPINFO) };
	PROCESS_INFORMATION pi = { 0 };
	TCHAR szCmdLine[MAX_PATH];


	for (DWORD i = 0; i < nCount; ++i)
	{
		StringCchCopy(szCmdLine, MAX_PATH, lpszCmdLine[i]);
		// ��������� ����� �������,
		// ��������������� ������ ��� �������� ������
		BOOL bRet = CreateProcess(NULL, szCmdLine, NULL, NULL,
			bInheritHandles, CreationFlags | CREATE_SUSPENDED | CREATE_BREAKAWAY_FROM_JOB, NULL, NULL, &si, &pi);

		if (FALSE != bRet)
		{
			//�������� ��������, ����������� ���� ��������� ������ ������ ����� ������� �������������
			AssignProcessToJobObject(hjob, pi.hProcess);// ��������� ����� ������� � �������
			
			ResumeThread(pi.hThread);// ������������ ������ ������ ������ ��������
			
			CloseHandle(pi.hThread);// ��������� ���������� ������ ������ ��������
			
			CloseHandle(pi.hProcess);// ��������� ���������� ������ ��������
		} 
	} 
	return TRUE;
} 

BOOL WaitProcessById(DWORD PID, DWORD WAITTIME, LPDWORD lpExitCode)
{
	HANDLE hProcess = OpenProcess(SYNCHRONIZE | PROCESS_QUERY_INFORMATION, FALSE, PID);// ��������� �������

	if (NULL == hProcess)
	{
		return FALSE;
	} 
	
	WaitForSingleObject(hProcess, WAITTIME);// ������� ���������� ��������

	if (NULL != lpExitCode)
	{
		GetExitCodeProcess(hProcess, lpExitCode);// ������� ��� ���������� ��������
	} 

	CloseHandle(hProcess);// ��������� ���������� ��������

	return TRUE;
} 