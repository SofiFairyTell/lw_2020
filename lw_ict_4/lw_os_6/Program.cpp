#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#pragma comment(lib,"ws2_32.lib")
#pragma comment(lib,"user32.lib")
#include <Winsock2.h>
#include <Windows.h> 
#include <Windowsx.h>
#include <CommCtrl.h> 
#include <tchar.h> 
#include <Psapi.h> 
#include <strsafe.h> 
#include <process.h>

#define IDC_LIST1       		2002
#define IDC_BUTTON				2004
#define IDC_BUTTON1				2005
#define IDC_RADIO1				2006
#define IDC_RADIO2				2007
#define IDC_EDIT1				2011
#define IDC_EDIT2				2012
#define IDC_EDIT4				2014

HWND hDlg = NULL;
HWND hFindDlg = NULL;
HWND hList;
HWND hRadio;

SOCKET sock = NULL;

HFONT hFont = NULL;

sockaddr_in sockSin = { 0 };
sockaddr_in sockSout = { 0 };
sockaddr_in sockSoin = { 0 };

LRESULT CALLBACK MyWindowProc(HWND hWnd, UINT uMsg, WPARAM
	wParam, LPARAM lParam);

void OnIdle(HWND hwnd);
BOOL PreTranslateMessage(LPMSG lpMsg);

BOOL OnCreate(HWND hwnd, LPCREATESTRUCT lpCreateStruct);
void OnDestroy(HWND hwnd);
void OnSize(HWND hwnd, UINT state, int cx, int cy);
void OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify);

HANDLE hThread = NULL;

unsigned __stdcall ThreadFunc(LPVOID lpParameter);


#pragma pack(1)
struct SLP_msg
{
	int filelen;		 //длина сообщени¤
	int numberfrag;		//номер фрагмента
	WCHAR username[20]; //им¤ отправител¤
	WCHAR text[10];		//текст сообщени¤
};
#pragma pack()


