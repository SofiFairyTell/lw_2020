
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
		HWND hwndCtl = GetDlgItem(hwnd,	IDC_EDIT_TEXT);
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
	
	CreateWindowEx(0, TEXT("Static"), TEXT("��������:"), WS_CHILD | WS_VISIBLE | SS_SIMPLE,
		30, 220, 80, 20, hwnd, NULL, lpCRStr->hInstance, NULL);
	CreateWindowEx(0, TEXT("Edit"), NULL, WS_CHILD | WS_VISIBLE | WS_BORDER, 110, 220, 310, 20, hwnd, (HMENU)IDC_EDIT_OWNER, lpCRStr->hInstance, NULL);

	CreateWindowEx(0, TEXT("Static"), TEXT("�������� ��:"), WS_CHILD | WS_VISIBLE | SS_SIMPLE,
		30, 250, 80, 20, hwnd, NULL, lpCRStr->hInstance, NULL);
	CreateWindowEx(0, TEXT("Edit"), NULL, WS_CHILD | WS_VISIBLE | WS_BORDER, 110, 250, 310, 20, hwnd, (HMENU)IDC_NEW_OWNER, lpCRStr->hInstance, NULL);


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
		
	}

	case ID_CHANGE_OWNER://�������������� ��� ���������� ���������
	{
		/*//������?
		HWND hHide = GetDlgItem(hwnd, IDC_LIST1);
		ShowWindow(hHide, SW_HIDE);
		/*��!*/

		LPWSTR Owner = NULL;
		//RetRes = GetOwnerName_W(Sec_Descriptor, &Owner);//�������� ����� ���������
		GetDlgItemText(hwnd, IDC_EDIT_OWNER, Owner, sizeof(Owner));// �������� ��� ������� ������ ��������� � ���� "������� ��������"

		TCHAR NewOwner[UNLEN + 1]; // ����� ��� ���������

		GetDlgItemText(hwnd, IDC_NEW_OWNER, NewOwner, _countof(NewOwner));//��� ��� � ��� � ��������� lpszFileName
		/*����� ���*/
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
					MessageBox(hwnd, TEXT("���� ������������ �� ����� ���� ���������� ����� ��� ��������"), NULL, MB_OK | MB_ICONERROR);
					//����� ������� ����� ������������ � ������ DALC. �� ����� ��� ���� ������ ��� ����
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
						MessageBox(hwnd, TEXT("�� ������� ������� ���������. ��������� ������������ ���������� ����� ������������."), NULL, MB_OK | MB_ICONERROR);
					}
				}
				else
				{
					MessageBox(hwnd, TEXT("�� ������� ������� ���������. ��������� ������������ ���������� ����� ������������."), NULL, MB_OK | MB_ICONERROR);
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
		int mdRes = DialogBox(GetWindowInstance(hwnd), MAKEINTRESOURCE(IDD_DIALOG_ACE), hwnd, DialogAceProc);// �������� ���������� ���� "������� ����������"

		if (IDOK == mdRes)
		{	
			ListViewInit(FileName, hwnd);// ������� ������ � ������������
		} 
	}
	break;
	case ID_DELETE_ACE:
	{
		// ������ ��������� DACL - ������ ����������������� �������� �������
		HWND hwndLV = GetDlgItem(hwnd, IDC_DACL);
		BOOL RetRes = FALSE;
		PACL lpDacl;
		BOOL bDaclPresent, bDaclDefaulted;
		DWORD dwRetCode;       // ��� ��������
		/*for (;;)
		{*/
			
		int item = ListView_GetNextItem(hwndLV, -1, LVNI_SELECTED);// ������� ���������� ������� � ������ ��������� DACL

		if (item == -1) // ���� ��� ���������� ���������
		{
			break; 
		} 

		// �������� ACE - �������� �������� ������� �� ����������� ������������
		//DeleteEntryFromDalc(Sec_Descriptor, iItem);

		// �������� DACL
		//RetRes = GetSecurityDescriptorDacl(Sec_Descriptor, &bDaclPresent, &lpDacl, &bDaclDefaulted);

		// �������� ������ DACL �� ����������� ������������
		if (!GetSecurityDescriptorDacl(
			Sec_Descriptor,              // ����� ����������� ������������
			&bDaclPresent,     // ������� ����������� ������ DACL
			&lpDacl,           // ����� ��������� �� DACL
			&bDaclDefaulted))  // ������� ������ DACL �� ���������
				{
					dwRetCode = GetLastError();
					MessageBox(hwnd, TEXT("�� ������� ������� ���������. ��������� ������������ ���������� ����� ������������.", dwRetCode), NULL, MB_OK | MB_ICONERROR);
					//printf("Get security descriptor DACL failed.\n");
					//printf("Error code: %d\n", dwRetCode);
					break;
				}
		else
		{
				RetRes = DeleteAce(lpDacl, item);// ������� ������� �� DACL
		}

		
		ListView_DeleteItem(hwndLV, item);// ������� ������� �� ������ ��������� DACL
		//} // for

		SetFileSecurity(FileName, DACL_SECURITY_INFORMATION, Sec_Descriptor); 		// �������� ���������� ������������ � �����/��������
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
	/*���������� �����*/
	LPTSTR lpFN = PathFindFileNameW(path);
	SetDlgItemText(hwnd, IDC_EDIT_FILENAME, lpFN);

	/*������������� ������*/
	HWND hwndLV = GetDlgItem(hwnd, IDC_LIST1);
	ListView_DeleteAllItems(hwndLV); //������� ������ ���������

	// ����������� ���������� ������
	if (NULL != Sec_Descriptor) 
		LocalFree(Sec_Descriptor), 
		Sec_Descriptor = NULL;

	
	RetRes = GetFileSecurityDescriptor(FileName, OWNER_SECURITY_INFORMATION | DACL_SECURITY_INFORMATION, &Sec_Descriptor);// ��������� ����������� ������������

	if (RetRes != FALSE)
	{
		LPWSTR Owner = NULL; 
		RetRes = GetOwnerName_W(Sec_Descriptor, &Owner);//�������� ����� ���������
		SetDlgItemText(hwnd, IDC_EDIT_OWNER, Owner);// �������� ��� ������� ������ ��������� � ���� "������� ��������"
		LocalFree(Owner);	
	}

	ULONG uCount = 0; // ���������� ��������� � ������� ACE
	PEXPLICIT_ACCESS pEA = NULL; // ������ ACE

	RetRes = GetItemFromDACL(Sec_Descriptor, &uCount, &pEA);//������ ���������� ��������� � DACL

	if (RetRes != FALSE)
	{
		for (ULONG i = 0; i < uCount; ++i)
		{
			LVITEM lvItem = { LVIF_TEXT | LVIF_PARAM };

			lvItem.iItem = (int)i;

			lvItem.lParam = (LPARAM)pEA[i].grfAccessPermissions;			// ��������� ����� �������

			// ��������� ��� ACE
			switch (pEA[i].grfAccessMode)
			{
			case GRANT_ACCESS:
				lvItem.pszText = (LPTSTR)TEXT("���������");
				break;
			case DENY_ACCESS:
				lvItem.pszText = (LPTSTR)TEXT("���������");
				break;
			} 

			// ��������� ����� ������� � ������ ��������� DACL
			int iItem = ListView_InsertItem(hwndLV, &lvItem);
			if (iItem == -1) 
				continue; // �� ������� �������� ����� �������

			 // ��������� ��� ������� ������
			if (TRUSTEE_IS_SID == pEA[i].Trustee.TrusteeForm)
			{
				LPTSTR lpszName = NULL; // ��� ������� ������
				
				GetAccountName_W(pEA[i].Trustee.ptstrName, &lpszName);// ������� ��� ������� ������

				if (NULL != lpszName)
				{
					
					ListView_SetItemText(hwndLV, iItem, 1, lpszName);// �������� ��� ������� ������ � ������ ������ ��������� DACL

					LocalFree(lpszName);
				} 
			} 
			
			DWORD grfInheritance = pEA[i].grfInheritance & (~INHERIT_NO_PROPAGATE);// ��������� � ����� �������� ����������� ����� �������

			for (int j = 0; j < _countof(dwInherit); ++j)
			{
				if (grfInheritance == dwInherit[j]) // ������� ��������
				{
					
					ListView_SetItemText(hwndLV, iItem, 2, (LPTSTR)szInheritText[j]);// �������� � ������ ������ ��������� DACL �������� ��� �������� ���� grfInheritance

					break; // ������� �� �����
				} 
			} 
		}
	}

	return TRUE;
}


