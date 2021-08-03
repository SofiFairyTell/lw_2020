#include "Editor2.h"
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

	LoadLibrary(TEXT("ComCtl32.dll"));//��� ��������� ������ �����������
	// ������� ������� ���� �� ������ ������ �������� ������
	TCHAR InitFN[MAX_PATH];//��� ����� �������������
	{
		GetModuleFileName(NULL, InitFN, MAX_PATH);
		LPTSTR str = _tcsrchr(InitFN, TEXT('.'));
		if (NULL != str) str[0] = TEXT('\0');
		StringCchCat(InitFN, MAX_PATH, TEXT(".ini"));
	}
	// ��������� ��������� ���������� �� ����� �������������
	LoadProfile(InitFN);

	HWND hWnd = CreateWindowEx(0, TEXT("MainWindowClass"), TEXT("Process"), WS_OVERLAPPEDWINDOW,
		WindowPosition.x, WindowPosition.y, WindowSize.cx, WindowSize.cy, NULL, NULL, hInstance, NULL);

	if (NULL == hWnd)
	{
		return -1; // ��������� ������ ����������
	}

	ShowWindow(hWnd, nCmdShow); // ���������� ������� ����

	MSG  msg;
	BOOL Ret;

	for (;;)
	{
		// ���������� ������� ��������� � �������
		while (!PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE))
		{
			OnIdle(hWnd);
		}
		// ��������� ��������� �� �������
		Ret = GetMessage(&msg, NULL, 0, 0);
		if (Ret == FALSE)
		{
			break; // �������� WM_QUIT, ����� �� �����
		}
		else if (!TranslateAccelerator(hWnd, hAccel, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
	SaveProfile(InitFN);//���������� ����������

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
		MoveWindow(hwndCtl, 0, 0, LOWORD(lParam), HIWORD(lParam), TRUE); // �������� ������� ���� �����
	}

	break;
	case WM_DESTROY:
	{
		if (INVALID_HANDLE_VALUE != hFile)
		{
			FinishIo(&ovlWrite);	// ������� ���������� �������� �����/������
			CloseHandle(hFile), hFile = INVALID_HANDLE_VALUE;// ��������� ���������� �����
		}

		if (NULL != hFont)
			DeleteObject(hFont), hFont = NULL;// ������� ��������� �����
		PostQuitMessage(0); // ���������� ��������� WM_QUIT
	}break;
	case WM_CLOSE:
		RECT rect;
		GetWindowRect(hwnd, &rect);

		WindowPosition.x = rect.left;
		WindowPosition.y = rect.top;

		WindowSize.cx = rect.right - rect.left;
		WindowSize.cy = rect.bottom - rect.top;

		DestroyWindow(hwnd); // ���������� ����
		break;
	}
	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}


void OnIdle(HWND hwnd)
{
	if (NULL != lpBuffReWri)
	{
		if (TryFinishIo(&ovlRead) != FALSE) // ����������� ������ ������ �� ����� ���������
		{
			if (ERROR_SUCCESS == ovlRead.Internal) // ������ ��������� �������
			{

				WORD bom = *(LPWORD)lpBuffReWri; // ������ ������������������ ������
				if (0xFEFF == bom) // Unicode-����
				{
					//LPWSTR lpszText = (LPWSTR)((BYTE*)lpBuffReWri);// +sizeof(WORD); // Unicode-������
					// ��������� ����� Unicode-������
					LPWSTR lpszText = (LPWSTR)((BYTE*)lpBuffReWri + sizeof(WORD));
					DWORD cch = (ovlRead.InternalHigh - sizeof(WORD)) / sizeof(WCHAR);
					lpszText[cch] = L'\0';// ����� ����-������ � ����� ������			
					SetDlgItemTextW(hwnd, IDC_EDIT_TEXT, lpszText);// �������� Unicode-������ � ���� �����
				}
				else // ANSI-����
				{
					lpBuffReWri[ovlRead.InternalHigh] = '\0';// ����� ����-������ � ����� ������
					SetDlgItemTextW(hwnd, IDC_EDIT_TEXT, lpBuffReWri);// �������� ANSI-������ � ���� �����
				}
			}
			delete[] lpBuffReWri, lpBuffReWri = NULL;	// ����������� ���������� ������
		}
		else if (TryFinishIo(&ovlWrite) != FALSE) // ����������� ������ ������ � ���� ���������
		{
			if (ERROR_SUCCESS == ovlWrite.Internal) // ������ ��������� �������
			{
				// �������� ������������ ������� �������� ������ � ���� �� ��������� ��� ��������
				FlushFileBuffers(hFile);
			}
			delete[] lpBuffReWri, lpBuffReWri = NULL;// ����������� ���������� ������
		}
	}
}



