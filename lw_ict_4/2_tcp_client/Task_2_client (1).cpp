#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#include <Winsock2.h>
#include <Windows.h>
#include <windowsx.h>
#include <CommCtrl.h>
#include <tchar.h>
#include <strsafe.h>
#include <process.h>
#include <string>
#include <fstream>
#include <iostream>
#include <codecvt>//для фассетов 
#include <experimental/filesystem>

namespace fs = std::experimental::filesystem;

#pragma comment(lib, "Ws2_32.lib")

using namespace std;

#define IDC_STATIC				2001
#define IDC_EDIT_OPEN_FILE_NAME 2003
#define IDC_BUTTON_CONNECTION	2004
#define IDC_BUTTON_CLOSE		2005
#define IDC_BUTTONOpen			2006
#define IDC_BUTTONSend			2007
#define IDC_BUTTONGet			2008

#define IDC_IPADDR				2011
#define IDC_USERNAME			2012
#define IDC_LIST				2013

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

BYTE* byteBuffer;

SOCKET sender_socket;//сокет для передачи данных 
//SOCKET ss;
//SOCKET sss;


volatile bool stoped = false;

AdressHeader msgA;

//sockaddr_in soin;

//wchar_t* NamesOfFile;

wchar_t FileNameTitles[260] = L"";//хранит указатель на папку, если выбрано более одного файла
wchar_t FileNameTitle[260] = L"";//использовать если выбран один файл
//wchar_t bufferNameIP[25] = L"";
//const char bufferNameIP[25];

BOOL FileSending(wchar_t* NameOfFiles);//функция для отправки файлов, передаем их имена

DWORD result;

LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

void OnIdle(HWND hWnd);
BOOL OnCreate(HWND hWnd, LPCREATESTRUCT lpCreateStruct);
void OnDestroy(HWND hWnd);
void OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify);
void sendfile(SOCKET send_socket, const char* buf, int len);

int WINAPI _tWinMain(HINSTANCE hInstance, HINSTANCE, LPTSTR lpszCmdLine, int nCmdShow) {

	WNDCLASSEX wcex = { sizeof(WNDCLASSEX) };

	wcex.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
	wcex.lpfnWndProc = WindowProc;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_BTNFACE + 2);
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = TEXT("WindowClass");
	wcex.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

	if (0 == RegisterClassEx(&wcex)) 
	{
		return -1;
	}

	LoadLibrary(TEXT("ComCtl32.dll"));

	HWND hWnd = CreateWindowEx(NULL, TEXT("WindowClass"), TEXT("Client"),
		WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 700, 400, NULL, NULL, hInstance, NULL);

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
		else 
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int)msg.wParam;
}

BOOL FileSending(wchar_t* NamesOfFile, int CountOfFiles)
{
	std::wstring filename = L"";

	if (CountOfFiles > 1)
	{
		/*выполнение первоначального сдвига, если в начале у нас был указатель на директорию*/
		filename = NamesOfFile;
		NamesOfFile += (filename.length() + 1);
	}

	
	while (NamesOfFile != L"")
	{
		std::ifstream file_open(NamesOfFile, ios::in|ios::binary); // создание входного потока

		/*не влияет???*/
		//чтобы русский язык нормально определялся в буфере wchar_t
		std::locale loc(std::locale(), new std::codecvt_utf8<__int32>);
		file_open.imbue(loc);

		//узнаем размер файла
		file_open.seekg(0,wifstream::end);//перейдем в конец файла
		int size = file_open.tellg();//определим размер файла
		file_open.seekg(0);//вернемся в начало файла

		char* buffer = new char[size+1];//инициализация буфера 

		file_open.read(buffer, size);

		if (file_open)
		{
			MainHeader msgH;

			StringCchCopy(msgH.filename, MAX_PATH, NamesOfFile);
			msgH.filesize = size;
			sendfile(sender_socket, (const char*)&msgH, sizeof(msgH));
			sendfile(sender_socket, (const char*)buffer, size);
			
			/*освобождение ресурсов т.д. */
			delete[] buffer;
			file_open.close();
			filename = NamesOfFile;
			
			NamesOfFile += (filename.length() + 1);//сдвиг к следующему файлу
		}
		
		else
		{
			delete[] buffer;
			file_open.close();
			return 0;
			//обработка ошибки чтения файла должна быть где-то здесь....
		}
	}
	return 0;
}

LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	switch (uMsg) 
	{
		HANDLE_MSG(hWnd, WM_CREATE, OnCreate);
		HANDLE_MSG(hWnd, WM_DESTROY, OnDestroy);
		HANDLE_MSG(hWnd, WM_COMMAND, OnCommand);
		break;
	}
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}
BOOL OnCreate(HWND hWnd, LPCREATESTRUCT lpCreateStruct) 
{
	/*Кнопки*/
	CreateWindowEx(0, WC_BUTTON, TEXT("Соединиться c сервером"),
		WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 275, 100, 240, 27, hWnd, (HMENU)IDC_BUTTON_CONNECTION, lpCreateStruct->hInstance, NULL);
	
	CreateWindowEx(0, WC_BUTTON, TEXT("Завершить соединение"),
		WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 275, 130, 240, 27, hWnd, (HMENU)IDC_BUTTON_CLOSE, lpCreateStruct->hInstance, NULL);
	
	/*Поле для ввода IP-адреса*/
	CreateWindowEx(0, TEXT("SysIPAddress32"), 
		NULL, WS_CHILD | WS_VISIBLE, 20, 100, 240, 27, hWnd, (HMENU)IDC_IPADDR, lpCreateStruct->hInstance, NULL);
	CreateWindowEx(0, WC_STATIC, TEXT("Адрес сервера:"), WS_CHILD | WS_VISIBLE, 20, 70, 240, 27, hWnd, NULL, lpCreateStruct->hInstance, NULL);

	/*Поле для ввода имени пользователя*/
	CreateWindowEx(0, TEXT("Edit"), NULL, WS_CHILD | WS_VISIBLE | WS_BORDER | ES_MULTILINE, 20, 40, 240, 27, hWnd, (HMENU)IDC_USERNAME, lpCreateStruct->hInstance, NULL);
	
	
	CreateWindowEx(0, WC_STATIC, TEXT("Имя:"), WS_CHILD | WS_VISIBLE, 20, 10, 240, 27, hWnd, NULL, lpCreateStruct->hInstance, NULL);

	/*Работа с файлами*/
	CreateWindowEx(0, WC_BUTTON, TEXT("Выбрать файлы/файлы"),
		WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 360, 230, 240, 27, hWnd, (HMENU)IDC_BUTTONOpen, lpCreateStruct->hInstance, NULL);

	CreateWindowEx(0, WC_BUTTON, TEXT("Передать файл"),
		WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 360, 260, 240, 27, hWnd, (HMENU)IDC_BUTTONSend, lpCreateStruct->hInstance, NULL);
	
	CreateWindowEx(0, TEXT("ListBox"), 
		NULL, WS_CHILD | WS_VISIBLE | WS_BORDER | LBS_WANTKEYBOARDINPUT | LBS_NOTIFY|WS_VSCROLL|WS_HSCROLL, 20, 170, 320, 150, hWnd, (HMENU)IDC_LIST, lpCreateStruct->hInstance, NULL);
	WSADATA wsaData;
	int err = WSAStartup(MAKEWORD(2, 2), &wsaData);
	sender_socket = socket(AF_INET, SOCK_STREAM, 0);

	return TRUE;
}

void OnDestroy(HWND hWnd)
{
	closesocket(sender_socket);
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
			const char bufferNameIP[25] = {0};
			GetDlgItemTextA(hwnd, IDC_IPADDR, (LPSTR)bufferNameIP, _countof(bufferNameIP));
			sockaddr_in sin = { 0 };
			sin.sin_family = AF_INET;
			sin.sin_port = htons(7581);
			//sin.sin_addr.s_addr = inet_addr("192.168.56.104");//адрес виртуальной машины
			//sin.sin_addr.s_addr = inet_addr("192.168.56.1");//адрес ПК
			sin.sin_addr.s_addr = inet_addr(bufferNameIP);
			int err = connect(sender_socket, (sockaddr*)&sin, sizeof(sin));
		}
		break;
		case IDC_BUTTON_CLOSE:
		{
			int err = shutdown(sender_socket, SD_BOTH);
		}
		break;
		case IDC_BUTTONOpen:
		{
			OPENFILENAME ofn = {sizeof(OPENFILENAME)};

			ofn.hInstance = GetModuleHandle(NULL);
			ofn.lpstrFile = FileNameTitles;//полный путь
			ofn.nMaxFile = MAX_PATH;
			ofn.lpstrFilter = TEXT("Files\0*.*");//фильтр
			ofn.nFilterIndex = 1;
			ofn.lpstrFileTitle = FileNameTitle;//название файла
			ofn.nMaxFileTitle = MAX_PATH;
			ofn.lpstrInitialDir = NULL;
			ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_EXPLORER | OFN_ALLOWMULTISELECT;

			if (GetOpenFileName(&ofn) == TRUE) 
			{
				MessageBox(NULL, TEXT("Файлы успешно выбраны"), TEXT("Client"), MB_OK | MB_ICONINFORMATION);
				int nFileOffset = ofn.nFileOffset;
				
				HWND hWNDctrl = GetDlgItem(hwnd, IDC_LIST);
				ListBox_ResetContent(hWNDctrl); //очистка списка просмотра
				/*Начало подсчета количества файлов*/
				if (nFileOffset > lstrlen(ofn.lpstrFile))
				{
					/*Было выделено более одного файла*/
					while (ofn.lpstrFile[nFileOffset])
					{

						nFileOffset = nFileOffset + wcslen(ofn.lpstrFile + nFileOffset) + 1;
						msgA.CountOfFiles++;
						int iItem = ListBox_AddString(hWNDctrl, ofn.lpstrFile);
						ListBox_SetCurSel(hWNDctrl, iItem);
					}
				}
				else
				{
					/*Был выделен один файл*/
					msgA.CountOfFiles++;
					int iItem = ListBox_AddString(hWNDctrl, ofn.lpstrFile);
					ListBox_SetCurSel(hWNDctrl, iItem);
				}
			}
		}
		break;

		case IDC_BUTTONSend:
		{
			GetDlgItemTextW(hwnd, IDC_USERNAME, (LPWSTR)msgA.adr, sizeof(msgA.adr));//Запись имени отправителя в структуру
			
			sendfile(sender_socket, (const char*)&msgA, sizeof(msgA));//отправим имя серверу
			
			if (msgA.CountOfFiles > 1)
			{
				FileSending(FileNameTitles, msgA.CountOfFiles);//отправка нескольких экземпляров файла
			}
			else
			{
				FileSending(FileNameTitle, msgA.CountOfFiles);//единственный экземпляр файла
			}
			msgA.CountOfFiles = 0; //обнуление счетчика файлов

		}
		break;

		}
	}
}
void sendfile(SOCKET send_socket, const char* buf, int len)
{
	int n, l = 0;
	while (len > 0) {
		n = send(send_socket, buf + l, len, 0);
		if (n > 0) 
		{
			l += n;
			len -= n;
		}
	}
}
void OnIdle(HWND hWnd) 
{
	
}
