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

//данные для отправки 
#define IDC_EDIT_MESSAGES           2001
#define IDC_EDIT_TEXT               2002
#define IDC_EDIT_USERNAME			2003
#define IDC_CLEAR_ALL				2004 //кнопка чтобы удалить весь отправленный текст

//переключение адреса
#define IDC_IPADDR					2005 //чтобы ввести целевой IP 
#define IDC_CONNECT					2006
#define IDC_DISCONNECT				2007

#define MAX_TEXT                    1024

//размеры текста с сообщением
#define MAX_MESSAGE_SIZE            255
#define MAX_USERNAME_SIZE			20
#define MAX_USERMESSAGE_SIZE		532
#define FRAFMENT_PACK_SIZE			10
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
void OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify);

//поток для приема сообщений от экземпляров приложения
HANDLE hThread;

unsigned _stdcall ThreadFunc(LPVOID lParam);

//пакет для передачи
#pragma pack(1)
struct SLP_msg
{
	int filelen;		 //длина сообщени¤
	int numberfrag;		//номер фрагмента
	WCHAR username[20]; //имя отправителя
	WCHAR text[10];		//текст сообщения
};
#pragma pack()

//работа с сокетами
SOCKET sockets;

sockaddr_in sockSin = { 0 };
sockaddr_in sockSout = { 0 };
sockaddr_in sockSoin = { 0 };

void ReceiveText(WCHAR* Message, SLP_msg recived_msg, int socket_len); //получене сообщений

void SendText(SLP_msg msg, LPWSTR Send_Data, size_t Send_Data_Size);//отправка сообщений

void StartChat(HWND hwnd, LPWSTR Message);//вывод имени отправителя и сообщения на экран

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
	WCHAR UserName[MAX_USERNAME_SIZE] = L"";//имя отправителя
	WCHAR UserMessage[MAX_USERMESSAGE_SIZE] = L"";//сообщение + имя отправителя

	//объявим структуру с пакетом
	SLP_msg msg;
	
	ZeroMemory(msg.text, sizeof(msg.text));
	ZeroMemory(msg.username, sizeof(msg.username));


	//memset(msg.text, NULL, 10);
	//memset(msg.username, NULL, 20);

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
				Edit_ReplaceSel(hwndCtl, L"\r\n");
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

					StartChat(hwnd,Message);//отобрзим отправляемый текст у себя

					SendText(msg, Message, wcslen(Message));

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
		520, 10, 200, 30, hwnd, (HMENU)IDC_CLEAR_ALL, lpCreateStruct->hInstance, NULL);

	// создаём поле ввода
	HWND hwndCtl = CreateWindowEx(WS_EX_CLIENTEDGE, TEXT("Edit"), TEXT(""), dwStyle,
		10, 320, 490, 140, hwnd, (HMENU)IDC_EDIT_TEXT, lpCreateStruct->hInstance, NULL);

	//Для работы с адресами
		// создаём поле ввода IP-адреса
	CreateWindowEx(0, TEXT("SysIPAddress32"), NULL, WS_CHILD | WS_VISIBLE,
		520, 50, 200, 30, hwnd, (HMENU)IDC_IPADDR, lpCreateStruct->hInstance, NULL);

	//кнопка для переключения
	CreateWindowEx(0, TEXT("Button"), TEXT("Connect"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
		730, 50, 100, 30, hwnd, (HMENU)IDC_CONNECT, lpCreateStruct->hInstance, NULL);

	//кнопка для переключения
	CreateWindowEx(0, TEXT("Button"), TEXT("Disconnect"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
		730, 90, 100, 30, hwnd, (HMENU)IDC_DISCONNECT, lpCreateStruct->hInstance, NULL);

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
			case IDC_CONNECT:
			{
				// получим IP-адрес из поля ввода
				const char bufferNameIP[25] = { 0 };
				GetDlgItemTextA(hwnd, IDC_IPADDR, (LPSTR)bufferNameIP, _countof(bufferNameIP));

				if (bufferNameIP != NULL)
				{
					sockaddr_in sin = { 0 };
					sockSin.sin_family = AF_INET;
					sockSin.sin_port = htons(7581);
					sockSin.sin_addr.s_addr = inet_addr(bufferNameIP);//"192.168.56.104" "192.168.56.1"
			
					MessageBox(hwnd, L"IP-connect Enable", L"Details", MB_OK);
				}
				else
				{
					MessageBox(hwnd, L"Null IP", L"Details", MB_OK);
				}
		
			}
			break;
			case IDC_DISCONNECT:
			{
				// очистим поле ввода IP-адреса
				SendDlgItemMessage(hwnd, IDC_IPADDR, IPM_CLEARADDRESS,0,0);
			
				BOOL optval = TRUE;
				int optlen = sizeof(optval);
				int err = setsockopt(sockets, SOL_SOCKET, SO_BROADCAST, (char*)&optval, optlen);
				sockSin.sin_family = AF_INET;
				sockSin.sin_port = htons(7581);
				sockSin.sin_addr.s_addr = htonl(INADDR_BROADCAST);

				MessageBox(hwnd, L"BroadCast Enable", L"Details", MB_OK);
			}
			break;
		}
	}
}


