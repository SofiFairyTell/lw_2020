#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#include <Winsock2.h>
#include <Windows.h>
#include <windowsx.h>
#include <CommCtrl.h>
#include <tchar.h>
#include <strsafe.h>
#include <process.h>

#pragma comment(lib, "Ws2_32.lib")

#define IDC_BUTTON1 2001
#define IDC_BUTTON2 2002
#define IDC_BUTTON3 2003

#pragma pack(1)
struct AdressHeader
{
	int CountOfFiles;
	TCHAR adr[128] = L""; //адрес отправителя
};
#pragma pack()

#pragma pack(1)
struct MainHeader
{
	int filesize; //размер файла
	TCHAR filename[50]; // имя файла
};
#pragma pack()


HWND hListBox = NULL;
volatile bool stoped = false;

SOCKET s;//сокет с данными от клиента
SOCKET ss;//сокет для прослушивания потока

BYTE* byteBuffer;
sockaddr_in sOut;

AdressHeader msgA;


char bufferNameIP[15];

HANDLE hFile = NULL;

void recvfile2(char* buf, int len);

unsigned __stdcall ListenThread(LPVOID lpParameter);
unsigned __stdcall RecvData(LPVOID lpParameter);


LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

BOOL OnCreate(HWND hWnd, LPCREATESTRUCT lpCreateStruct);
void OnDestroy(HWND hWnd);

int WINAPI _tWinMain(HINSTANCE hInstance, HINSTANCE, LPTSTR lpszCmdLine, int nCmdShow) {

	WNDCLASSEX wcex = { sizeof(WNDCLASSEX) };

	wcex.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
	wcex.lpfnWndProc = WindowProc;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW);
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = TEXT("WindowClass");
	wcex.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

	if (0 == RegisterClassEx(&wcex)) {
		return -1;
	}

	LoadLibrary(TEXT("ComCtl32.dll"));

	HWND hWnd = CreateWindowEx(NULL, TEXT("WindowClass"), TEXT("Server"),
		WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 550, 200, NULL, NULL, hInstance, NULL);

	if (NULL == hWnd) {
		return -1;
	}

	ShowWindow(hWnd, nCmdShow);

	MSG msg;
	BOOL bRet;

	for (;;)
	{
		bRet = GetMessage(&msg, NULL, 0, 0);

		if (bRet == -1) 
		{ 
		}
		else if (FALSE == bRet) 
		{
			break;
		}
		else 
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int)msg.wParam;
}

LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) 
{
	switch (uMsg) 
	{

		HANDLE_MSG(hWnd, WM_CREATE, OnCreate);
		HANDLE_MSG(hWnd, WM_DESTROY, OnDestroy);

	case WM_COMMAND:
	{
		switch (LOWORD(wParam)) 
		{

		case IDC_BUTTON2:
		{
			int err = shutdown(ss, SD_BOTH);
		}
		break;
		case IDC_BUTTON3:
		{
			recvfile2((char*)&msgA, sizeof(msgA));
			TCHAR adr[100];
			CHAR name[MAX_PATH] = "";
			StringCchPrintf(adr, 100, TEXT("Файл получен от %s"), msgA.adr);
			MessageBox(NULL, adr, TEXT("Server"), MB_OK | MB_ICONINFORMATION);
			for (int i = 0; i < msgA.CountOfFiles; i++)
			{
				MainHeader msgH = {0};

				recvfile2((char*)&msgH, sizeof(msgH));
				byteBuffer = new BYTE[msgH.filesize];
				recvfile2((char*)byteBuffer, msgH.filesize);

				//LPWSTR DIR = NULL;
				//GetCurrentDirectory(MAX_PATH, DIR);

				hFile = CreateFile((LPCWSTR)msgH.filename, GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_FLAG_OVERLAPPED, NULL);
				OVERLAPPED oWrite = { 0 };
				oWrite.Offset = 0;
				oWrite.hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);

				if (INVALID_HANDLE_VALUE != hFile)
				{
					WriteFile(hFile, byteBuffer, msgH.filesize, NULL, &oWrite);
					WaitForSingleObject(oWrite.hEvent, INFINITE);
					CloseHandle(oWrite.hEvent);
				}
				CloseHandle(hFile);
			}
		}
		break;
		return TRUE;
		}
		return 0;
	}
	}

	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

BOOL OnCreate(HWND hWnd, LPCREATESTRUCT lpCreateStruct) {

	CreateWindowEx(0, WC_BUTTON, TEXT("Завершить соединение"),
		WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 150, 20, 240, 30, hWnd, (HMENU)IDC_BUTTON2, lpCreateStruct->hInstance, NULL);
	CreateWindowEx(0, WC_BUTTON, TEXT("Получить файлы"),
		WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 150, 60, 240, 30, hWnd, (HMENU)IDC_BUTTON3, lpCreateStruct->hInstance, NULL);

	WSADATA wsaData;
	u_long argp = 1;
	int err = ioctlsocket(s, FIONBIO, &argp);// перевода сокета в не блокируемое состояние (nonblocking mode) используется команда FIONBIO
	err = WSAStartup(MAKEWORD(2, 2), &wsaData);
	
	if (0 == err)
	{
		ss = socket(AF_INET, SOCK_STREAM, 0);
		
		if (ss != INVALID_SOCKET) 
		{
			sOut = { AF_INET, htons(7581), INADDR_ANY };
			bind(ss, (sockaddr*)&sOut, sizeof(sOut));
			_beginthreadex(NULL, 0, ListenThread, NULL, 0, NULL);

		}
	}
	return TRUE;
}

void OnDestroy(HWND hWnd) {
	stoped = true;
	closesocket(ss);
	WSACleanup();
	PostQuitMessage(0);
}


void recvfile2(char* buf, int len) 
	{
		int n;
		while (len > 0 && stoped == false) 
		{
			n = recv(s, buf, len, 0);
			if (n > 0) 
			{
				buf += n;
				len -= n;
			}
		}
	}

unsigned __stdcall ListenThread(LPVOID lpParameter)
{
	int err = listen(ss, 5);
	if (SOCKET_ERROR != err)
	{
		for (;;)
		{
			s = accept(ss, NULL, NULL);//установка соединения
			MessageBox(NULL, TEXT("Соединение установлено"), TEXT("Server"), MB_OK | MB_ICONINFORMATION);
			if (INVALID_SOCKET == s)
			{
				if (WSAEINTR == WSAGetLastError()) break;
			}
		}
		
	}
	return 0;
}


