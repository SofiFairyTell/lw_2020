#include "FileCopeDialogHeader.h"
#include <string>

/*Ïîêà íå ðàçáåðóñü æèâåò çäåñü*/
// ôóíêöèÿ äëÿ âõîäà ïîëüçîâàòåëÿ íà ëîêàëüíûé êîìïüþòåð
HANDLE LogonUserToLocalComputer();
HANDLE OpenUserToken(
	LPCTSTR lpUserName, LPCTSTR lpDomain, LPCTSTR lpPassword, 
	DWORD LogonType, DWORD DesireAcces, PSECURITY_ATTRIBUTES PSECUR_ATTRIB,
	TOKEN_TYPE TOKEN_TYP, SECURITY_IMPERSONATION_LEVEL IMPERSONATION_LEVEL);

int WINAPI _tWinMain(HINSTANCE hInstance, HINSTANCE, LPTSTR lpszCmdLine, int nCmdShow)
{

	HINSTANCE relib = LoadLibrary(TEXT("riched32.dll"));    //load the dll don't forget this   
											//and don't forget to free it (see wm_destroy) 
	if (relib == NULL)
		MessageBox(NULL, TEXT("Failed to load riched32.dll!"), TEXT("Error"), MB_ICONEXCLAMATION);
	HACCEL hAccel = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDR_ACCELERATOR1));

	WNDCLASSEX wcex = { sizeof(WNDCLASSEX) };

	wcex.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
	wcex.lpfnWndProc = MainWindowProc; // îêîííàÿ ïðîöåäóðà
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_BTNFACE + 2);
	
	wcex.lpszClassName = TEXT("MainWindowClass"); // èìÿ êëàññà
	wcex.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

	if (0 == RegisterClassEx(&wcex)) // ЕСЛИ НЕ УДАЛОСЬ ЗАРЕГЕСТРИРОВАТЬ КЛАСС
	{
		return -1; 
	}

	LoadLibrary(TEXT("ComCtl32.dll"));//äëÿ ýëåìåíòîâ îáùåãî ïîëüçîâàíèÿ
	// ñîçäàåì ãëàâíîå îêíî íà îñíîâå íîâîãî îêîííîãî êëàññà
	

	HWND hWnd = CreateWindowEx(0, TEXT("MainWindowClass"), TEXT("Process"), WS_OVERLAPPEDWINDOW,
		100, 100, 10, 10, NULL, NULL, hInstance, NULL);

	if (NULL == hWnd)
	{
		return -1; // çàâåðøàåì ðàáîòó ïðèëîæåíèÿ
	}

	ShowWindow(hWnd, SW_HIDE); // îòîáðàæàåì ãëàâíîå îêíî
	


	MSG  msg;
	BOOL Ret;

	for (;;)
	{

		// èçâëåêàåì ñîîáùåíèå èç î÷åðåäè
		Ret = GetMessage(&msg, NULL, 0, 0);
		if (Ret == FALSE)
		{
			break; // ïîëó÷åíî WM_QUIT, âûõîä èç öèêëà
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
	case WM_CREATE:
	{
		HINSTANCE hInstance = GetWindowInstance(hwnd);
		HWND hDlg = CreateDialog(hInstance, MAKEINTRESOURCE(IDD_DIALOG1), hwnd, DialogProc);
		ShowWindow(hDlg, SW_SHOW);
	}
	break;
	
	}

	return DefWindowProc(hwnd, msg, wParam, lParam);
}