int WINAPI _tWinMain(HINSTANCE hInstance, HINSTANCE, LPTSTR
	lpszCmdLine, int nCmdShow)
{

	WNDCLASSEX wcex = { sizeof(WNDCLASSEX) };
	wcex.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
	wcex.lpfnWndProc = MyWindowProc;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_BTNFACE + 1);
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = TEXT("MyWindowClass");
	wcex.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

	if (0 == RegisterClassEx(&wcex))
	{
		return -1;
	}

	LoadLibrary(TEXT("ComCtl32.dll"));
	LoadLibrary(TEXT("Ws2_32.dll"));

	HWND hWnd = CreateWindowEx(0, TEXT("MyWindowClass"), TEXT("NET4"), WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, 0,
		700, 500, NULL, NULL, hInstance, NULL);

	if (NULL == hWnd)
	{
		return -1;
	}

	ShowWindow(hWnd, nCmdShow);


	MSG msg;
	BOOL bRet;

	for (;;)
	{
		while (!PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE))
		{
			OnIdle(hWnd);
		}

		bRet = GetMessage(&msg, NULL, 0, 0);

		if (bRet == -1)
		{

		}
		else if (FALSE == bRet)
		{
			break;
		}
		else if (PreTranslateMessage(&msg) == FALSE)
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int)msg.wParam;
}
LRESULT CALLBACK MyWindowProc(HWND hWnd, UINT uMsg, WPARAM
	wParam, LPARAM lParam)
{
	switch (uMsg)
	{
		HANDLE_MSG(hWnd, WM_CREATE, OnCreate);
		HANDLE_MSG(hWnd, WM_DESTROY, OnDestroy);
		HANDLE_MSG(hWnd, WM_COMMAND, OnCommand);
		break;
	}

	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

void OnIdle(HWND hwnd)
{
}

BOOL PreTranslateMessage(LPMSG lpMsg)
{
	return IsDialogMessage(hDlg, lpMsg) ||
		IsDialogMessage(hFindDlg, lpMsg);
}


BOOL OnCreate(HWND hwnd, LPCREATESTRUCT lpCreateStruct)
{
	WSADATA wsaData;
	int err = WSAStartup(MAKEWORD(2, 2), &wsaData);
	hList = CreateWindowEx(0, TEXT("ListBox"), NULL,
		WS_CHILD | WS_VISIBLE | WS_BORDER, 10, 100, 600,
		300, hwnd, (HMENU)IDC_LIST1, lpCreateStruct->hInstance, NULL);

	CreateWindowEx(0, TEXT("Edit"), NULL,
		WS_CHILD | WS_VISIBLE | WS_BORDER | ES_MULTILINE, 10, 400, 450, 30,
		hwnd, (HMENU)IDC_EDIT1, lpCreateStruct->hInstance, NULL);

	CreateWindowEx(0, TEXT("Button"), TEXT("Отправить"),
		WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 465, 400, 100, 30,
		hwnd, (HMENU)IDC_BUTTON, lpCreateStruct->hInstance, NULL);

	hRadio = CreateWindowEx(0, TEXT("Button"), TEXT("Целевой IP-адресс"), WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON, 330, 30, 150, 25, hwnd, (HMENU)IDC_RADIO1, lpCreateStruct->hInstance, NULL);
	CreateWindowEx(0, TEXT("Button"), TEXT("Широковещательный адрес"), WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON, 330, 60, 210, 25, hwnd, (HMENU)IDC_RADIO2, lpCreateStruct->hInstance, NULL);
	Button_SetCheck(hRadio, 1);
	CreateWindowEx(0, TEXT("Edit"), NULL,
		WS_CHILD | WS_VISIBLE | WS_BORDER, 10, 60, 100, 20,
		hwnd, (HMENU)IDC_EDIT2, lpCreateStruct->hInstance, NULL);

	CreateWindowEx(0, TEXT("Edit"), NULL,
		WS_CHILD | WS_VISIBLE | WS_BORDER, 10, 30, 200, 20,
		hwnd, (HMENU)IDC_EDIT4, lpCreateStruct->hInstance, NULL);

	CreateWindowEx(0, TEXT("Button"), TEXT("Соединитьс¤"),
		WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 215, 30, 100, 20,
		hwnd, (HMENU)IDC_BUTTON1, lpCreateStruct->hInstance, NULL);

	sock = socket(AF_INET, SOCK_DGRAM, NULL); //открытие сокета
	sockSout.sin_family = AF_INET; // семейство адресов = IPv4
	sockSout.sin_port = htons(7581); // номер порта = 7581
	sockSout.sin_addr.s_addr = htonl(INADDR_ANY);
	err = bind(sock, (sockaddr*)&sockSout, sizeof(sockSout));
	if (err != SOCKET_ERROR)
	{
		hThread = (HANDLE)_beginthreadex(NULL, 0, ThreadFunc, NULL, 0, NULL);
	}
	return TRUE;
}

void OnDestroy(HWND hwnd)
{
	closesocket(sock);
	WSACleanup();
	PostQuitMessage(0);
}
void OnSize(HWND hwnd, UINT state, int cx, int cy)
{

}

void OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify)
{
	if (BN_CLICKED == codeNotify)
	{
		HINSTANCE hInstance = GetWindowInstance(hwnd);
		switch (id)
		{
		case IDC_BUTTON:
		{
			WCHAR usernames[20] = L"";
			WCHAR text[255] = L"";
			WCHAR textt[255] = L"";
			SLP_msg msg; // буфер дл¤ отправки текстового сообщени¤
			memset(msg.text, NULL, 10);
			GetDlgItemText(hwnd, IDC_EDIT2, usernames, 20);
			StringCchPrintf(msg.username, 20, usernames);
			GetDlgItemText(hwnd, IDC_EDIT1, text, 255);
			StringCchPrintf(textt, 255, L"я:   %s", text);
			ListBox_AddString(hList, textt);
			int n = 0;
			int lentext = wcslen(text); //длина текста
			msg.filelen = lentext;
			int num;
			int fragment = (lentext / 5);
			if (lentext % 5 == 0)
				num = 1;
			else num = 0;
			for (int i = 0; i < lentext; i += 5)
			{
				WCHAR frag[5] = L"";
				for (int j = 0; j < 5; j++)
				{
					frag[j] = text[j + i];
				}
				msg.numberfrag = num;
				num++;
				StringCchPrintf(msg.text, 6, frag);
				n = sendto(sock, (const char*)&msg, sizeof(msg), NULL, (struct sockaddr*)&sockSin, sizeof(sockSin));
				ZeroMemory(msg.text, 10);
			}

		}

		break;
		case IDC_BUTTON1:
		{
			int a = Button_GetCheck(hRadio);
			if (a)
			{
				CHAR Buf[128] = "";
				//BOOL optval = TRUE;
				//int optlen = sizeof(optval);
				//int err = setsockopt(sock, SOL_SOCKET, NULL, (char*)&optval, optlen);
				sockSin.sin_family = AF_INET;
				sockSin.sin_port = htons(7581);
				GetDlgItemTextA(hwnd, IDC_EDIT4, Buf, 128);
				sockSin.sin_addr.s_addr = inet_addr(Buf);
			}
			else
			{
				BOOL optval = TRUE;
				int optlen = sizeof(optval);
				int err = setsockopt(sock, SOL_SOCKET, SO_BROADCAST, (char*)&optval, optlen);
				sockSin.sin_family = AF_INET;
				sockSin.sin_port = htons(7581);
				sockSin.sin_addr.s_addr = htonl(INADDR_BROADCAST);
			}

		}
		break;
		}

	}

}


unsigned __stdcall ThreadFunc(void* lpParameter)
{
	SLP_msg msgt = { 0 };

	int len = sizeof(sockSout);
	WCHAR text[255] = L"";
	WCHAR text1[255] = L"";
	for (;;)
	{
		int n = recvfrom(sock, (char*)&msgt, sizeof(msgt), NULL, (struct sockaddr*)&sockSout, &len);

		if (SOCKET_ERROR != n) // сообщение успешно получено
		{
			int a = msgt.filelen / 5;
			if (msgt.filelen % 5 == 0)
				msgt.numberfrag--;
			for (int i = 0; i < 255; i++)
			{
				if (i == msgt.numberfrag * 5)
				{
					for (int j = 0; j < 5; j++)
						text[i + j] = msgt.text[j];
				}
			}
			if (msgt.filelen % 5 == 0)
				msgt.numberfrag++;
			if (a == msgt.numberfrag)
			{
				wcscat(text1, msgt.username);
				wcscat(text1, L": ");
				wcscat(text1, text);
				ListBox_AddString(hList, text1);
				memset(text, NULL, 255);
				memset(text1, NULL, 255);
			}
		}
	}

	return(0);
}
