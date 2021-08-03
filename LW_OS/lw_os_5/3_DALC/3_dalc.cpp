
#include "FileInfoFuncHeader.h"


int WINAPI _tWinMain(HINSTANCE hInstance, HINSTANCE, LPTSTR lpszCmdLine, int nCmdShow)
{

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



	//DWORD dwDisposition;

	//// ������ � ��������� ���� ������� ��� ���������� ���������� ����������
	//RegCreateKeyEx(HKEY_CURRENT_USER, TEXT("Software\\IT-311"),
	//	0, NULL, REG_OPTION_NON_VOLATILE, KEY_QUERY_VALUE | KEY_SET_VALUE, NULL, &hKey, &dwDisposition);
	//
	//// �������� ��� �����/�������� �� ���������� �������
	//RegGetValueSZ(hKey, TEXT("Path"), FileName, _countof(FileName), NULL);
	//// �������� ��������� ���� �� ���������� �������
	//RegGetValueBinary(hKey, TEXT("rect"), (LPBYTE)&rect, sizeof(rect), NULL);


	LoadLibrary(TEXT("ComCtl32.dll"));//��� ��������� ������ �����������		


	if (0 == RegisterClassEx(&wcex)) // ������������ �����
	{
		return -1; // ��������� ������ ����������
	}
	RECT wr = { 0, 0, 500, 500 };    // set the size, but not the position

	// ������� ������� ���� �� ������ ������ �������� ������
	HWND hWnd = CreateWindowEx(0, TEXT("MainWindowClass"), TEXT("Process"), WS_OVERLAPPEDWINDOW^WS_THICKFRAME^WS_MINIMIZEBOX^WS_MAXIMIZEBOX, 300, 300,
		wr.right - wr.left, wr.bottom - wr.top, NULL, NULL, hInstance, NULL);
	if (IsRectEmpty(&rect) == FALSE)
	{
		// �������� ��������� ����
		SetWindowPos(hWnd, NULL, rect.left, rect.top, 0, 0, SWP_NOSIZE | SWP_SHOWWINDOW);
	} // if


	if (NULL == hWnd)
	{
		return -1; // ��������� ������ ����������
	}

	ShowWindow(hWnd, nCmdShow); // ���������� ������� ����

	MSG  msg;
	BOOL Ret;

	for (;;)
	{

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
		HWND hwndCtl = GetDlgItem(hwnd, IDC_EDIT_TEXT);
		MoveWindow(hwndCtl, 0, 0, LOWORD(lParam), HIWORD(lParam), TRUE); // �������� ������� ���� �����
	}

	break;
	case WM_DESTROY:
	{
		PostQuitMessage(0); // ���������� ��������� WM_QUIT
	}break;
	case WM_CLOSE:

		DestroyWindow(hwnd); // ���������� ����
		break;
	}
	return DefWindowProc(hwnd, msg, wParam, lParam);
}