// ----------------------------------------------------------------------------------------------
void SendText(SLP_msg msg, LPWSTR Send_Data, size_t Send_Data_Size)
{
	
	if ((Send_Data != NULL) && (Send_Data_Size != NULL))
	{
		int packnum = 0;//номер пакета

		msg.filelen = Send_Data_Size;

		for (unsigned int i = 0; i < Send_Data_Size; i+=10)
		{
			WCHAR frag_pack[FRAFMENT_PACK_SIZE] = { L"" }; //инициализация фрагмента пакета
			
			
			memcpy_s(frag_pack, sizeof(frag_pack), &Send_Data[i], sizeof(frag_pack));//скопируем данные в фрагмента пакета
			//memcpy(frag_pack, &Send_Data[i], sizeof(frag_pack));//скопируем данные в фрагмента пакета
			
			StringCchCat(msg.text,sizeof(msg.text), frag_pack);//скопируем данные в отправялемый пакет
			msg.numberfrag = packnum;//запишем номер пакета
			
			int result = sendto(sockets,(const char*)&msg, sizeof(msg), NULL,(struct sockaddr*)&sockSin, sizeof(sockSin));			
			
			packnum++;//увеличим номер пакета для следующей отправки
			ZeroMemory(msg.text, sizeof(msg.text));
		}
	}
}
				
void StartChat(HWND hwnd, LPWSTR Message)
{
	MessageBox(hwnd, L"LET'S GO CHAT!", L"CHAT", MB_OK);

	DWORD err = GetLastError();
	wchar_t* us = new wchar_t[wcslen(Message)+4];

	WCHAR UserMessage[MAX_USERMESSAGE_SIZE] = L"";
	//WCHAR UserMessage[MAX_USERMESSAGE_SIZE] = { L"" };

	

	HWND hwndCtl = GetDlgItem(hwnd, IDC_EDIT_MESSAGES);

	StringCchCat(UserMessage, _countof(UserMessage), L"\r\n");
	StringCchCat(UserMessage, _countof(UserMessage), Message);
	//StringCchCat(us, wcslen(us), L"\r\n");
	//StringCchCat(us, wcslen(us), Message);

	Edit_SetSel(hwndCtl, Edit_GetTextLength(hwndCtl), -1);// устанавливаем курсор в конец поля вывода

	SetFocus(hwnd);

	Edit_ReplaceSel(hwndCtl, UserMessage);// вставляем текст в поле вывода
	//Edit_ReplaceSel(hwndCtl, us);// вставляем текст в поле вывода
//	MessageBox(hwnd, L"LET'S GO CHAT!", L"CHAT", MB_OK);
} 

unsigned __stdcall ThreadFunc(void* lParam)
{
	/*Переменные*/
	//WCHAR Message[MAX_MESSAGE_SIZE] = _T("");		//сообщение
	WCHAR Message[MAX_MESSAGE_SIZE] = L"";			//сообщение
	WCHAR UserName[MAX_USERNAME_SIZE] = L"";		//имя отправителя
	WCHAR UserMessage[MAX_USERMESSAGE_SIZE] = L"";	//имя отправителя+сообщение

	SLP_msg recived_msg = {0};
	int socket_len  = sizeof(sockSout);

		for (;;)
		{
			//Начнем прием сообщений через сокет
			int result = recvfrom(sockets,(char*)&recived_msg,sizeof(recived_msg),NULL,(struct sockaddr*)&sockSout, &socket_len);
		
			if(result != SOCKET_ERROR)
			{
				//Тестирование на: ТестТестТестТестТестТестТестТест
				//ReceiveText(Message, recived_msg, socket_len);
				int struct_size = sizeof(recived_msg);//узнаем размер структуры
			
				int reseived_size = recived_msg.filelen / _countof(recived_msg.text); //количество пакетов

				/*Собираем сообщение*/
				for (int i = 0; i < MAX_MESSAGE_SIZE; i++)
				{
					//будем выполнять сдвиг указателя в массиве Message 
					//пока не дойдем до конца уже заполненной части массива
					if (i == recived_msg.numberfrag*FRAFMENT_PACK_SIZE)
					{
						StringCchCatW(Message, _countof(Message), recived_msg.text);
					}
				}	

				int recived = wcslen(Message);//не используется. Для просмотра размера при отладке

				//если определенный через размер номер равен номеру пакета, значит сообщение было получено верно

				if (reseived_size == recived_msg.numberfrag)
				{
					/*Заполнение массивов для вывода имени отправителя и сообщения*/
					StringCchCat(UserName, 20, recived_msg.username);

					StringCchCat(UserName, _countof(UserName), _T(":"));

					StringCchCat(UserMessage, _countof(UserMessage), UserName);

					StringCchCat(UserMessage, _countof(UserMessage), Message);

					StartChat(hwnd, UserMessage);

					/*Освобождение ресурсов и т.д.*/
					memset(UserName, NULL, 255);
					memset(UserMessage, NULL, 255);
					memset(Message, NULL, 255);
				}
			}
		}
		ZeroMemory(Message, sizeof(Message));
return(0);
}