INT_PTR CALLBACK DialogProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	TCHAR FileName[260];
	BROWSEINFO bi;//structure for open special box with folder in treview
	HDC hdc;
	LPITEMIDLIST pidl;
	LPMALLOC  pMalloc = NULL;
	switch (uMsg)
	{
	case WM_LBUTTONDOWN:
	{
		DWORD xPos, yPos, nSize;
		TCHAR szBuf[80];

		// Ñîõðàíÿåì êîîðäèíàòû êóðñîðà ìûøè
		xPos = LOWORD(lParam);
		yPos = HIWORD(lParam);
	
		/*Îòñëåäèì òî÷êè íàä ïåðâûì è âòîðûì editbox 
		Åñëè äà, òî îòêðîåì äëÿ ñîîòâåòñòâóþùåãî editbox îêíà äëÿ èõ çàïîëíåíèÿ*/
		if ((xPos > 312 & xPos < 544)&(yPos>39&yPos<81))
		{
			//Â êàêóþ äèðåêòîðèþ ñêîïèðîâàòü
				//TextOut(hdc, xPos, yPos, szBuf, nSize);
				ZeroMemory(&bi, sizeof(bi));
				bi.hwndOwner = NULL;
				bi.pszDisplayName = FileName;
				bi.lpszTitle = TEXT("Select folder");
				bi.ulFlags = BIF_RETURNONLYFSDIRS;

				pidl = SHBrowseForFolder(&bi);//open window for select
				if (pidl)
				{
					SHGetPathFromIDList(pidl, FileName);//get path
					SetDlgItemText(hwndDlg, IDC_EDIT_TO,FileName);
				}
				
				
		}
		else 
			if ((xPos > 36 & xPos < 250)&(yPos > 39 & yPos < 81))
			{
					ZeroMemory(&bi, sizeof(bi));
					bi.hwndOwner = NULL;
					bi.pszDisplayName = FileName;
					bi.lpszTitle = TEXT("Select folder");
					bi.ulFlags = BIF_BROWSEINCLUDEFILES;
					pidl = SHBrowseForFolder(&bi);//open window for select
					if (pidl)
					{
						SHGetPathFromIDList(pidl, FileName);//get path
						SetDlgItemText(hwndDlg, IDC_EDIT_FROM, FileName);
					}
					
				}
	}break;
	case WM_INITDIALOG:
	{
		BOOL bRet = HANDLE_WM_INITDIALOG(hwndDlg, wParam, lParam, Dialog_OnInitDialog);
		return SetDlgMsgResult(hwndDlg, uMsg, bRet);
	}

	case WM_CLOSE:
		HANDLE_WM_CLOSE(hwndDlg, wParam, lParam, Dialog_OnClose);

		return TRUE;

	case WM_COMMAND:
		HANDLE_WM_COMMAND(hwndDlg, wParam, lParam, Dialog_OnCommand);
		return TRUE;
	} // switch

	return FALSE;
} // DialogProc

// ----------------------------------------------------------------------------------------------
BOOL Dialog_OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam)
{
	return TRUE;
}

void Dialog_OnClose(HWND hwnd)
{
	EndDialog(hwnd, IDCLOSE);
	DestroyWindow(hwnd); // óíè÷òîæàåì îêíî
	PostQuitMessage(0); // îòïðàâëÿåì ñîîáùåíèå WM_QUIT
}


BOOL Copy(LPCTSTR szInDirName, LPCTSTR szOutDirName)
{
	WIN32_FIND_DATA ffd;
	HANDLE hFind;

	TCHAR szFind[MAX_PATH + 1];
	TCHAR szInFileName[MAX_PATH + 1];
	TCHAR szOutFileName[MAX_PATH + 1];

	lstrcpy(szFind, szInDirName);
	lstrcat(szFind, L"\\*.*"); //èùåì ôàéëû ñ ëþáûì èìåíåì è ðûñøèðåíèåì

	hFind = FindFirstFile(szFind, &ffd);

	do
	{
		//Ôîðìèðóåì ïîëíûé ïóòü (èñòî÷íèê)
		lstrcpy(szInFileName, szInDirName);
		lstrcat(szInFileName, L"\\");
		lstrcat(szInFileName, ffd.cFileName);

		//Ôîðìèðóåì ïîëíûé ïóòü (ðåçóëüòàò)

		lstrcpy(szOutFileName, szOutDirName);
		lstrcat(szOutFileName, L"\\");
		lstrcat(szOutFileName, ffd.cFileName);
	
			if (ffd.dwFileAttributes & 0x00000010)
			{
				if (lstrcmp(ffd.cFileName, L".") == 0 || lstrcmp(ffd.cFileName, L"..") == 0) continue;

				CreateDirectory(szOutFileName, NULL);
				Copy(szInFileName, szOutFileName);
			}
		
		CopyFile(szInFileName, szOutFileName, TRUE);

	} while (FindNextFile(hFind, &ffd));

	FindClose(hFind);
	return TRUE;
}

