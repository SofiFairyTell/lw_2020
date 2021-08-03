//#include <commctrl.h>
//#include <Windows.h>
//#pragma comment(lib, "C:\\Program Files\\Microsoft SDKs\\Windows\\v7.1\\Lib\\ComCtl32.Lib")
//
//#pragma comment(linker,"\"/manifestdependency:type                  = 'win32' \
//                                              name                  = 'Microsoft.Windows.Common-Controls' \
//                                              version               = '6.0.0.0' \
//                                              processorArchitecture = '*' \
//                                              publicKeyToken        = '6595b64144ccf1df' \
//                                              language              = '*'\"")
//
//
//INITCOMMONCONTROLSEX icex;    // Structure for control initialization.
//
//const UINT valMin = 0;          // The range of values for the Up-Down control.
//const UINT valMax = 100;
//
//RECT rcClient;               // Client area of parent window (Dialog Box).
//UINT cyVScroll;              // Height of scroll bar arrow.
//
//HWND hControl = NULL;  // Handles to the controls.
//HWND hwndGroupBox = NULL;
//HWND hwndLabel = NULL;
//HWND hwndUpDnEdtBdy = NULL;
//HWND hwndUpDnCtl = NULL;
//HWND hwndProgBar = NULL;
//
//HWND CreateGroupBox(HWND);   // Handles to the create controls functions.
//HWND CreateLabel(HWND);
//HWND CreateUpDnBuddy(HWND);
//HWND CreateUpDnCtl(HWND);
//HWND CreateProgBar(HWND);
//
//INT_PTR CALLBACK UpDownDialogProc(HWND, UINT, WPARAM, LPARAM);
//
//INT_PTR CALLBACK UpDownDialogProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
//{
//	UINT nCode;
//	int iPosIndicated;
//	LPNMUPDOWN lpnmud;
//
//	switch (message)
//	{
//	case WM_INITDIALOG:
//		GetClientRect(hDlg, &rcClient);
//
//		cyVScroll = GetSystemMetrics(SM_CYVSCROLL);
//
//		hwndGroupBox = CreateGroupBox(hDlg);   // Group the controls.
//		hwndLabel = CreateLabel(hDlg);      // Create a label for the Up-Down control.
//		hwndUpDnEdtBdy = CreateUpDnBuddy(hDlg);  // Create an buddy window (an Edit control).
//		hwndUpDnCtl = CreateUpDnCtl(hDlg);    // Create an Up-Down control.
//		hwndProgBar = CreateProgBar(hDlg);    // Create a Progress bar,
//												 // to reflect the state of the Up-down control.
//
//		return (INT_PTR)TRUE;
//
//	case WM_NOTIFY:
//		nCode = ((LPNMHDR)lParam)->code;
//
//		switch (nCode)
//		{
//		case UDN_DELTAPOS:
//			lpnmud = (LPNMUPDOWN)lParam;
//			iPosIndicated = SendMessage(hwndProgBar, PBM_GETPOS, (WPARAM)0, (LPARAM)0);
//
//			SendMessage(hwndProgBar,
//				PBM_SETPOS,
//				(WPARAM)(iPosIndicated + lpnmud->iDelta),  // iPosIndicated can have
//				(LPARAM)0);                                // any signed integer value.
//
//			break;
//		}
//
//	case WM_COMMAND:
//		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
//		{
//			EndDialog(hDlg, LOWORD(wParam));
//			return TRUE;
//		}
//		break;
//	}
//	return FALSE;
//}