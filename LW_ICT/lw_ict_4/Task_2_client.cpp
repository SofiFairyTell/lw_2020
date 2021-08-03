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

#define IDC_STATIC				2001
#define IDC_EDIT_OPEN_FILE_NAME 2003
#define IDC_BUTTON_CONNECTION	2004
#define IDC_BUTTON_CLOSE		2005
#define IDC_BUTTONOpen			2006
#define IDC_BUTTONSend			2007
#define IDC_BUTTONGet			2008
#define IDC_EDIT1				2011
#define IDC_EDIT2				2012

#pragma pack(1)
struct msg_t {
	TCHAR filename[MAX_PATH];//имя файла
	unsigned int fileSize; //размер файла
	TCHAR adr[128]; //адрес отправителя
};
#pragma pack()

SOCKET s;
SOCKET ss;
SOCKET sss;

OVERLAPPED ReadOL;
volatile bool stoped = false;
msg_t msgOut;
sockaddr_in soin;

HANDLE hFile = NULL;
TCHAR fullFileName[MAX_PATH];
TCHAR fileName[MAX_PATH] = L"";
TCHAR bufferNameIP[25];

DWORD sizeBuffer;
BYTE* byteBuffer;
DWORD result;

LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

void OnIdle(HWND hWnd);
BOOL OnCreate(HWND hWnd, LPCREATESTRUCT lpCreateStruct);
void OnDestroy(HWND hWnd);
void OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify);

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

	HWND hWnd = CreateWindowEx(NULL, TEXT("WindowClass"), TEXT("Client"),
		WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 550, 240, NULL, NULL, hInstance, NULL);

	if (NULL == hWnd) {
		return -1;
	}

	ShowWindow(hWnd, nCmdShow);

	MSG msg;
	BOOL bRet;

	for (;;) {
		while (!PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE)) {
			OnIdle(hWnd);
		}

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
		HANDLE_MSG(hWnd, WM_COMMAND, OnCommand);
		break;
	}
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}
BOOL OnCreate(HWND hWnd, LPCREATESTRUCT lpCreateStruct) {

	CreateWindowEx(0, WC_BUTTON, TEXT("Соединиться c сервером"),
		WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 275, 20, 240, 27, hWnd, (HMENU)IDC_BUTTON_CONNECTION, lpCreateStruct->hInstance, NULL);

	CreateWindowEx(0, TEXT("Edit"), NULL, WS_CHILD | WS_VISIBLE | WS_BORDER | ES_MULTILINE, 20, 20, 240, 27, hWnd, (HMENU)IDC_EDIT1, lpCreateStruct->hInstance, NULL);

	CreateWindowEx(0, TEXT("Edit"), NULL, WS_CHILD | WS_VISIBLE | WS_BORDER | ES_MULTILINE, 20, 60, 240, 27, hWnd, (HMENU)IDC_EDIT2, lpCreateStruct->hInstance, NULL);
	CreateWindowEx(0, WC_STATIC, TEXT("- Никнейм отправителя"), WS_CHILD | WS_VISIBLE, 275, 60, 240, 27, hWnd, NULL, lpCreateStruct->hInstance, NULL);
	CreateWindowEx(0, WC_BUTTON, TEXT("Завершить соединение"),
		WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 20, 150, 240, 27, hWnd, (HMENU)IDC_BUTTON_CLOSE, lpCreateStruct->hInstance, NULL);
	CreateWindowEx(0, WC_BUTTON, TEXT("Файл для отправки"),
		WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 20, 110, 240, 27, hWnd, (HMENU)IDC_BUTTONOpen, lpCreateStruct->hInstance, NULL);
	CreateWindowEx(0, WC_BUTTON, TEXT("Передать файл"),
		WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 275, 110, 240, 27, hWnd, (HMENU)IDC_BUTTONSend, lpCreateStruct->hInstance, NULL);

	WSADATA wsaData;
	int err = WSAStartup(MAKEWORD(2, 2), &wsaData);
	s = socket(AF_INET, SOCK_STREAM, 0);

	return TRUE;
}
void OnDestroy(HWND hWnd)
{
	closesocket(s);
	WSACleanup();
	PostQuitMessage(0);
}

void OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify)
{
	if (BN_CLICKED == codeNotify)
	{
		HINSTANCE hInstance = GetWindowInstance(hwnd);
		switch (id)
		{
		case IDC_BUTTON_CONNECTION:
		{
			GetDlgItemText(hwnd, IDC_EDIT1, (LPWSTR)bufferNameIP, _countof(bufferNameIP));
			sockaddr_in sin = { 0 };
			sin.sin_family = AF_INET;
			sin.sin_port = htons(7581);
			sin.sin_addr.s_addr = inet_addr("192.168.56.101");
			int err = connect(s, (sockaddr*)&sin, sizeof(sin));
		}
		break;
		case IDC_BUTTON_CLOSE:
		{
			int err = shutdown(s, SD_BOTH);
		}
		break;
		case IDC_BUTTONOpen:
		{
			OPENFILENAME ofn;
			ZeroMemory(&ofn, sizeof(ofn));
			ofn.lStructSize = sizeof(ofn);
			ofn.hwndOwner = hwnd;
			ofn.hInstance = GetModuleHandle(NULL);
			ofn.lpstrFile = fullFileName;
			ofn.nMaxFile = MAX_PATH;
			ofn.lpstrFilter = TEXT("");
			ofn.nFilterIndex = 1;
			ofn.lpstrFileTitle = fileName;
			ofn.nMaxFileTitle = MAX_PATH;
			ofn.lpstrInitialDir = NULL;
			ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_EXPLORER;
			if (GetOpenFileName(&ofn) == TRUE) {
				MessageBox(NULL, TEXT("Файл успешно выбран"), TEXT("Client"), MB_OK | MB_ICONINFORMATION);
			}
		}
		break;

		case IDC_BUTTONSend:
		{

			hFile = CreateFile(fullFileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_FLAG_OVERLAPPED, NULL);
			if (INVALID_HANDLE_VALUE != hFile) {
				ReadOL.Offset = 0;
				ReadOL.OffsetHigh = 0;
				ReadOL.hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
				LARGE_INTEGER li;
				GetFileSizeEx(hFile, &li);
				sizeBuffer = li.LowPart;
				byteBuffer = new BYTE[sizeBuffer];							 
				ReadFile(hFile, byteBuffer, sizeBuffer, NULL, &ReadOL);
			}
		}
		break;

		}
	}
}
void sendfile(SOCKET s, const char* buf, int len) {
	int n;
	while (len > 0) {
		n = send(s, buf, len, 0);
		if (n > 0) {
			buf += n;
			len -= n;
		}
	}
}
void OnIdle(HWND hWnd) {
	result = WaitForSingleObject(ReadOL.hEvent, 0);
	TCHAR Buf[128] = L"";
	if (result == WAIT_OBJECT_0) {
		if (ERROR_SUCCESS == ReadOL.Internal) {
			msg_t msg;
			StringCchCopy(msg.filename, MAX_PATH, fileName);
			msg.fileSize = sizeBuffer;
			GetDlgItemTextA(hWnd, IDC_EDIT2, (LPSTR)Buf, 128);
			StringCchCopy(msg.adr, 128, Buf);
			sendfile(s, (const char*)&msg, sizeof(msg));
			sendfile(s, (const char*)byteBuffer, sizeBuffer);
		}
		delete[] byteBuffer;
		CloseHandle(ReadOL.hEvent);
		CloseHandle(hFile);
	}
}