void Dialog_OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify)
{
	switch (id)
	{

	case IDOK:
	{
		TCHAR FromName[260];
		TCHAR ToName[260];
		TCHAR NewName[MAX_PATH + 1];

		GetDlgItemText(hwnd, IDC_EDIT_FROM, FromName, _countof(FromName));
		GetDlgItemText(hwnd, IDC_EDIT_TO, ToName, _countof(ToName));//êàòàëîã êóäà êîïèðóåì
		
		/*Âûÿñíèì, ÷òî êîïèðóåòñÿ, ôàéë èëè ïàïêà. 
		Åñëè ïàïêà, òî ñôîðìèðóåì ñ íåé ìàðøðóò è ïðîäîëæèì ïîèñê*/
		WIN32_FIND_DATA ffd;
		HANDLE hFind;
		BOOL BRET;
		LPCTSTR FILE = PathFindFileNameW(FromName);
		hFind = FindFirstFile(FromName, &ffd); //Èùåì ôàéë/êàòàëîã
		if (ffd.dwFileAttributes == FILE_ATTRIBUTE_DIRECTORY)
		{
			lstrcat(ToName, L"\\");
			lstrcat(ToName, FILE);
			CreateDirectory(ToName, NULL);
			BRET = Copy(FromName, ToName);
		}
		else
		{
			lstrcat(ToName, L"\\");
			lstrcat(ToName, ffd.cFileName);
			BRET = CopyFile(FromName, ToName, TRUE);
			
		}
		
		/*Ðàáîòàþ íàä ýòèì*/
		// ïîëó÷èì êîä ïîñëåäíåé îøèáêè
		DWORD dwError = (FALSE == BRET) ? GetLastError() : ERROR_SUCCESS;

		// çàâåðøàåì îëèöèòâîðåíèå
		RevertToSelf();

		if (ERROR_ACCESS_DENIED == dwError) // (!) îøèáêà: îòêàçàíî â äîñòóïå
		{
			_tprintf(TEXT("> Îòêàçàíî â äîñòóïå.\n\n"));

			// ïîëó÷àåì ìàðêåð äîñòóïà ïîëüçîâàòåëÿ
			HANDLE hToken = LogonUserToLocalComputer();

			if (NULL != hToken)
			{
				// íà÷èíàåì îëèöèòâîðåíèå
				ImpersonateLoggedOnUser(hToken);
				// çàêðûâàåì ìàðêåð äîñòóïà
				CloseHandle(hToken);
			} // if
			else
			{
				break; // (!) âûõîäèì èç öèêëà
			} // else
		} // if
		else
		{
			SetLastError(dwError);
			break; // (!) âûõîäèì èç öèêëà
		} // else

		/*Ðàáîòàþ íàä ýòèì*/ 

		TCHAR Message[MAX_PATH];
		if (BRET == 0)
		{
			lstrcpy(Message, L"Ôàéëû íå ñêîïèðîâàíû â ïàïêó: ");
			lstrcat(Message, ToName);
			MessageBox(hwnd, Message, L"Îøèáêà", MB_OK);
			SetDlgItemText(hwnd, IDC_EDIT_FROM, L" ");
			SetDlgItemText(hwnd, IDC_EDIT_TO, L" ");
			//ShowMessage(IntToStr(result));
		}
		else
		{
			
			lstrcpy(Message, L"Ôàéëû ñêîïèðîâàíû. Ïðîâåðüòå ïàïêó: ");
			lstrcat(Message, ToName);
			MessageBox(hwnd,Message, L" Óñïåõ!", MB_OK);
			SetDlgItemText(hwnd, IDC_EDIT_FROM, L" ");
			SetDlgItemText(hwnd, IDC_EDIT_TO, L" ");
		}
				
	}	break;

	case IDCANCEL:
	{
		EndDialog(hwnd, IDCANCEL);
		DestroyWindow(hwnd); // óíè÷òîæàåì îêíî
		PostQuitMessage(0);
	}
	break;
	}
}

HANDLE LogonUserToLocalComputer()
{
	TCHAR szUserName[UNLEN + 1]; // èìÿ ïîëüçîâàòåëÿ
	TCHAR szPassword[51]; // ïàðîëü

	for (int j = 0; j < 3; ++j) // ìàêñèìàëüíîå ÷èñëî ïîïûòîê = 3
	{
		_tprintf(TEXT("> èìÿ ïîëüçîâàòåëÿ: "));
		if (!getline(szUserName, _countof(szUserName)))
		{
			break; // (!) âûõîäèì èç öèêëà
		} // if

		_tprintf(TEXT("> ïàðîëü: "));
		getline(szPassword, _countof(szPassword), _T('*'));

		_tprintf(TEXT("\n"));

		// ïîëó÷àåì ìàðêåð äîñòóïà ïîëüçîâàòåëÿ
		HANDLE hToken = OpenUserToken(szUserName, TEXT("."), szPassword,
			LOGON32_LOGON_INTERACTIVE, TOKEN_QUERY | TOKEN_IMPERSONATE, NULL, TokenImpersonation, SecurityImpersonation);

		if (NULL != hToken)
		{
			return hToken; // âîçâðàùàåì äåñêðèïòîð ìàðêåðà äîñòóïà
		} // if
	} // for

	return NULL;
} // LogonUserToLocalComputer

HANDLE OpenUserToken(LPCTSTR lpUserName, LPCTSTR lpDomain, LPCTSTR lpPassword, DWORD LogonType, DWORD DesireAcces, PSECURITY_ATTRIBUTES PSECUR_ATTRIB, TOKEN_TYPE TOKEN_TYP, SECURITY_IMPERSONATION_LEVEL IMPERSONATION_LEVEL)
{
	HANDLE TOKEN = NULL;
	BOOL BRET = LogonUser;
	return HANDLE();
}