BOOL OnCreate(HWND hwnd, LPCREATESTRUCT lpCRStr)
{
	CreateWindowEx(0, TEXT("Edit"), NULL, WS_CHILD | WS_VISIBLE | WS_BORDER, 30, 10, 400, 30, hwnd, (HMENU)IDC_EDIT_FILENAME, lpCRStr->hInstance, NULL);
	HWND hwndLV = CreateWindowEx(0, TEXT("SysListView32"), NULL, WS_CHILD | WS_VISIBLE | WS_BORDER | LVS_REPORT | LVS_SHOWSELALWAYS, 30, 40, 400, 150, hwnd, (HMENU)IDC_LIST1, lpCRStr->hInstance, NULL);

	//�������� ���������
	/*
	CreateWindowEx(0, TEXT("button"), TEXT("������ ��� ������"), WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX, 30, 200, 400, 30, hwnd, (HMENU)IDC_ATTRIBUTE_READONLY, lpCRStr->hInstance, NULL);
	CreateWindowEx(0, TEXT("button"), TEXT("�������"), WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX, 30, 230, 400, 30, hwnd, (HMENU)IDC_ATTRIBUTE_HIDDEN, lpCRStr->hInstance, NULL);
	CreateWindowEx(0, TEXT("button"), TEXT("���� ����� ��� ���������������"), WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX, 30, 260, 400, 30, hwnd, (HMENU)IDC_ATTRIBUTE_ARCHIVE, lpCRStr->hInstance, NULL);
	CreateWindowEx(0, TEXT("button"), TEXT("���������"), WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX, 30, 290, 400, 30, hwnd, (HMENU)IDC_ATTRIBUTE_SYSTEM, lpCRStr->hInstance, NULL);
	CreateWindowEx(0, TEXT("button"), TEXT("���������"), WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX, 30, 320, 400, 30, hwnd, (HMENU)IDC_ATTRIBUTE_TEMPORARY, lpCRStr->hInstance, NULL);
	CreateWindowEx(0, TEXT("button"), TEXT("������� ��� �������� �����"), WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX, 30, 350, 400, 30, hwnd, (HMENU)IDC_ATTRIBUTE_COMPRESSED, lpCRStr->hInstance, NULL);
	CreateWindowEx(0,TEXT("button"), TEXT("��������� ���������� ��� ������"), WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX,30, 380, 400, 30, hwnd,(HMENU)IDC_ATTRIBUTE_ENCRYPTED,lpCRStr->hInstance, NULL);
	*/
	// ����� ����������� 
	ListView_SetExtendedListViewStyle(hwndLV, LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

	// ��������� ��� ������� � ������ ���������

	LVCOLUMN lvColumns[] = {
		{ LVCF_WIDTH | LVCF_TEXT, 0, 100, (LPTSTR)TEXT("���") },
		{ LVCF_WIDTH | LVCF_TEXT, 0, 100, (LPTSTR)TEXT("���") },
		{ LVCF_WIDTH | LVCF_TEXT, 0, 100, (LPTSTR)TEXT("��������� �:") },

	};

	for (int i = 0; i < _countof(lvColumns); ++i)
	{
		// ��������� �������
		ListView_InsertColumn(hwndLV, i, &lvColumns[i]);
	}
	//if (FileName != NULL) //���� ���� �� ������, �� ��������� ������
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
	case ID_OPEN_FILE: // �������
	{
		/*OPENFILENAME openfile = { sizeof(OPENFILENAME) };

		openfile.hInstance = GetWindowInstance(hwnd);
		openfile.lpstrFilter = TEXT("��������� ��������� (*.txt)\0*.txt\0");
		openfile.lpstrFile = FileName;

		openfile.nMaxFile = _countof(FileName);
		openfile.lpstrTitle = TEXT("�������");
		openfile.Flags = OFN_EXPLORER | OFN_ENABLESIZING | OFN_FILEMUSTEXIST;
		openfile.lpstrDefExt = TEXT("txt");*/

		//if (GetOpenFileName(&openfile) != FALSE)
		/*{
			// �������� ���������� � �����
			//get info about file
			if (!(ListViewInit(FileName, hwnd)))
			{
				GetLastError();
				break;
			}
			else
			{
				break;
			}
		}
		else
		{
			MessageBox(NULL, TEXT("�� ������� ������� ��������� ����."), NULL, MB_OK | MB_ICONERROR);
			FileName[0] = _T('\0');
		}*/
		BROWSEINFO bi;//structure for open special box with folder in treview
		LPITEMIDLIST pidl;
		ZeroMemory(&bi, sizeof(bi));
		bi.hwndOwner = NULL;
		bi.pszDisplayName = FileName;
		bi.lpszTitle = TEXT("Select folder");
		bi.ulFlags = BIF_BROWSEINCLUDEFILES;
		pidl = SHBrowseForFolder(&bi);//open window for select
		if (pidl)
		{
			SHGetPathFromIDList(pidl, FileName);//get path
			SetDlgItemText(hwnd, IDC_EDIT, FileName);
		}



	}
	break;
	case ID_OPEN_DIR://������� �����
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
	case ID_CHANGE_ATR://��������� ���������
	{
		/*
		TCHAR NewFileName[MAX_PATH]; // ����� ��� �����/��������
		GetDlgItemText(hwnd, IDC_EDIT_FILENAME, NewFileName, _countof(NewFileName));//��� ��� � ��� � ��������� lpszFileName

			// ����� ��� � ���� � �����/��������
		lpszFileName = PathFindFileName(FileName);

		// ��������� ����� ���� � �����/��������
		cchPath = (DWORD)(lpszFileName - FileName) - 1;
		// ��������� ����-�������� ���� � ��� �����/��������
		FileName[cchPath] = _T('\0');

		if (CompareString(LOCALE_USER_DEFAULT, 0, lpszFileName, -1, NewFileName, -1) != CSTR_EQUAL) // (!) ���������� ��� �����/��������
		{
			TCHAR ExistingFileName[MAX_PATH]; // ������ ��� �����/��������
			StringCchPrintf(ExistingFileName, _countof(ExistingFileName), TEXT("%s\\%s"), FileName, lpszFileName);

			// ��������� ����� ���� � �����/��������
			PathAppend(FileName, NewFileName);
			// ��������������� ����/�������
			MoveFile(ExistingFileName, FileName);
		} // if
		else
		{
			// ������� ����-������, ����������� ���� � ��� �����/��������
			FileName[cchPath] = _T('\\');
		} // else

		// ������ ���������
		constexpr DWORD attr[] = {
			FILE_ATTRIBUTE_READONLY, FILE_ATTRIBUTE_HIDDEN, FILE_ATTRIBUTE_ARCHIVE,
			FILE_ATTRIBUTE_SYSTEM, FILE_ATTRIBUTE_TEMPORARY, FILE_ATTRIBUTE_COMPRESSED, FILE_ATTRIBUTE_ENCRYPTED
		};

		// ������ ��������������� ������� ��� ���������
		constexpr DWORD ids[] = {
			IDC_ATTRIBUTE_READONLY, IDC_ATTRIBUTE_HIDDEN, IDC_ATTRIBUTE_ARCHIVE,
			IDC_ATTRIBUTE_SYSTEM, IDC_ATTRIBUTE_TEMPORARY, IDC_ATTRIBUTE_COMPRESSED, IDC_ATTRIBUTE_ENCRYPTED
		};

		DWORD dwFileAttributes = 0; // �������� �����/��������

		// ����������, ����� �������� �������

		for (int i = 0; i < _countof(attr); ++i)
		{
			if (IsDlgButtonChecked(hwnd, ids[i]) == BST_CHECKED) // ������ ����������
			{
				dwFileAttributes |= attr[i]; // ������� ��������������� �������
			} // if
		} // for

		// ������� ��������
		SetFileAttributes(FileName, dwFileAttributes);

		ListViewInit(FileName, hwnd);*/
	}

	case ID_RENAME://�������������� ��� ���������� ���������
	{
		/*
		TCHAR NewFileName[MAX_PATH]; // ����� ��� �����/��������
		GetDlgItemText(hwnd, IDC_EDIT_FILENAME, NewFileName, _countof(NewFileName));//��� ��� � ��� � ��������� lpszFileName

			// ����� ��� � ���� � �����/��������
		lpszFileName = PathFindFileName(FileName);

		// ��������� ����� ���� � �����/��������
		cchPath = (DWORD)(lpszFileName - FileName) - 1;
		// ��������� ����-�������� ���� � ��� �����/��������
		FileName[cchPath] = _T('\0');

		if (CompareString(LOCALE_USER_DEFAULT, 0, lpszFileName, -1, NewFileName, -1) != CSTR_EQUAL) // (!) ���������� ��� �����/��������
		{
			TCHAR ExistingFileName[MAX_PATH]; // ������ ��� �����/��������
			StringCchPrintf(ExistingFileName, _countof(ExistingFileName), TEXT("%s\\%s"), FileName, lpszFileName);

			// ��������� ����� ���� � �����/��������
			PathAppend(FileName, NewFileName);
			// ��������������� ����/�������
			MoveFile(ExistingFileName, FileName);
		}
		else
		{
			FileName[cchPath] = _T('\\');// ������� ����-������, ����������� ���� � ��� �����/��������
		}
		// �������� ������ � ��������� ����
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
	// ������ ��������� �������� ���� grfInheritance
	constexpr DWORD dwInherit[7] = {
		NO_INHERITANCE,
		SUB_CONTAINERS_AND_OBJECTS_INHERIT,
		SUB_CONTAINERS_ONLY_INHERIT,
		SUB_OBJECTS_ONLY_INHERIT,
		INHERIT_ONLY | SUB_CONTAINERS_AND_OBJECTS_INHERIT,
		INHERIT_ONLY | SUB_CONTAINERS_ONLY_INHERIT,
		INHERIT_ONLY | SUB_OBJECTS_ONLY_INHERIT
	};

	// ������ �����, ���������� �������� ��� ��������� �������� ���� grfInheritance
	constexpr LPCTSTR szInheritText[7] = {
		TEXT("������ ��� ����� ��������"),
		TEXT("��� ����� ��������, ��� ������������ � ������"),
		TEXT("��� ����� �������� � ��� ������������"),
		TEXT("��� ����� �������� � ��� ������"),
		TEXT("������ ��� ������������ � ������"),
		TEXT("������ ��� ������������"),
		TEXT("������ ��� ������")
	};

	WIN32_FILE_ATTRIBUTE_DATA bhfi;
	TCHAR TimeBuffer[100], Buffer[100];
	if (!GetFileAttributesEx(path, GetFileExInfoStandard, &bhfi))
	{
		GetLastError();
	}
	/*���������� �����*/
	LPTSTR lpFN = PathFindFileNameW(path);
	SetDlgItemText(hwnd, IDC_EDIT_FILENAME, lpFN);

	/*������������� ������*/
	HWND hwndLV = GetDlgItem(hwnd, IDC_LIST1);
	ListView_DeleteAllItems(hwndLV); //������� ������ ���������

	// ����������� ���������� ������
	if (NULL != pSD) LocalFree(pSD), pSD = NULL;
	// ������� ���������� ������������
	BOOL RetRes = GetFileSecurityDescriptor(FileName, OWNER_SECURITY_INFORMATION | DACL_SECURITY_INFORMATION, &pSD);

	if (RetRes != FALSE)
	{
		/*// �������� ��� ������� ������ ��������� � ���� "������� ��������"
			SetDlgItemText(hwnd, IDC_EDIT_OWNER, lpAccountOwner);
			// ����������� ���������� ������
			LocalFree(lpAccountOwner);
		*/
	}

	ULONG uCount = 0; // ���������� ��������� � ������� ACE
	PEXPLICIT_ACCESS pEA = NULL; // ������ ACE

	RetRes = GetItemFromDACL(pSD, &uCount, &pEA);

	if (RetRes != FALSE)
	{
		for (ULONG i = 0; i < uCount; ++i)
		{
			LVITEM lvItem = { LVIF_TEXT | LVIF_PARAM };

			lvItem.iItem = (int)i;

			// ��������� ����� �������
			lvItem.lParam = (LPARAM)pEA[i].grfAccessPermissions;

			// ��������� ��� ACE
			switch (pEA[i].grfAccessMode)
			{
			case GRANT_ACCESS:
				lvItem.pszText = (LPTSTR)TEXT("���������");
				break;
			case DENY_ACCESS:
				lvItem.pszText = (LPTSTR)TEXT("���������");
				break;
			} // switch

			// ��������� ����� ������� � ������ ��������� DACL
			int iItem = ListView_InsertItem(hwndLV, &lvItem);
			if (iItem == -1) continue; // �� ������� �������� ����� �������

			 // ��������� ��� ������� ������
			if (TRUSTEE_IS_SID == pEA[i].Trustee.TrusteeForm)
			{
				LPTSTR lpszName = NULL; // ��� ������� ������

				// ������� ��� ������� ������
				GetAccountName_W(pEA[i].Trustee.ptstrName, &lpszName);

				if (NULL != lpszName)
				{
					// �������� ��� ������� ������ � ������ ������ ��������� DACL
					ListView_SetItemText(hwndLV, iItem, 1, lpszName);
					// ����������� ���������� ������
					LocalFree(lpszName);
				} // if
			} // if
			// ��������� � ����� �������� ����������� ����� �������
			DWORD grfInheritance = pEA[i].grfInheritance & (~INHERIT_NO_PROPAGATE);

			for (int j = 0; j < _countof(dwInherit); ++j)
			{
				if (grfInheritance == dwInherit[j]) // ������� ��������
				{
					// �������� � ������ ������ ��������� DACL �������� ��� �������� ���� grfInheritance
					ListView_SetItemText(hwndLV, iItem, 2, (LPTSTR)szInheritText[j]);

					break; // ������� �� �����
				} // if
			} // for
		}
	}

	//��������� ���������� � ������� �����
	//get info about size of file
	//LARGE_INTEGER LI_Size;
	//ULARGE_INTEGER sizeDir = { 0 };
	//hFile = CreateFile(
	//	FileName,   // ��� �����
	//	GENERIC_READ,          // ������ �� �����
	//	FILE_SHARE_READ,       // ���������� ������ � �����
	//	NULL,                  // ������ ���
	//	OPEN_EXISTING,         // ��������� ������������ ����
	//	FILE_ATTRIBUTE_NORMAL,  // ����������� ����
	//	NULL                   // ������� ���
	//);
	//if (!GetFileSizeEx(hFile, &LI_Size))
	//{
	//	//��������� ������
	//}
	//if (bhfi.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
	//{
	//	//������ ��� ����� 
	//	CalculateSize(path, &bhfi, &sizeDir);
	//	ConvertDirectSize(Buffer, _countof(Buffer), sizeDir);
	//}
	//else
	//{

	//	ConvertFileSize(Buffer, _countof(Buffer), LI_Size);
	//	
	//}

	/*��������� ��� �������� ���������*/






	/*

	LPTSTR lpFN = PathFindFileNameW(path);
	SetDlgItemText(hwnd, IDC_EDIT_FILENAME, lpFN);

	//���������� ��������� ��������� � ������ ���������
	HWND hwndLV = GetDlgItem(hwnd, IDC_LIST1);
	// ��������� ����� ������� � ������ ���������
	ListView_DeleteAllItems(hwndLV);
	LVITEM lvItem = { LVIF_TEXT | LVIF_PARAM };
	lvItem.iItem = ListView_GetItemCount(hwndLV);
	//������
	lvItem.pszText = (LPWSTR)(L"������� ��������:");
	lvItem.iItem = ListView_InsertItem(hwndLV, &lvItem);
	if ((lvItem.iItem != -1))
	{
		ListView_SetItemText(hwndLV, lvItem.iItem, 1, Buffer);
	}
	//������ ��������
	lvItem.pszText = (LPWSTR)(L"����� ���������:");
	lvItem.iItem = ListView_InsertItem(hwndLV, &lvItem);
	if ((lvItem.iItem != -1))
	{
		GetFileTimeFormat(&bhfi.ftCreationTime, TimeBuffer, _countof(TimeBuffer));//����� ��������
		ListView_SetItemText(hwndLV, lvItem.iItem, 1, TimeBuffer);
	}
	//������ ��������
	lvItem.pszText = (LPWSTR)(L"����� ���������� ���������:");
	lvItem.iItem = ListView_InsertItem(hwndLV, &lvItem);
	if ((lvItem.iItem != -1))
	{
		GetFileTimeFormat(&bhfi.ftLastAccessTime, TimeBuffer, _countof(TimeBuffer));//����� ���������� ���������
		ListView_SetItemText(hwndLV, lvItem.iItem, 1, TimeBuffer);
	}
	//������ ��������
	lvItem.pszText = (LPWSTR)(L"����� ��������:");
	lvItem.iItem = ListView_InsertItem(hwndLV, &lvItem);
	if ((lvItem.iItem != -1))
	{
		GetFileTimeFormat(&bhfi.ftLastWriteTime, TimeBuffer, _countof(TimeBuffer));//����� ���������
		ListView_SetItemText(hwndLV, lvItem.iItem, 1, TimeBuffer);
	}
	//������������
	lvItem.pszText = (LPWSTR)(L"������������:");
	lvItem.iItem = ListView_InsertItem(hwndLV, &lvItem);
	if ((lvItem.iItem != -1))
	{
		ListView_SetItemText(hwndLV, lvItem.iItem, 1, path);
	}
	//���
	lvItem.pszText = (LPWSTR)(L"���:");
	lvItem.iItem = ListView_InsertItem(hwndLV, &lvItem);
	if ((lvItem.iItem != -1))
	{
		if (!(bhfi.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY))
		{
			ListView_SetItemText(hwndLV, lvItem.iItem, 1, (LPWSTR)(L"����"));
		}
		else
			if (bhfi.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
					ListView_SetItemText(hwndLV, lvItem.iItem, 1, (LPWSTR)(L"����� � �������"));
			}

	}

	// ������ ���������
	constexpr DWORD attr[] = {
		FILE_ATTRIBUTE_READONLY, FILE_ATTRIBUTE_HIDDEN, FILE_ATTRIBUTE_ARCHIVE,
		FILE_ATTRIBUTE_SYSTEM, FILE_ATTRIBUTE_TEMPORARY, FILE_ATTRIBUTE_COMPRESSED, FILE_ATTRIBUTE_ENCRYPTED
	};
	// ������ ��������������� ������� ��� ���������
	constexpr DWORD ids[] = {
		IDC_ATTRIBUTE_READONLY, IDC_ATTRIBUTE_HIDDEN, IDC_ATTRIBUTE_ARCHIVE,
		IDC_ATTRIBUTE_SYSTEM, IDC_ATTRIBUTE_TEMPORARY, IDC_ATTRIBUTE_COMPRESSED, IDC_ATTRIBUTE_ENCRYPTED
	};

	// �������� ������ �������������� � �������������� ���������� �����/��������

	for (int i = 0; i < _countof(attr); ++i)
	{
		UINT uCheck = (bhfi.dwFileAttributes & attr[i]) ? BST_CHECKED : BST_UNCHECKED;
		CheckDlgButton(hwnd, ids[i], uCheck);
	}

	// ��������� ���������� ����� */
	CloseHandle(hFile);
	return TRUE;
}

BOOL __stdcall CalculateSize(LPCTSTR lpszFileName, const LPWIN32_FILE_ATTRIBUTE_DATA lpFileAttributeData, LPVOID lpvParam)
{
	if (lpFileAttributeData->dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
	{
		// ��������� ����� ������ ��������
		return FileSearch(TEXT("*"), lpszFileName, CalculateSize, lpvParam);
	} // if
	return TRUE; // ���������� TRUE, ����� ���������� �����
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

	// ����������� ���� � ����� �� FILETIME � SYSTEMTIME
	BOOL bRet = FileTimeToSystemTime(lpFileTime, &st);

	// �������� ���� � ����� � �������� �������� �����
	if (FALSE != bRet)
		bRet = SystemTimeToTzSpecificLocalTime(NULL, &st, &st);

	if (FALSE != bRet)
	{
		// ��������� ���� � �������������� ������
		GetDateFormat(LOCALE_USER_DEFAULT, DATE_LONGDATE, &st, NULL, lpszFileTime, cchFileTime);

		// ������� ����� � �������������� ������

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
		StringCchPrintf(lpszBuffer, cch, TEXT("%.1f ��"), (size.QuadPart / (float)0x40000000ULL));
	} // if
	else if (size.QuadPart >= 0x100000ULL)
	{
		StringCchPrintf(lpszBuffer, cch, TEXT("%.1f ��"), (size.QuadPart / (float)0x100000ULL));
	} // if
	else if (size.QuadPart >= 0x0400ULL)
	{
		StringCchPrintf(lpszBuffer, cch, TEXT("%.1f ��"), (size.QuadPart / (float)0x0400ULL));
	} // if
	else
	{
		StringCchPrintf(lpszBuffer, cch, TEXT("%u ����"), size.LowPart);
	} // else

	size_t len = _tcslen(lpszBuffer);

	if (len < cch)
	{
		StringCchPrintf((lpszBuffer + len), (cch - len), TEXT(" (%llu ����)"), size.QuadPart);
	} // if
} // StringCchPrintFileSize

void ConvertDirectSize(LPTSTR lpszBuffer, DWORD cch, ULARGE_INTEGER size)
{
	if (size.QuadPart >= 0x40000000ULL)
	{
		StringCchPrintf(lpszBuffer, cch, TEXT("%.1f ��"), (size.QuadPart / (float)0x40000000ULL));
	} // if
	else if (size.QuadPart >= 0x100000ULL)
	{
		StringCchPrintf(lpszBuffer, cch, TEXT("%.1f ��"), (size.QuadPart / (float)0x100000ULL));
	} // if
	else if (size.QuadPart >= 0x0400ULL)
	{
		StringCchPrintf(lpszBuffer, cch, TEXT("%.1f ��"), (size.QuadPart / (float)0x0400ULL));
	} // if
	else
	{
		StringCchPrintf(lpszBuffer, cch, TEXT("%u ����"), size.LowPart);
	} // else

	size_t len = _tcslen(lpszBuffer);

	if (len < cch)
	{
		StringCchPrintf((lpszBuffer + len), (cch - len), TEXT(" (%llu ����)"), size.QuadPart);
	} // if
} // StringCchPrintFileSize


//work wit regist
LSTATUS RegGetValueSZ(HKEY hKey, LPCTSTR lpValueName, LPTSTR lpszData, DWORD cch, LPDWORD lpcchNeeded)
{
	// ���������� ��� ����������� �������� ���������
	DWORD RegType;
	LSTATUS retCode = RegQueryValueEx(hKey, lpValueName, NULL, &RegType, NULL, NULL);
	if (ERROR_SUCCESS == retCode && REG_SZ == RegType)
	{
		DWORD DataBuffer = cch * sizeof(TCHAR);
		// �������� �������� ���������
		retCode = RegQueryValueEx(hKey, lpValueName, NULL, NULL, (LPBYTE)lpszData, &DataBuffer);
	}
	else if (ERROR_SUCCESS == retCode)
	{
		retCode = ERROR_UNSUPPORTED_TYPE; // �������� ��� ������
	}

	return retCode;
}

LSTATUS RegGetValueBinary(HKEY hKey, LPCTSTR lpValueName, LPBYTE lpData, DWORD cb, LPDWORD lpcbNeeded)
{
	DWORD dwType;
	// ���������� ��� ����������� �������� ���������
	LSTATUS lStatus = RegQueryValueEx(hKey, lpValueName, NULL, &dwType, NULL, NULL);

	if (ERROR_SUCCESS == lStatus && REG_BINARY == dwType)
	{
		// �������� �������� ���������
		lStatus = RegQueryValueEx(hKey, lpValueName, NULL, NULL, lpData, &cb);

		if (NULL != lpcbNeeded) *lpcbNeeded = cb;
	} // if
	else if (ERROR_SUCCESS == lStatus)
	{
		lStatus = ERROR_UNSUPPORTED_TYPE; // �������� ��� ������
	} // if

	return lStatus;
} // RegGetValueBinary

/*�����������*/
BOOL GetFileSecurityDescriptor(LPCWSTR lpFileName, SECURITY_INFORMATION RequestedInformation, PSECURITY_DESCRIPTOR *ppSD)
{
	DWORD cb = 0;

	// ��������� ������ ����������� ������������
	GetFileSecurity(lpFileName, RequestedInformation, NULL, 0, &cb);

	// ������� ������ ��� ����������� ������������
	PSECURITY_DESCRIPTOR pSD = (PSECURITY_DESCRIPTOR)LocalAlloc(LMEM_FIXED, cb);
	if (NULL == pSD) return FALSE;

	// ������� ���������� ������������
	BOOL bRet = GetFileSecurity(lpFileName, RequestedInformation, pSD, cb, &cb);

	if (FALSE != bRet)
	{
		*ppSD = pSD; // ���������� ���������� ���������� ������������
	} // if
	else
	{
		//LocalFree(pSD); // ����������� ���������� ������
	} // else

	return bRet;
} // GetFileSecurityDescriptor

BOOL GetItemFromDACL(PSECURITY_DESCRIPTOR pSD, PULONG pcCountOfEntries, PEXPLICIT_ACCESS *pListOfEntries)
{
	PACL pDacl = NULL;
	BOOL bDaclPresent = FALSE, bDaclDefaulted = FALSE;

	// �������� DACL
	BOOL bRet = GetSecurityDescriptorDacl(pSD, &bDaclPresent, &pDacl, &bDaclDefaulted);

	if (FALSE != bRet && FALSE != bDaclPresent)
	{
		// ��������� �������� �� DACL
		DWORD dwResult = GetExplicitEntriesFromAcl(pDacl, pcCountOfEntries, pListOfEntries);
		bRet = (ERROR_SUCCESS == dwResult) ? TRUE : FALSE;
	} // if
	else
	{
		*pcCountOfEntries = 0; // ���������� 0 ���������
	} // else

	return bRet;
}

/*��������� ����� �������� �� ��� SID*/
BOOL GetAccountName_W(PSID psid, LPWSTR* AccountName)
{
	BOOL RetRes = FALSE;
	SID_NAME_USE SidType;//���������� �������������� ����, ���� �������� ������������ ��� SID
	/*���������� ��� ������*/
	LPWSTR Name = NULL;
	DWORD cch = 0, cchRefDomainName = 0;

	if (IsValidSid(psid) == FALSE)
	{
		return FALSE;
	}

	LookupAccountSid(NULL, psid, NULL, &cch, NULL, &cchRefDomainName, NULL);//��������� ������� �������

	DWORD cb = (cch + cchRefDomainName) * sizeof(TCHAR);

	if (cb > 0)
	{

		Name = (LPWSTR)LocalAlloc(LMEM_FIXED, cb);//��������� ������ �� ��������� ���� ��������
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
				StringCbCopy(Name, cb, Name + 1);//����������� ��� �������� � ������� ��������
			}
		}
		*AccountName = Name; //������ ����������� ��� � ���������
	}
	else
	{
		ConvertSidToStringSid(psid, AccountName);//���� �� ���������� �������� ���, �� ������ SID
		if (Name != NULL)
		{
			LocalFree(Name);
		}
	}
	return RetRes;
}