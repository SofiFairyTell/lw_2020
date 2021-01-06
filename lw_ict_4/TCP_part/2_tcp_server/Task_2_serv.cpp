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

#pragma comment(lib, "Ws2_32.lib")

#define BUTTON_DISCONNECT		2001
#define BUTTON_RECEIVE			2002

#define MAX_MESSAGE_SIZE	255

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


volatile bool stoped = false;

SOCKET data_socket;//сокет с данными от клиента
SOCKET listen_socket;//сокет для прослушивания потока


sockaddr_in sOut;

AdressHeader msgA;//для пакета с имененем адресата и количеством файлов


void recv_file(char* Data, int Size);

unsigned __stdcall ListenThread(LPVOID lpParameter);//для ожидания соединения


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

	if (0 == RegisterClassEx(&wcex)) 
	{
		return -1;
	}

	LoadLibrary(TEXT("ComCtl32.dll"));

	HWND hWnd = CreateWindowEx(NULL, TEXT("WindowClass"), TEXT("Server"),
		WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 550, 200, NULL, NULL, hInstance, NULL);

	if (NULL == hWnd) 
	{
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

				case BUTTON_DISCONNECT:
				{
					int err = shutdown(listen_socket, SD_BOTH);
				}
				break;
				case BUTTON_RECEIVE:
				{
					recv_file((char*)&msgA, sizeof(msgA));

					TCHAR Message[MAX_MESSAGE_SIZE] = _T(""); //сообщение

					StringCchCat(Message, _countof(Message), _T("Отправитель:"));
					StringCchCat(Message, _countof(msgA.adr), msgA.adr);
			
					MessageBox(NULL, Message, TEXT("Server"), MB_OK | MB_ICONINFORMATION);
		
					for (int i = 0; i < msgA.CountOfFiles; i++)
					{
						MainHeader msgH = {0};

						recv_file((char*)&msgH, sizeof(msgH)); //получим пакет с данными

						std::ofstream file_receive(msgH.filename, std::ios::out | std::ios::binary); // создание выходного потока
						
						//чтобы русский язык нормально определялся в буфере wchar_t
						std::locale loc(std::locale(), new std::codecvt_utf8<__int32>);
						file_receive.imbue(loc);

						char* Data = new char[msgH.filesize + 1];//инициализация буфера 
						recv_file((char*)Data, msgH.filesize);//чтение данных в буфер
						
						file_receive.write((char*)Data, msgH.filesize + 1);
						file_receive.close();

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
		WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 150, 20, 240, 30, hWnd, (HMENU)BUTTON_DISCONNECT, lpCreateStruct->hInstance, NULL);
	
	CreateWindowEx(0, WC_BUTTON, TEXT("Получить файлы"),
		WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 150, 60, 240, 30, hWnd, (HMENU)BUTTON_RECEIVE, lpCreateStruct->hInstance, NULL);

	WSADATA wsaData;
	u_long argp = 1;
	int result = ioctlsocket(data_socket, FIONBIO, &argp);// перевода сокета в не блокируемое состояние (nonblocking mode) используется команда FIONBIO
	
	result = WSAStartup(MAKEWORD(2, 2), &wsaData);
	
	if (result == 0)
	{
		listen_socket = socket(AF_INET, SOCK_STREAM, 0);
		
		if (listen_socket != INVALID_SOCKET)
		{
			sOut = { AF_INET, htons(7581), INADDR_ANY };
			bind(listen_socket, (sockaddr*)&sOut, sizeof(sOut));
			_beginthreadex(NULL, 0, ListenThread, NULL, 0, NULL);
		}
	}
	return TRUE;
}

void OnDestroy(HWND hWnd) 
{
	stoped = true;
	closesocket(listen_socket);
	WSACleanup();
	PostQuitMessage(0);
}


void recv_file(char* Data, int Size)
{
	if (Data == nullptr || Size == 0)
	{
		return;
	}
	int _return, bytes_receive = 0;
	do
	{
		_return = recv(data_socket, Data + bytes_receive, Size, 0);
		if (_return == SOCKET_ERROR)
		{
			int err = WSAGetLastError();
			MessageBox(NULL, TEXT("Возникла ошибка", err), TEXT("Client"), MB_OK | MB_ICONERROR);
		}
		else
		{
			bytes_receive += _return;
			Size -= _return;
		}
	} while (Size > 0 && stoped == false);
}

unsigned __stdcall ListenThread(LPVOID lpParameter)
{
	int result = listen(listen_socket, 5);
	if ( result != SOCKET_ERROR)
	{
		for (;;)
		{
			data_socket = accept(listen_socket, NULL, NULL);
			MessageBox(NULL, TEXT("Соединение установлено"), TEXT("Server"), MB_OK | MB_ICONINFORMATION);
			if (INVALID_SOCKET == data_socket)
			{
				if (WSAEINTR == WSAGetLastError()) break;
			}
		}
	}
	return 0;
}