BOOL OnCreate(HWND hwnd, LPCREATESTRUCT lpCreateStruct)
{
	// ������ ���� ����� ��� �������������� ������
	DWORD Styles = WS_VISIBLE | WS_CHILD | WS_BORDER | WS_HSCROLL | WS_VSCROLL | ES_NOHIDESEL | ES_AUTOVSCROLL | ES_MULTILINE | ES_SAVESEL | ES_SUNKEN;

	// ������� ����� ���������� Rich Edit
	HWND hwndCtl = CreateWindowEx(0, TEXT("RICHEDIT"), TEXT(""), Styles, 0, 0, 100, 100, hwnd, (HMENU)IDC_EDIT_TEXT, lpCreateStruct->hInstance, NULL);

	if (hwndCtl == NULL)
		return FALSE;

	// �������� ����� ����� ������ ���������� Rich Edit
	SetFocus(hwndCtl);


	// ������ ����������� �� ������ ������
	//Edit_LimitText(hwndCtl, (DWORD)-1); //If it will be return RichEdit will be block for edit and entering text

	logFont.lfCharSet = DEFAULT_CHARSET; //�������� �� ���������
	logFont.lfPitchAndFamily = DEFAULT_PITCH; //�������� �� ���������
	wcscpy_s(logFont.lfFaceName, L"Times New Roman"); //�������� � ������ �������� ������
	logFont.lfHeight = 20; //������
	logFont.lfWidth = 10; //������
	logFont.lfWeight = 40; //�������
	logFont.lfEscapement = 0; //����� ��� ��������

	// ������ �����
	hFont = CreateFontIndirect(&logFont);
	static HWND hEdit = GetDlgItem(hwnd, IDC_EDIT_TEXT);
	if (NULL != hFont)
	{
		// ������������� ����� ��� ���� �����
		SendMessage(hwndCtl, WM_SETFONT, (WPARAM)hFont, (LPARAM)TRUE);
	}

	// ��������� ��������� ������������� ��������� ���
	if (OpenFileAsync(hwndCtl) != FALSE)
	{
		// ����� ��������� �������� ����
		SetWindowText(hwnd, FileName);
		SendMessage(hEdit, EM_SETPARAFORMAT, 0, (LPARAM)&pf);//������������ ������
	}
	else
	{
		FileName[0] = _T('\0');// ������� ��� �������������� ���������� �����	
		SetWindowText(hwnd, TEXT("����������"));// ����� ��������� �������� ����
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
	case ID_NEW_FILE: // �������
	{
		if (INVALID_HANDLE_VALUE != hFile)
		{

			FinishIo(&ovlWrite);// ������� ���������� �������� �����/������
			CloseHandle(hFile), hFile = INVALID_HANDLE_VALUE;
		}
		Edit_SetText(hEdit, NULL);// ������� ����� �� ���� �����

		FileName[0] = _T('\0');
		SetWindowText(hwnd, TEXT("����������"));
	}
	break;

	case ID_OPEN: // �������
	{
		OPENFILENAME openfile = { sizeof(OPENFILENAME) };

		openfile.hInstance = GetWindowInstance(hwnd);
		openfile.lpstrFilter = TEXT("��������� ��������� (*.txt)\0*.txt\0");
		openfile.lpstrFile = FileName;
		openfile.nMaxFile = _countof(FileName);
		openfile.lpstrTitle = TEXT("�������");
		openfile.Flags = OFN_EXPLORER | OFN_ENABLESIZING | OFN_FILEMUSTEXIST;
		openfile.lpstrDefExt = TEXT("txt");

		if (GetOpenFileName(&openfile) != FALSE)
		{
			if (OpenFileAsync(hEdit) != FALSE) // ��������� ����
			{
				SetWindowText(hwnd, FileName);// ����� ��������� �������� ����
			}
			else
			{
				MessageBox(NULL, TEXT("�� ������� ������� ��������� ����."), NULL, MB_OK | MB_ICONERROR);
				FileName[0] = _T('\0');
				SetWindowText(hwnd, TEXT("����������"));
			}
		}
	}
	break;

	case ID_SAVE: // ���������
		if (SaveFileAsync(hEdit) != FALSE) // ��������� ����
		{
			break;
		}
	case ID_SAVE_AS: // ��������� ���
	{
		OPENFILENAME savefile = { sizeof(OPENFILENAME) };

		savefile.hInstance = GetWindowInstance(hwnd);
		savefile.lpstrFilter = TEXT("��������� ��������� (*.txt)\0*.txt\0");
		savefile.lpstrFile = FileName;
		savefile.nMaxFile = _countof(FileName);
		savefile.lpstrTitle = TEXT("��������� ���");
		savefile.Flags = OFN_EXPLORER | OFN_ENABLESIZING | OFN_CREATEPROMPT | OFN_OVERWRITEPROMPT;
		savefile.lpstrDefExt = TEXT("txt");

		if (GetSaveFileName(&savefile) != FALSE)
		{
			if (SaveFileAsync(hEdit, TRUE) != FALSE) // ������������� ����
			{
				SetWindowText(hwnd, FileName);// ����� ��������� �������� ����
			}
			else
			{
				MessageBox(NULL, TEXT("�� ������� ��������� ��������� ����."), NULL, MB_OK | MB_ICONERROR);
			}
		}
	}
	break;

	case ID_EXIT:
		SendMessage(hwnd, WM_CLOSE, 0, 0);
		break;

	case ID_UNDO: // ��������
	{

		Edit_Undo(hEdit);// �������� �������� ��������� � ���� �����	
		SetFocus(hEdit);// ������� ����� ��������� � ���� �����
	}
	break;

	case ID_SELECT_ALL: // �������� ���
	{
		Edit_SetSel(hEdit, 0, -1);// �������� ����� � ���� �����
		SetFocus(hEdit);// ������� ����� ��������� � ���� �����
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

		// ������������� ������������ ��������� �� ������ �������
		// ���� ������ ���������� Rich Edit
	case ID_FORMAT_PARAGRAPH_RIGHT:
	{
		pf.cbSize = sizeof(pf);
		pf.dwMask = PFM_ALIGNMENT;
		pf.wAlignment = PFA_RIGHT;
		SendMessage(hEdit, EM_SETPARAFORMAT, 0, (LPARAM)&pf);
		break;
	}

	// ��������� ��������� �������� ���������
	case ID_FORMAT_PARAGRAPH_CENTER:
	{
		pf.cbSize = sizeof(pf);
		pf.dwMask = PFM_ALIGNMENT;
		pf.wAlignment = PFA_CENTER;
		SendMessage(hEdit, EM_SETPARAFORMAT, 0, (LPARAM)&pf);

		break;
	}
	// ������������� ������������ ��������� �� ����� �������
// ���� ������ ���������� Rich Edit
	case ID_FORMAT_PARAGRAPH_LEFT:
	{
		pf.cbSize = sizeof(pf);
		pf.dwMask = PFM_ALIGNMENT;
		pf.wAlignment = PFA_LEFT;

		// �������� ��� ������������ �������� ���������
		SendMessage(hEdit, EM_SETPARAFORMAT, 0, (LPARAM)&pf);
		break;
	}
	}



} // OnCommand

void LoadProfile(LPCTSTR lpFileName)
{
	// ��������� ��������� � ������ ����

	WindowPosition.x = GetPrivateProfileInt(TEXT("Window"), TEXT("X"), CW_USEDEFAULT, lpFileName);
	WindowPosition.y = GetPrivateProfileInt(TEXT("Window"), TEXT("Y"), 0, lpFileName);

	WindowSize.cx = GetPrivateProfileInt(TEXT("Window"), TEXT("Width"), CW_USEDEFAULT, lpFileName);
	WindowSize.cy = GetPrivateProfileInt(TEXT("Window"), TEXT("Height"), 600, lpFileName);

	/*Dont init without this items*/
	pf.cbSize = sizeof(pf);
	pf.dwMask = PFM_ALIGNMENT;

	//�������� ���� ������������
	if (GetPrivateProfileInt(TEXT("Paraformat"), TEXT("wAlignment"), 0, lpFileName) == 3)
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
	// ��������� ��� ���������� �������������� ���������� �����
	GetPrivateProfileString(TEXT("File"), TEXT("Filename"), NULL, FileName, MAX_PATH, lpFileName);
}

void SaveProfile(LPCTSTR lpFileName)
{
	TCHAR szString[10];

	// ��������� ��������� � ������ ����

	StringCchPrintf(szString, 10, TEXT("%d"), WindowPosition.x);
	WritePrivateProfileString(TEXT("Window"), TEXT("X"), szString, lpFileName);

	StringCchPrintf(szString, 10, TEXT("%d"), WindowPosition.y);
	WritePrivateProfileString(TEXT("Window"), TEXT("Y"), szString, lpFileName);

	StringCchPrintf(szString, 10, TEXT("%d"), WindowSize.cx);
	WritePrivateProfileString(TEXT("Window"), TEXT("Width"), szString, lpFileName);

	StringCchPrintf(szString, 10, TEXT("%d"), WindowSize.cy);
	WritePrivateProfileString(TEXT("Window"), TEXT("Height"), szString, lpFileName);

	//��������� ��������� ������������ ������
	/*	warning! now this line describe all text in program.
		so, if you have first string with PFA_LEFT and second string with PFA_CENTER
		saveprofile write wAlignment = 3 (it is PFA_CENTER) in .ini file
	*/
	StringCchPrintf(szString, 10, TEXT("%d"), pf.dwMask);
	WritePrivateProfileString(TEXT("Paraformat"), TEXT("dwMask"), szString, lpFileName);

	StringCchPrintf(szString, 10, TEXT("%d"), pf.wAlignment);
	WritePrivateProfileString(TEXT("Paraformat"), TEXT("wAlignment"), szString, lpFileName);

	// ��������� ��� ���������� �������������� ���������� �����

	WritePrivateProfileString(TEXT("File"), TEXT("Filename"), FileName, lpFileName);



}

BOOL OpenFileAsync(HWND hwndCtl)
{

	HANDLE hExistingFile = CreateFile(FileName,
		GENERIC_READ | GENERIC_WRITE,//������ � �����
		FILE_SHARE_READ, //��� ����������� ������
		NULL, //������ ���
		OPEN_EXISTING, //������� ������������
		FILE_FLAG_OVERLAPPED,//����������� ������ � �����
		NULL); //������� ���

	if (INVALID_HANDLE_VALUE == hExistingFile) // �� ������� ������� ����
	{
		return FALSE;
	}

	Edit_SetText(hwndCtl, NULL);// ������� ����� �� ���� �����

	if (INVALID_HANDLE_VALUE != hFile)
	{
		FinishIo(&ovlWrite);	// ������� ���������� �������� �����/������
		CloseHandle(hFile);
	}

	hFile = hExistingFile;

	LARGE_INTEGER size;// ���������� ������ ����� 
	BOOL bRet = GetFileSizeEx(hFile, &size);


	if ((FALSE != bRet) && (size.LowPart > 0))
	{
		// �������� ������ ��� ������, � ������� ����� ����������� ������ �� �����
		lpBuffReWri = new WCHAR[size.LowPart + 1];

		bRet = ReadAsync(hFile, lpBuffReWri, 0, size.LowPart, &ovlRead);// ����������� ������ ������ �� �����

		if (FALSE == bRet) // �������� ������
		{
			delete[] lpBuffReWri, lpBuffReWri = NULL;// ����������� ���������� ������
		}
	}
	return bRet;
}

BOOL SaveFileAsync(HWND hwndCtl, BOOL fSaveAs)
{

	if (fSaveAs != FALSE)
	{
		// ������ � ��������� ���� ��� ������ � ������
		HANDLE hNewFile = CreateFile(FileName, GENERIC_WRITE | GENERIC_READ, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_FLAG_OVERLAPPED, NULL);


		if (hNewFile == INVALID_HANDLE_VALUE) // �� ������� ������� ����
		{
			return FALSE;
		}

		if (hFile != INVALID_HANDLE_VALUE)
		{
			FinishIo(&ovlWrite);// ������� ���������� �������� �����/������
			CloseHandle(hFile);
		}

		hFile = hNewFile;
	}
	else
		if (hFile != INVALID_HANDLE_VALUE)
		{
			FinishIo(&ovlWrite);// ������� ���������� �������� �����/������
		}
		else
		{
			hFile = CreateFile(FileName, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_FLAG_OVERLAPPED, NULL);

			if (hFile == INVALID_HANDLE_VALUE) // �� ������� ������� ����
			{
				return FALSE;
			}
		}


	LARGE_INTEGER size;// ��� ����������� ������� ������

	size.QuadPart = GetWindowTextLengthW(hwndCtl);//��������� ������� ��� ������ ������
	BOOL bRet = FALSE;


	if (size.LowPart > 0)//���� �� ������
	{
		lpBuffReWri = new WCHAR[size.LowPart + 2];	// �������� ������ ��� ������, �� �������� ����� ������������ ������ � ���� + 2 �� ���������

		/*------For UNICODE---------*/
		*(WORD*)lpBuffReWri = 0xfeff;//���������� � ������ ����� ��������� BOM ��� ����, ����� ���� ��� UNICODE. UNICODE, BOM. 
		/*------For UNICODE---------*/

		GetWindowTextW(hwndCtl, lpBuffReWri + 1, size.LowPart + 1);		// �������� UNICODE ������ �� ���� ����� � ������

		bRet = WriteAsync(hFile, lpBuffReWri, 0, size.LowPart * 2 + 2, &ovlWrite);// ����������� ������ ������ � ����
																			//������ *2, ������ ��� ��������� WCHAR, +2 �� ���������

		if (FALSE == bRet)
		{
			delete[] lpBuffReWri, lpBuffReWri = NULL;// ����������� ���������� ������
		}
	}

	return bRet;
}

/*Asynch work*/

BOOL ReadAsync(HANDLE hFile, LPVOID lpBuffer, DWORD dwOffset, DWORD dwSize, LPOVERLAPPED ovl)
{
	// �������������� ��������� OVERLAPPED 
	ZeroMemory(ovl, sizeof(ovl));

	ovl->Offset = dwOffset; // ������� ����� ��������
	ovl->hEvent = CreateEvent(NULL, FALSE, FALSE, NULL); //������� ��� ���������� ���������� ������

	// �������� ����������� �������� ������ ������ �� �����
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
	// �������������� ��������� OVERLAPPED
	ZeroMemory(ovl, sizeof(ovl));
	ovl->Offset = dwOffset; // ������� ����� ��������
	ovl->hEvent = CreateEvent(NULL, FALSE, FALSE, NULL); //������� ��� ���������� ���������� ������

	BOOL bRet = WriteFile(hFile, lpBuffer, dwSize, NULL, ovl);

	if (FALSE == bRet && ERROR_IO_PENDING != GetLastError())
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
		// ������� ���������� �������� �����/������
		DWORD dwRes = WaitForSingleObject(ovl->hEvent, INFINITE);

		if (WAIT_OBJECT_0 == dwRes) // �������� ���������
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

		DWORD dwResult = WaitForSingleObject(lpOverlapped->hEvent, 0);// ���������� ��������� �������� �����/������

		if (WAIT_OBJECT_0 == dwResult) // �������� ���������
		{
			CloseHandle(lpOverlapped->hEvent),
				lpOverlapped->hEvent = NULL;
			return TRUE;
		}
	}

	return FALSE;
}