/*�����������*/
BOOL GetFileSecurityDescriptor(LPCWSTR lpFileName, SECURITY_INFORMATION RequestedInformation, PSECURITY_DESCRIPTOR *ppSD)
{
	DWORD lpnLengthNeeded = 0;

	GetFileSecurity(lpFileName, RequestedInformation, NULL, 0, &lpnLengthNeeded);	// ��������� ������ ����������� ������������
	
	PSECURITY_DESCRIPTOR pSD = (PSECURITY_DESCRIPTOR)LocalAlloc(LMEM_FIXED, lpnLengthNeeded);// ��������� ������ ��� ����������� ������������
	if (NULL == pSD) return FALSE;
		
	BOOL RetRes = GetFileSecurity(lpFileName, RequestedInformation, pSD, lpnLengthNeeded, &lpnLengthNeeded);// ������� ���������� ������������

	if (FALSE != RetRes)
	{
		*ppSD = pSD; // ���������� ���������� ���������� ������������
	} 
	else
	{
		LocalFree(pSD); // ����������� ���������� ������
	} 

	return RetRes;
} 

BOOL GetItemFromDACL(PSECURITY_DESCRIPTOR pSecurityDescriptor, PULONG pcCountOfEntries, PEXPLICIT_ACCESS *pListOfEntries)
{
	PACL pDacl = NULL;// ��������� �� ������ ���������� ��������
	BOOL lpbDaclPresent = FALSE; // ������� ����������� ������ DACL
	BOOL lpbDaclDefaulted = FALSE; // ������� ������ DACL �� ���������
	BOOL RetRes;
	DWORD Result; //��� ���������� ���������� �������� �� DALC
	


	// �������� DACL
	RetRes = GetSecurityDescriptorDacl(pSecurityDescriptor, &lpbDaclPresent, &pDacl, &lpbDaclDefaulted);

	if (RetRes != FALSE && lpbDaclPresent != FALSE)
	{
		// ������ �������� �� ������ DACL
		Result = GetExplicitEntriesFromAcl(
			pDacl,				// ����� ������ DACL
			pcCountOfEntries,	// ����� ��� ���������� ���������
			pListOfEntries);	// ����� ��������� �� �����

		RetRes = (ERROR_SUCCESS == Result) ? TRUE : FALSE;
	} 
	else
	{
		*pcCountOfEntries = 0; // ���������� 0 ���������
	} 

	return RetRes;
}
/*��������� ����� �������� �� ��� SID � �� ����� ��������*/
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

