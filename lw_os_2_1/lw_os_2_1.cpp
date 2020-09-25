#include<Windows.h>
#include <windowsx.h>
#include <CommCtrl.h>
#include <tchar.h>
#include "resource.h"

HWND hWnd = NULL;
HWND hSaveDlg = NULL; //save as
HWND hRlsDlg = NULL; //replace

HACCEL hAccel = NULL;

TCHAR szBuffer[100] = TEXT("");

LRESULT CALLBACK MyWindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
