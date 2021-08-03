#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#include <winsock2.h>
#include <Windows.h>
#include <WindowsX.h>
#include <tchar.h>
#include <locale.h>
#include <CommCtrl.h>
#include <Strsafe.h>
#include <string.h>
#include <process.h>
#include <math.h> 
#include <iostream>

#pragma comment (lib, "ws2_32.lib")
using namespace std;
#define SERVER "127.0.0.1"


#define IDC_EDIT_MESSAGE		2002
#define IDC_LISTBOX				2003
#define IDC_BUTTON_SEND			2004
#define IDC_BUTTON_UPDATE		2005
#define IDC_LISTB2				2006
#define IDC_BUTTON_ADD			2009
#define IDC_BUTTON_CLEAR		2010


#define SHUTDOWN(sock) if(sock != NULL) {shutdown(sock, SD_BOTH); closesocket(sock); sock = NULL;}
struct hostent *hst;
sockaddr_in SIN = { 0 };

#pragma pack(push,1)
struct Package
{
	CHAR recipients[4096];
	CHAR message[1024];
};
#pragma pack(pop)


LRESULT CALLBACK windowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM	lparam);
BOOL OnCreate(HWND hwnd, LPCREATESTRUCT lpCreateStruct);
void OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify);
void OnDestroy(HWND hwnd);
void GetClientNames(char string[], HWND hwndLB);
int WriteMessage(SOCKET s, char* buf, size_t length);
int ReadMessage(SOCKET s, char* buf, size_t length);
HWND hDlg = NULL;

BOOL PreTranslateMessage(LPMSG lpMsg);

WSADATA wsaData;
SOCKET  s;
HANDLE hThread;
HWND hWnd, hList2;
char host[50];

unsigned int CALLBACK ThreadFunc(LPVOID lpParameter);

int WINAPI _tWinMain(HINSTANCE hInstance, HINSTANCE, LPTSTR lpszCmdLine, int nCmdShow)
{
	setlocale(LC_ALL, "Russian");
	WNDCLASSEX wcex = { sizeof(WNDCLASSEX) };
	wcex.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
	wcex.lpfnWndProc = windowProc; // оконна€ процедура
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_BTNFACE + 1);
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = TEXT("winClass"); // им€ класса
	wcex.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

	if (0 == RegisterClassEx(&wcex))
		return -1;

	LoadLibrary(TEXT("ComCtl32.dll"));
	hWnd = CreateWindowEx(0, TEXT("winClass"),
		TEXT("TCP клиент"),
		WS_OVERLAPPEDWINDOW & (~WS_MAXIMIZEBOX) ^ WS_THICKFRAME,
		CW_USEDEFAULT, 0,
		CW_USEDEFAULT, 0,
		NULL, NULL,
		hInstance, NULL);

	if (NULL == hWnd)
		return -1;

	ShowWindow(hWnd, nCmdShow);

	MSG msg;
	BOOL bRet;

	for (;;)
	{
		// определ€ем наличие сообщений в очереди
		while (!PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE))
		{

		} // while

		// извлекаем сообщение из очереди
		bRet = GetMessage(&msg, NULL, 0, 0);

		if (bRet == -1)
		{
			/* обработка ошибки и возможно выход из цикла */
		} // if
		else if (FALSE == bRet)
		{
			break; // получено WM_QUIT, выход из цикла
		} // if
		else if (PreTranslateMessage(&msg) == FALSE)
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		} // if

	} // for
	return (int)msg.wParam; // завершаем работу приложени€
} // _tWinMain


LRESULT CALLBACK windowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
		HANDLE_MSG(hWnd, WM_CREATE, OnCreate);
		HANDLE_MSG(hWnd, WM_DESTROY, OnDestroy);
		HANDLE_MSG(hWnd, WM_COMMAND, OnCommand);
	}
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

BOOL PreTranslateMessage(LPMSG lpMsg)
{
	return IsDialogMessage(hDlg, lpMsg);
} // PreTranslateMessage

BOOL OnCreate(HWND hwnd, LPCREATESTRUCT lpCreateStruct)
{
	if (NULL != hwnd)
	{

		CreateWindowExA(0, "Edit", "",
			WS_CHILD | WS_VISIBLE | WS_BORDER, 250, 120, 620, 25, hwnd, (HMENU)IDC_EDIT_MESSAGE, lpCreateStruct->hInstance, NULL);
		CreateWindowEx(0, TEXT("Button"), TEXT("отправить"),
			WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 600, 160, 150, 75, hwnd, (HMENU)IDC_BUTTON_SEND, lpCreateStruct->hInstance, NULL);
		CreateWindowEx(0, TEXT("Button"), TEXT("обновить"),
			WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 20, 200, 150, 75, hwnd, (HMENU)IDC_BUTTON_UPDATE, lpCreateStruct->hInstance, NULL);
		CreateWindowEx(0, TEXT("Button"), TEXT("очистить контакты"),
			WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 230, 200, 150, 75, hwnd, (HMENU)IDC_BUTTON_CLEAR, lpCreateStruct->hInstance, NULL);
		CreateWindowEx(0, TEXT("ListBox"), NULL,
			WS_CHILD | WS_VISIBLE | WS_BORDER | WS_HSCROLL | LBS_STANDARD, 20, 320, 850, 210, hwnd, (HMENU)IDC_LISTBOX, lpCreateStruct->hInstance, NULL);
		hList2 = CreateWindowExA(0, "ListBox", NULL,
			WS_CHILD | WS_VISIBLE | WS_BORDER | WS_HSCROLL | WS_VSCROLL | LBS_MULTIPLESEL, 20, 40, 220, 150, hwnd, (HMENU)IDC_LISTB2, lpCreateStruct->hInstance, NULL);

		int err = WSAStartup(MAKEWORD(2, 2), &wsaData);

		if (0 == err)
		{
			/* »нициализаци€ WinSock выполнена успешно */
		} // if
		s = socket(AF_INET, SOCK_STREAM, 0);
		if (s == INVALID_SOCKET)
		{
			printf("Socket error: %d\n", WSAGetLastError());
			WSACleanup();
		}

		SIN.sin_family = AF_INET; // семейство адресов = IPv4
		SIN.sin_port = htons(5150);
		SIN.sin_addr.s_addr = inet_addr(SERVER);

		if (SOCKET_ERROR != err)
		{
			err = connect(s, (sockaddr *)&SIN, sizeof(SIN));

			/* —окет успешно ассоциирован */
			if (SOCKET_ERROR == err)
			{
				MessageBox(hwnd, TEXT("ќшибка!Ќе удалось установить соедине-ние"), TEXT("”становка соеденени€ с сервером"), MB_OK | MB_ICONERROR);
			}
			hThread = (HANDLE)_beginthreadex(NULL, NULL, ThreadFunc, NULL, NULL, NULL);
		} // if
	} // if
	DWORD cchSize = 50;
	GetComputerNameEx(ComputerNamePhysicalDnsHostname, host, &cchSize);
	return TRUE;
}

void OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify)
{
	Package msg;
	if (BN_CLICKED == codeNotify)
	{
		HINSTANCE hInstance = GetWindowInstance(hwnd);
		switch (id)
		{
		case IDC_BUTTON_SEND:
		{
			Package msg;
			HWND hwndEdit = GetDlgItem(hwnd, IDC_EDIT_MESSAGE);
			HWND hwndLB = GetDlgItem(hWnd, IDC_LISTB2);
			int sizeText = Edit_GetTextLength(hwndEdit);
			GetDlgItemTextA(hwnd, IDC_EDIT_MESSAGE, msg.message, sizeText + 1);
			char Buff[1024];
			StringCchPrintf(Buff, 1024, "%s", msg.message);
			StringCchPrintf(msg.message, 1024, "[%s]: %s", host, Buff);
			int cItems = ListBox_GetCount(hwndLB);
			int* indexes = new int[cItems];
			int numOfSelected = ListBox_GetSelItems(hwndLB, cItems, indexes);
			int size = 0;
			strcpy(msg.recipients, "");
			for (int i = 0; i < cItems; ++i)
			{
				ListBox_GetText(hwndLB, indexes[i], Buff);
				char c = strlen(Buff);
				StringCchPrintf(msg.recipients, 4096, "%s%c%s", msg.recipients, c, Buff);
			}
			WriteMessage(s, (char *)&msg, sizeof(msg));
			SetDlgItemTextA(hwnd, IDC_EDIT_MESSAGE, "");
		}
		break;
		case IDC_BUTTON_UPDATE:
		{
			Package msg;
			StringCchCopyA(msg.recipients, 2, "1");
			WriteMessage(s, (char *)&msg, sizeof(msg));
		}
		break;
		case IDC_BUTTON_CLEAR:
		{
			HWND hwndLB = GetDlgItem(hwnd, IDC_LISTB2);
			ListBox_ResetContent(hwndLB);
		}
		break;
		}
	}//if
}

void OnDestroy(HWND hwnd)
{
	Package msg;
	StringCchCopyA(msg.recipients, 2, "0");
	WriteMessage(s, (char *)&msg, sizeof(msg));
	SHUTDOWN(s);
	CloseHandle(hThread);
	PostQuitMessage(0); // отправл€ем сообщение WM_QUIT
}


unsigned int CALLBACK ThreadFunc(LPVOID lpParameter)
{
	Package msg;
	for (;;)
	{


		// получаем текстовое сообщение
		int   n = ReadMessage(s, (char *)&msg, sizeof(msg));

		if (SOCKET_ERROR != n) // сообщение успешно получено
		{

			if (strcmp(msg.message, "") == 0)
			{

				HWND hwndCB = GetDlgItem(hWnd, IDC_LISTB2);
				GetClientNames(msg.recipients, hwndCB);


			}
			else
			{
				HWND hwndLB = GetDlgItem(hWnd, IDC_LISTBOX);

				SendMessageA(hwndLB, LB_ADDSTRING, NULL, (LPARAM)msg.message);
			}


		} // if	
		else
			if (WSAEINTR == WSAGetLastError()) break;


	}
	return 0;
}
void GetClientNames(char string[], HWND hwndCB)
{
	ListBox_ResetContent(hwndCB);
	int j = 0;
	unsigned int i = 0;
	do
	{
		int n = string[i];
		char *clientName = new char[n + 1];
		for (int k = 0; k < n; k++)
		{
			clientName[k] = string[i + 1 + k];
		}
		clientName[n] = '\0';
		ListBox_AddString(hList2, clientName);
		delete[]clientName;
		i += n + 1;
	} while (i < 4096 && string[i] != '\0');
	ListBox_SetCurSel(hList2, 0);
}
int ReadMessage(SOCKET s, char* buf, size_t length)
{
	int all = length;
	int received = 0;
	while (all > 0)
	{
		received = recv(s, buf, all, 0);
		if (received < 0) return SOCKET_ERROR;
		buf += received;
		all -= received;
	}
	return length;
}

int WriteMessage(SOCKET s, char* buf, size_t length)
{
	int all = length;
	int sended = 0;
	while (all > 0)
	{
		sended = send(s, buf, all, 0);
		if (sended < 0) return SOCKET_ERROR;
		buf += sended;
		all -= sended;
	}
	return length;
}