/*����������� ������� ������������ SID*/
BOOL GetAccountSID_W(LPCWSTR AccountName, PSID *ppsid)
{
	BOOL RetRes = FALSE;
	SID_NAME_USE SidType;//���������� �������������� ����, ���� �������� ������������ ��� SID

	/*���������� ��� ����������� ����� � SID*/
	LPWSTR RefDomainName = NULL;
	PSID psid = NULL;
	DWORD cbSID = 0, cchRefDomainName = 0;

	LookupAccountNameW(NULL, AccountName, NULL, &cbSID, NULL, &cchRefDomainName, NULL);//����������� �������� ������ ��� �����

	if ((cbSID > 0) && (cchRefDomainName > 0))
	{
		psid = (PSID)LocalAlloc(LMEM_FIXED, cbSID); //��������� ������ �� ��������� ���� ��������
		RefDomainName = (LPWSTR)LocalAlloc(LMEM_FIXED, cchRefDomainName * sizeof(WCHAR));// -||- ��� ����� ������
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
			LocalFree(psid);//����������� ������
		}
	}

	if (RefDomainName != NULL)
	{
		LocalFree(RefDomainName);//����������� ������
	}

	return RetRes;
}


/*��������� ����� ������������ �� ����������� ������������*/
BOOL GetOwnerName_W(PSECURITY_DESCRIPTOR Sec_Descriptor, LPWSTR *OwnerName)
{
	PSID psid;
	BOOL bDefaulted;
		
	BOOL bRet = GetSecurityDescriptorOwner(Sec_Descriptor, &psid, &bDefaulted);// �������� SID ���������

	if (FALSE != bRet)
	{
		bRet = GetAccountName_W(psid, OwnerName);// ���������� ��� ������� ������ ���������
	} 
	return bRet;
}

