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


#define IDC_EDIT_MESSAGES           2001
#define IDC_EDIT_TEXT               2002
#define IDC_EDIT_USERNAME			2003

#define IDC_RBUTTON_WM_SETTEXT      2004

#define IDC_CLEAR_ALL					2006 //кнопка чтобы удалить весь отправленный текст

#define IDC_RBUTTON_PIPE            2006
#define MAX_TEXT                    1024

//размеры текста с сообщением
#define MAX_MESSAGE_SIZE            255
#define MAX_USERNAME_SIZE			20
/*Дескрипторы*/
HWND hwnd = NULL; // дескриптор главного окна


/*Оконные процедуры*/
LRESULT CALLBACK MainWindowProcess(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

// функция, которая вызывается в цикле обработки сообщений
// перед тем, как сообщение будет передано в оконную процедуру
BOOL PreTranslateMessage(LPMSG lpMsg);

/*Обработчики WM_CREATE, WM_DESTROY, WM_SIZE*/

BOOL OnCreate(HWND hwnd, LPCREATESTRUCT lpCreateStruct);
void OnDestroy(HWND hwnd);

//поток для приема сообщений от экземпляров приложения
HANDLE hThread;

unsigned _stdcall ThreadFunc(LPVOID lParam);

//пакет для передачи
#pragma pack(1)
struct SLP_msg
{
	int filelen;		 //длина сообщени¤
	int numberfrag;		//номер фрагмента
	WCHAR username[20]; //им¤ отправител¤
	WCHAR text[10];		//текст сообщени¤
};
#pragma pack()

//работа с сокетами
SOCKET sockets;
sockaddr_in sockSin = { 0 };
sockaddr_in sockSout = { 0 };
sockaddr_in sockSoin = { 0 };

//отправка сообщений
void SendText(SLP_msg msg, LPCTSTR lpData, DWORD cbData, BOOL DataCopy);
void StartChat(HWND hwnd, LPCTSTR Message);
int WINAPI _tWinMain(HINSTANCE hInstance, HINSTANCE, LPTSTR CmdLine, int CmdShow)
{

	/*Регистрация оконного класса и обработка ошибки*/
	WNDCLASSEX wcex = { sizeof(WNDCLASSEX) };

	wcex.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
	wcex.lpfnWndProc = MainWindowProcess; // оконная процедура
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)CreateSolidBrush(RGB(0, 100, 256));
	wcex.lpszClassName = TEXT("MainWindowProcess"); // имя класса
	wcex.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

	if (0 == RegisterClassEx(&wcex))
	{
		return -1;
	}
	/*---------------------------------------------*/

	LoadLibrary(TEXT("ComCtl32.dll"));
	LoadLibrary(TEXT("Ws2_32.dll"));//for sockets

	/*Создание главного файла и обработка ошибки */
	hwnd = CreateWindowEx(0, TEXT("MainWindowProcess"), TEXT("Chat"),
		WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, 0, 900, 600, NULL, NULL, hInstance, NULL);

	if (hwnd == NULL)
	{
		return -1;
	}
	/*--------------------------------------------------*/
	
	ShowWindow(hwnd, CmdShow); // отображаем главное окно

	/*Цикл обработки сообщений*/
	MSG  msg;
	BOOL RetRes;
	while ((RetRes = GetMessage(&msg, NULL, 0, 0)) != FALSE)
	{
		if (RetRes == -1)
		{
			//Error editing
		}
		else if (!PreTranslateMessage(&msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int)msg.wParam;
}


/*Процедура главного окна*/
LRESULT CALLBACK MainWindowProcess(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
		HANDLE_MSG(hWnd, WM_CREATE, OnCreate);
		HANDLE_MSG(hWnd, WM_DESTROY, OnDestroy);
		HANDLE_MSG(hWnd, WM_COMMAND, OnCommand);
	}
	return DefWindowProc(hWnd, msg, wParam, lParam);
}

/*Обработчик сообщений*/
BOOL PreTranslateMessage(LPMSG Msg)
{
	/*Переменные*/
	WCHAR Message[MAX_MESSAGE_SIZE]; //сообщение
	WCHAR UserName[MAX_USERNAME_SIZE] = _T("");//имя отправителя
	WCHAR UserMessage[532] = _T("");

	//объявим структуру с пакетом
	SLP_msg msg;
	memset(msg.text, NULL, 10);
	memset(msg.username, NULL, 20);
	DWORD symbols, symb_user;  //количество символов в сообщении

	if ((WM_KEYDOWN == Msg->message) && (VK_RETURN == Msg->wParam)) // нажата клавиша Enter
	{
		HWND hwndCtl = GetDlgItem(hwnd, IDC_EDIT_TEXT);
		HWND hwndUser = GetDlgItem(hwnd, IDC_EDIT_USERNAME);

		if (GetFocus() == hwndCtl) // поле ввода обладает фокусом клавиатуры
		{
			/*Чтобы можно было отправить многострочный текст*/
			/*CTRL+ENTER*/
			if (GetKeyState(VK_SHIFT) < 0) // нажата клавиша SHIFT
			{
				Edit_ReplaceSel(hwndCtl, _T("\r\n"));
			}
			else
			{
				symbols = Edit_GetText(hwndCtl, Message, _countof(Message));// копируем текст из поля ввода
				symb_user = Edit_GetText(hwndUser, UserName, _countof(UserName));// копируем текст из поля ввода

				if (symbols > 0)
				{
					// очищаем поле ввода
					Edit_SetText(hwndCtl, NULL);
					//скопируем введенные данные в структуру
					StringCchCat(msg.username,20, UserName);

					MessageBox(hwnd, L"thread", L"tr", MB_OK);

					StartChat(hwnd,Message);//отобрзим отправляемый текст у себя
					
					BOOL optval = TRUE;
					int optlen = sizeof(optval);
					int err = setsockopt(sockets, SOL_SOCKET, SO_BROADCAST, (char*)&optval, optlen);
					sockSin.sin_family = AF_INET;
					sockSin.sin_port = htons(7581);
					sockSin.sin_addr.s_addr = htonl(INADDR_BROADCAST);

					

					SendText(msg, Message, _tcslen(Message), FALSE);

				}
			}
			return TRUE;
		}
	}
	return FALSE;
}

BOOL OnCreate(HWND hwnd, LPCREATESTRUCT lpCreateStruct)
{

	DWORD dwStyle = WS_CHILD | WS_VISIBLE | WS_VSCROLL | WS_HSCROLL | ES_MULTILINE | ES_AUTOHSCROLL | ES_AUTOVSCROLL;

	// создаём поле вывода сообщений
	CreateWindowEx(WS_EX_STATICEDGE, TEXT("Edit"), TEXT(""), dwStyle | ES_READONLY,
		10, 10, 490, 250, hwnd, (HMENU)IDC_EDIT_MESSAGES, lpCreateStruct->hInstance, NULL);

	//Для username
	CreateWindowEx(0, TEXT("Static"), TEXT("User: "), WS_CHILD | WS_VISIBLE | SS_SIMPLE,
		10, 270, 40, 40, hwnd, NULL, lpCreateStruct->hInstance, NULL);

	CreateWindowEx(WS_EX_CLIENTEDGE, TEXT("Edit"), TEXT(""), WS_CHILD | WS_VISIBLE,
		50, 270, 450, 40, hwnd, (HMENU)IDC_EDIT_USERNAME, lpCreateStruct->hInstance, NULL);

	CreateWindowEx(0, TEXT("Button"), TEXT("Clear History"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 
		530, 10, 200, 30, hwnd, (HMENU)IDC_CLEAR_ALL, lpCreateStruct->hInstance, NULL);

	// создаём поле ввода
	HWND hwndCtl = CreateWindowEx(WS_EX_CLIENTEDGE, TEXT("Edit"), TEXT(""), dwStyle,
		10, 320, 490, 140, hwnd, (HMENU)IDC_EDIT_TEXT, lpCreateStruct->hInstance, NULL);

	// задаём ограничение на ввод текста
	Edit_LimitText(hwndCtl, MAX_MESSAGE_SIZE);


	//Инициализация библиотеки WinSock
	WSADATA WsaData;
	int err = WSAStartup(MAKEWORD(2,2),&WsaData);
	
	// Открытие сокета
	sockets = socket(AF_INET, SOCK_DGRAM,NULL);

	//заполним структур, чтобы через можно было получать данные
	sockSout.sin_family = AF_INET; // AF_INET = IPv4 addresses
	sockSout.sin_port = htons(7581); // Little to big endian conversion
	sockSout.sin_addr.s_addr = htonl(INADDR_ANY);//0.0.0.0

	//выполни ассоциирование сокета	
	err = bind(sockets, (sockaddr*)&sockSout, sizeof(sockSout));

	if (err != SOCKET_ERROR)
	{
		hThread = (HANDLE)_beginthreadex(NULL, 0, ThreadFunc, NULL, 0, NULL);
	}
	return TRUE;
}

void OnDestroy(HWND hwnd)
{
	// уведомляем о завершении приложения,
	closesocket(sockets);
	WSACleanup();
	PostQuitMessage(0); // отправляем сообщение WM_QUIT
}

void OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify)
{
	if (BN_CLICKED == codeNotify)
	{
		HINSTANCE hInstance = GetWindowInstance(hwnd);
		switch (id)
		{
		case IDC_CLEAR_ALL:
		{
			HWND hwndCtl = GetDlgItem(hwnd, IDC_EDIT_MESSAGES);		
			Edit_SetText(hwndCtl, NULL);// очищаем поле ввода
		}
		break;
		}

	}
}


// ----------------------------------------------------------------------------------------------
void SendText(SLP_msg msg,LPCTSTR lpData, DWORD cbData, BOOL DataCopy)
{
	
	if ((lpData != NULL) && (cbData > 0))
	{
		//cbData - размер отправляемого сообщения
		msg.filelen = (int)cbData;
		int packnum;
		int fragment = (int)(cbData/5);
		if((int)cbData % 5 == 0)
			packnum = 1;
		else packnum = 0;
		for (int i = 0; i<(int)cbData; i+=5)
		{
			WCHAR frag_pack[5] = L"";
			for (int j = 0; j < 5; j++)
			{
				frag_pack[j] = lpData[j+i]; //заполним пакет данными 
			}
			msg.numberfrag = packnum;//укажем номер пакета
			packnum++;
			StringCchCat(msg.text, 6, frag_pack);
			int result = sendto(sockets,(const char*)&msg, sizeof(msg), NULL,(struct sockaddr*)&sockSin, sizeof(sockSin));
			ZeroMemory(msg.text,10);
		}
	}
}
				
void StartChat(HWND hwnd, LPCTSTR Message)
{
	MessageBox(hwnd, L"LET'S GO CHAT", L"tr", MB_OK);

	HWND hwndCtl = GetDlgItem(hwnd, IDC_EDIT_MESSAGES);
	Edit_SetSel(hwndCtl, Edit_GetTextLength(hwndCtl), -1);// устанавливаем курсор в конец поля вывода
	Edit_ReplaceSel(hwndCtl, Message);// вставляем текст в поле вывода
} 

unsigned __stdcall ThreadFunc(void* lParam)
{
	/*Переменные*/
	WCHAR Message[MAX_MESSAGE_SIZE] = _T(""); //сообщение
	WCHAR UserName[MAX_USERNAME_SIZE] = _T("");//имя отправителя
	WCHAR UserMessage[532] = _T("");

	SLP_msg recived_msg = {0};
	int socket_len  = sizeof(sockSout);

	for (;;)
	{
		//Начнем прием сообщений через сокет
		int result = recvfrom(sockets,(char*)&recived_msg,sizeof(recived_msg),NULL,(struct sockaddr*)&sockSout, &socket_len);
		
		//MessageBox(hwnd, L"START RECEVIENG MSG", L"tr", MB_OK);
		if(result != SOCKET_ERROR)
		{
			int reseived_size = recived_msg.filelen/5;
			if (recived_msg.filelen %5 == 0)
			{
				recived_msg.numberfrag--;
			}
			for (int i = 0; i < 255; i++)
			{
				if (i==recived_msg.numberfrag*5)
				{
					for (int j = 0; j < 5; j++)
					{
						Message[i+j] = recived_msg.text[j];
					}
				}
			}	
				if (recived_msg.filelen%5 == 0)
				{
					recived_msg.numberfrag++;
				}

				if (reseived_size == recived_msg.numberfrag)
				{
					StringCchCat(UserName, 20, recived_msg.username);

					StringCchCat(UserName, _countof(UserName), _T(":"));

					StringCchCat(Message, _countof(Message), _T("\n\n"));

					StringCchCat(UserMessage, _countof(UserMessage), UserName);

					StringCchCat(UserMessage, _countof(UserMessage), Message);

					StartChat(hwnd, UserMessage);

					memset(Message, NULL, 255);
					memset(Message, NULL, 255);
				}
			}
		}
		return(0);
	}
