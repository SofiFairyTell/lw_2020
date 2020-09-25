#include <Windows.h>
#include <WindowsX.h>
#include <CommCtrl.h>
#include <tchar.h>

#include "resource.h"
//#include "afxres.h"

#define WM_ADDITEM WM_USER+1

HWND hWnd = NULL; //дескриптор окна
HACCEL hAccel = NULL; //дескриптор акселератора 