/*��������� ���������� � ����������� ������������*/
BOOL SetFileSecurityInfo(LPCTSTR FileName, LPWSTR NewOwner,ULONG CountOfEntries, PEXPLICIT_ACCESS pListOfEntries, BOOL bMergeEntries)
{
	BOOL RetRes = FALSE;
	SECURITY_DESCRIPTOR secur_desc;
	
	/*�������� ������ ��� ����� ��������*/
	PSID psid_Owner = NULL;
	PACL pNewDacl = NULL;

	RetRes = InitializeSecurityDescriptor(&secur_desc, SECURITY_DESCRIPTOR_REVISION);

	if (RetRes != FALSE && NewOwner != NULL)
	{
		
		RetRes = GetAccountSID_W(NewOwner, &psid_Owner);//�� ����� ��������� ������� ��� SID
			

		if (RetRes != FALSE)
		{
			RetRes = SetSecurityDescriptorOwner(&secur_desc, psid_Owner, FALSE);//��� ����� ����������� � SID
		}
	}

	if (RetRes != FALSE && CountOfEntries > 0 && pListOfEntries != NULL)
	{
		PSECURITY_DESCRIPTOR OldSD = NULL;
		PACL pOldDacl = NULL; // ��������� �� ����� ��� DACL

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

	// ��������� ��������� ����������� ������������
	if (!IsValidSecurityDescriptor(&secur_desc))
	{
		DWORD dwRetCode = GetLastError();

	}



	if (RetRes != NULL)
	{
		SECURITY_INFORMATION si = 0;
		if (psid_Owner != NULL) si |= OWNER_SECURITY_INFORMATION;
		if (pNewDacl != NULL) si |= DACL_SECURITY_INFORMATION;

		/*���� ������������ ������� �� ����������� ������ ��������������� �������� �������� ���� �� ���������
		�� ��� �������� � ��������� ������ INVALID_OWNER*/
		RetRes = SetFileSecurity(FileName, si, &secur_desc); //�������� ���������� ������������ ��� �����
	}

	/*������������ ������*/
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
	PEXPLICIT_ACCESS pEA = (PEXPLICIT_ACCESS)lParam; // ��������� �� ������ ��������� ����
													// EXPLICIT_ACCESS
	DWORD grfAccessMode;// ��� ��������� ������ �������
	HWND hwndCtl;//���������� ��� ������

	BOOL fEditable = (NULL == pEA) ? TRUE : FALSE; // ����, ������������ ����������� ������������� ������� ACE


	grfAccessMode = (NULL != pEA) ? pEA->grfAccessMode : GRANT_ACCESS;

	// ��������� ���������� ������ "���": ��������� ��� ���������
	hwndCtl = GetDlgItem(hwnd, IDC_ACCESS_MODE);
	EnableWindow(hwndCtl, fEditable);

	int iItem = ComboBox_AddString(hwndCtl, TEXT("���������"));

	if (iItem != -1)
	{
		ComboBox_SetItemData(hwndCtl, iItem, GRANT_ACCESS);
		if (GRANT_ACCESS == grfAccessMode) ComboBox_SetCurSel(hwndCtl, iItem);
	} // if

	iItem = ComboBox_AddString(hwndCtl, TEXT("���������"));

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

		LPTSTR AccountName = NULL; // ��� ������� ������

		GetAccountName_W(pEA->Trustee.ptstrName, &AccountName);	// ������� ��� ������� ������

		if (AccountName != NULL)
		{
			// �������� ��� ������� ������ � ���� "���"
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


	if (FALSE != fEditable) // ��������� ������������� ������� ACE
	{
		// ������� �������� �����/��������
		DWORD dwFileAttributes = GetFileAttributes(FileName);

		// ��������� �����, �� ������������� ������� ACE (������ ��� ���������)
		fEditable = ((INVALID_FILE_ATTRIBUTES != dwFileAttributes) && (dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) ? TRUE : FALSE;
	} 


	// ��������� ���������� ������ "���������"

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

		if (IsWindowEnabled(hwndCtl) != FALSE) // �������� ����� ACE
		{
			PSID pSid = NULL; // ��������� �� ���������� ������������

			int cch = GetWindowTextLength(hwndCtl);

			if (cch > 0)
			{
				LPTSTR lpszName = new TCHAR[cch + 1]; // ��� ������� ������

				// �������� ��� ������� ������ �� ���� �����
				GetDlgItemText(hwnd, IDC_EDIT_NAME, lpszName, cch + 1);
				// ������� SID ������� ������
				GetAccountSID_W(lpszName, &pSid);

				// ����������� ���������� ������
				delete[] lpszName, lpszName = NULL;
			} 

			if (NULL != pSid)
			{
				EXPLICIT_ACCESS ea;

				ea.Trustee.TrusteeForm = TRUSTEE_IS_SID; // ���������� ������ �������� � ������� SID
				ea.Trustee.ptstrName = (LPTSTR)pSid; // SID

				// ��������� ����������

				ea.grfAccessPermissions = 0;

				for (int i = 0; i < 13; ++i)
				{
					if (IsDlgButtonChecked(hwnd, idcPermissions[i]) == BST_CHECKED)
					{
						ea.grfAccessPermissions |= dwPermissions[i];
					} 
				} 

				// ��������� ��� ����������

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
					// �������� ���������� � ����������� ������������
					BOOL RetRes = SetFileSecurityInfo(FileName, NULL, 1, &ea, TRUE);

					if (RetRes != FALSE)
					{					
						EndDialog(hwnd, IDOK);// ��������� ���������� ����
					} 
				} 
				LocalFree(pSid);
			} 
			else
			{
				MessageBox(hwnd, TEXT("�� ������� ����� ������� ������. ��������� ������������ ���������� �����."), NULL, MB_OK | MB_ICONERROR);
			} 
		} 
		else
		{
			EndDialog(hwnd, IDOK);	// ��������� ���������� ����
		} 
	}
	break;

	case IDCANCEL:
		EndDialog(hwnd, IDCANCEL);
		break;

	case IDC_BUTTON_NAME_TEST: // ���������
	{
		int cch = GetWindowTextLength(GetDlgItem(hwnd, IDC_EDIT_NAME));

		if (cch > 0)
		{
			PSID pSid = NULL;

			LPTSTR AccountName = new TCHAR[cch + 1]; // ��� ������� ������
			
			GetDlgItemText(hwnd, IDC_EDIT_NAME, AccountName, cch + 1);// �������� ��� ������� ������ �� ���� �����
			
			GetAccountSID_W(AccountName, &pSid);		// ������� SID ������� ������

			delete[] AccountName, AccountName = NULL;	// ����������� ���������� ������

			if ( pSid != NULL)
			{		
				GetAccountName_W(pSid, &AccountName);	// ������� ��� ������� ������
				
				SetDlgItemText(hwnd, IDC_EDIT_NAME, AccountName);	// �������� ��� ������� ������ � ���� �����
				
				if (AccountName != NULL)
					LocalFree(AccountName);// ����������� ���������� ������

				LocalFree(pSid);

				MessageBox(hwnd, TEXT("������� ������ ����."), NULL, MB_OK | MB_ICONERROR);
			} 
			else
			{
				MessageBox(hwnd, TEXT("�� ������� ����� ������� ������. ��������� ������������ ���������� �����."), NULL, MB_OK | MB_ICONERROR);

				// ������� ����� �� ���� "���"
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
//	// �������� DACL
//	BOOL bRet = GetSecurityDescriptorDacl(pSD, &bDaclPresent, &pDacl, &bDaclDefaulted);
//
//	if (FALSE != bRet)
//	{
//		// ������� ������� �� DACL
//		bRet = DeleteAce(pDacl, dwIndex);
//	} // if
//
//	return bRet;
//} // DeleteEntryFromDalc