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

#pragma pack(1)
struct msg_t
{
	TCHAR filename[MAX_PATH];//имя файла
	unsigned int fileSize; //размер файла
	TCHAR adr[15]; //имя отправителя
};
#pragma pack()

BYTE* byteBuffer;

HWND hListBox = NULL;
volatile bool stoped = false;

SOCKET s;
SOCKET ss;

sockaddr_in sOut;

msg_t msgOut;

char bufferNameIP[15];

HANDLE hFile = NULL;

void recvfile(char* buf, int len);
unsigned __stdcall ListenThread(LPVOID lpParameter);

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
		WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 550, 100, NULL, NULL, hInstance, NULL);

	if (NULL == hWnd) {
		return -1;
	}

	ShowWindow(hWnd, nCmdShow);

	MSG msg;
	BOOL bRet;

	for (;;) {
		bRet = GetMessage(&msg, NULL, 0, 0);

		if (bRet == -1) {
		}
		else if (FALSE == bRet) {
			break;
		}
		else {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int)msg.wParam;
}

LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	switch (uMsg) {

		HANDLE_MSG(hWnd, WM_CREATE, OnCreate);
		HANDLE_MSG(hWnd, WM_DESTROY, OnDestroy);

	case WM_COMMAND:
	{
		switch (LOWORD(wParam)) {

		case IDC_BUTTON2:
		{
			int err = shutdown(ss, SD_BOTH);
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
	WSADATA wsaData;
	int err = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (0 == err) {
		ss = socket(AF_INET, SOCK_STREAM, 0);
		if (ss != INVALID_SOCKET) {
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

//void recvfile1(msg_t msg) {
//	int len = sizeof(msg);
//	int n, l = 0;
//	while (len > 0 && stoped == false) {
//		n = recv(s, (char*)&msg, len, 0);
//		if (n > 0) {
//			l += n;
//			len -= n;
//		}
//	}
//}
void recvfile2(char* buf, int len) {
	int n;
	while (len > 0 && stoped == false) {
		n = recv(s, buf, len, 0);
		if (n > 0) {
			buf += n;
			len -= n;
		}
	}
}

unsigned __stdcall ListenThread(LPVOID lpParameter) {
	int err = listen(ss, 5);
	if (SOCKET_ERROR != err)
	{
		for (;;)
		{
			s = accept(ss, NULL, NULL);
			MessageBox(NULL, TEXT("Соединение установлено"), TEXT("Server"), MB_OK | MB_ICONINFORMATION);
			if (INVALID_SOCKET == s)
			{
				if (WSAEINTR == WSAGetLastError()) break;
			}
			else
			{
				CHAR name[MAX_PATH] = "";
				recvfile2((char*)&msgOut, sizeof(msgOut));
				byteBuffer = new BYTE[msgOut.fileSize];
				recvfile2((char*)byteBuffer, msgOut.fileSize);
				hFile = CreateFile((LPCWSTR)msgOut.filename, GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_FLAG_OVERLAPPED, NULL);
				OVERLAPPED oWrite = { 0 };
				oWrite.Offset = 0;
				oWrite.hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
				TCHAR adr1[30];
				StringCchPrintf(adr1, 39, TEXT("Файл получен от %s"), msgOut.adr);
				if (INVALID_HANDLE_VALUE != hFile) {
					MessageBox(NULL, adr1, TEXT("Server"), MB_OK | MB_ICONINFORMATION);
					WriteFile(hFile, byteBuffer, msgOut.fileSize, NULL, &oWrite);
					WaitForSingleObject(oWrite.hEvent, INFINITE);
					CloseHandle(oWrite.hEvent);
				}
				CloseHandle(hFile);

			}

		}
	}
	return 0;

}
