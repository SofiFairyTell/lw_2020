#include <Windows.h>
#include <WindowsX.h>
#include <CommCtrl.h>
#include <tchar.h>
#include <strsafe.h>
#include <process.h>

#define IDC_EDIT_MESSAGES           2001
#define IDC_EDIT_TEXT               2002
#define IDC_EDIT_USERNAME			2003
#define IDC_RBUTTON_WM_SETTEXT      2004
#define IDC_RBUTTON_WM_COPYDATA     2005
#define IDC_RBUTTON_CLIPBOARD       2006
#define IDC_RBUTTON_PIPE            2007


#define MAX_MESSAGE_SIZE            512
#define MAX_USERNAME_SIZE			20

/*Дескрипторы*/
HWND hwnd = NULL; // дескриптор главного окна
HKEY hKey = NULL; // дескриптор открытого ключа реестра для запущенных экзм
HANDLE hStopper = NULL; // событие для уведомления потока ThreadFuncClipboard о завершения приложения
HANDLE hThreads[2]; // дескрипторы созданных потоков

/*Оконные процедуры*/
LRESULT CALLBACK MainWindowProcess(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

BOOL PreTranslateMessage(LPMSG lpMsg);//вызывает перед передачей сообщений в оконную процедуру

/*Обработчики WM_CREATE, WM_DESTROY, WM_SIZE*/

BOOL OnCreate(HWND hwnd, LPCREATESTRUCT lpCreateStruct);
void OnDestroy(HWND hwnd);


/*---------Работа с реестром---------------*/
BOOL RegisterApplication();// регистрация каждого нового экземпляра приложения
void UregisterApplication();// удаление информации о каждом созданном экземпляре
/*---------------------------------------*/

/*-----Отправка оконных сообщений-------*/
void SendText(LPCTSTR lpText, DWORD cchText, BOOL fCopyData);// отправка с помощью оконных сообщений
void StartChat(HWND hwnd, LPCTSTR lpszText);// обработчик события WM_SETTEXT




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
	LoadLibrary(TEXT("ComCtl32.dll"));//

	/*Создание главного файла и обработка ошибки */
	hwnd = CreateWindowEx(0, TEXT("MainWindowProcess"), TEXT("Chat"),
		WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, 0, 520, 520, NULL, NULL, hInstance, NULL);

	if (hwnd == NULL)
	{
		return -1;
	}
	/*--------------------------------------------------*/
	/*Регистрация в системном реестре и обработка ошибки*/
	BOOL RetRes = RegisterApplication();// регистрируем приложение в системном реестре
	if (RetRes == FALSE)
	{
		return -1;
	}
	/*-----------------------------------------------*/

	ShowWindow(hwnd, CmdShow); // отображаем главное окно
	
	/*Цикл обработки сообщений*/
	MSG  msg;

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

	UregisterApplication();// удаляем информацию о приложении из системного реестра
	return (int)msg.wParam;
}


/*Процедура главного окна*/
LRESULT CALLBACK MainWindowProcess(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
		HANDLE_MSG(hWnd, WM_CREATE, OnCreate);
		HANDLE_MSG(hWnd, WM_DESTROY, OnDestroy);
		HANDLE_MSG(hWnd, WM_SETTEXT, StartChat);
	} 
	return DefWindowProc(hWnd, msg, wParam, lParam);
} 

/*Обработчик сообщений*/
BOOL PreTranslateMessage(LPMSG Msg)
{
	/*Переменные*/
	TCHAR Message[MAX_MESSAGE_SIZE]; //сообщение
	TCHAR UserName[MAX_USERNAME_SIZE];//имя отправителя
	TCHAR UserMessage[532] = _T("");

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
					
					StringCchCat(UserName, _countof(UserName), _T(":"));
					StringCchCat(Message, _countof(Message), _T("\n\n"));

					StringCchCat(UserMessage, _countof(UserMessage), UserName);

					StringCchCat(UserMessage, _countof(UserMessage), Message);

					SendText(UserMessage, _tcslen(UserMessage), FALSE);
	
				} 
			} 
			return TRUE;
		} 
	}
	return FALSE;
} 

BOOL OnCreate(HWND hwnd, LPCREATESTRUCT lpCreateStruct)
{
	// создаём событие для уведомления потока ThreadFuncClipboard о завершения приложения
	hStopper = CreateEventEx(NULL, NULL, CREATE_EVENT_MANUAL_RESET, EVENT_ALL_ACCESS);
	  

	DWORD dwStyle = WS_CHILD | WS_VISIBLE | WS_VSCROLL | WS_HSCROLL | ES_MULTILINE | ES_AUTOHSCROLL | ES_AUTOVSCROLL;

	// создаём поле вывода сообщений
	CreateWindowEx(WS_EX_STATICEDGE, TEXT("Edit"), TEXT(""), dwStyle | ES_READONLY,
		10, 10, 490, 250, hwnd, (HMENU)IDC_EDIT_MESSAGES, lpCreateStruct->hInstance, NULL);
	
	//Для username
	CreateWindowEx(0, TEXT("Static"), TEXT("Имя: "), WS_CHILD | WS_VISIBLE | SS_SIMPLE,
		10, 270, 40, 40, hwnd, NULL, lpCreateStruct->hInstance, NULL);
	
	CreateWindowEx(WS_EX_CLIENTEDGE, TEXT("Edit"), TEXT(""), WS_CHILD | WS_VISIBLE ,
		50, 270, 450, 40, hwnd, (HMENU)IDC_EDIT_USERNAME, lpCreateStruct->hInstance, NULL);
	
	// создаём поле ввода
	HWND hwndCtl = CreateWindowEx(WS_EX_CLIENTEDGE, TEXT("Edit"), TEXT(""), dwStyle,
		10, 320, 490, 140, hwnd, (HMENU)IDC_EDIT_TEXT, lpCreateStruct->hInstance, NULL);

	// задаём ограничение на ввод текста
	Edit_LimitText(hwndCtl, MAX_MESSAGE_SIZE);
	// передаём фокус полю ввода
	//SetFocus(hwndCtl);

	return TRUE;
} 

void OnDestroy(HWND hwnd)
{
	/*После закрытия окна приложения*/
	
	SetEvent(hStopper);//  завершить поток ThreadFuncClipboard
	WaitForMultipleObjects(_countof(hThreads), hThreads, TRUE, INFINITE);//ждем заверения остальных потоков
	CloseHandle(hStopper);// закрываем дескриптор события для завершении приложения
	PostQuitMessage(0); // отправляем сообщение WM_QUIT
} 


void StartChat(HWND hwnd, LPCTSTR Message)
{
	HWND hwndCtl = GetDlgItem(hwnd, IDC_EDIT_MESSAGES);
	Edit_SetSel(hwndCtl, Edit_GetTextLength(hwndCtl), -1);// устанавливаем курсор в конец поля вывода
	Edit_ReplaceSel(hwndCtl, Message);// вставляем текст в поле вывода
} 


BOOL RegisterApplication()
{
	/*Переменные*/
	DWORD lpdwDisposition;	//Адрес переменной, куда будет возвращено: Ключ не существует и был создан/ Ключ был открыт
	DWORD PID;				//идентификатор процесса

	LONG lStatus;		//результат создания ключа
	HKEY  SubKey_Handle = NULL;		// дескриптор вложенного ключа реестра, который будет содержать данные текущего приложения
	TCHAR SubKey_Name[20];			// имя вложенного ключа 

	// создаём и открываем ключ реестра HKEY_CURRENT_USER\Software\\IT-311

	lStatus = RegCreateKeyEx(HKEY_CURRENT_USER, TEXT("Software\\IT-311"),
		0, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hKey, &lpdwDisposition);

	if (lStatus == ERROR_SUCCESS)
	{
		PID = GetCurrentProcessId();// получим идентификатор текущего процесса

		StringCchPrintf(SubKey_Name, _countof(SubKey_Name), TEXT("%d"), PID);// формируем имя вложенного ключа реестра
		
		lStatus = RegCreateKey(hKey, SubKey_Name, &SubKey_Handle);// создаём вложенный ключ реестра

		if (ERROR_SUCCESS == lStatus)
		{
			RegSetValueEx(SubKey_Handle, NULL, 0, REG_DWORD, (LPBYTE)&PID, sizeof(DWORD));// сохраняем идентификатор текущего процесса		
			RegSetValueEx(SubKey_Handle, TEXT("hwnd"), 0, REG_BINARY, (LPBYTE)&hwnd, sizeof(HWND));// сохраняем значение дескриптор главного окна	
			RegCloseKey(SubKey_Handle), SubKey_Handle = NULL; // закрываем вложенный ключ реестра
			return TRUE;
		} 
		RegCloseKey(hKey), hKey = NULL;// закрываем ключ реестра
	} 
	return FALSE;
} 

void UregisterApplication()
{
	/*Переменные*/
	DWORD SubKeys_quant = 0; // количество вложенных ключей	
	TCHAR SubKey_Name[20]; // имя вложенного ключа реестра, который содержит данные текущего приложения
	LSTATUS lStatus; //результат определения количества ключей
	if (hKey != NULL)
	{
		lStatus = RegQueryInfoKey(hKey, NULL, NULL, NULL, &SubKeys_quant, NULL, NULL, NULL, NULL, NULL, NULL, NULL);

		if ((lStatus == ERROR_SUCCESS) && (SubKeys_quant < 2))
		{	
			RegDeleteTree(hKey, NULL);// удаляем всю ветку		
			RegDeleteKey(HKEY_CURRENT_USER, TEXT("Software\\IT-311"));// удаляем ключ
		} 
		else
		{	
			StringCchPrintf(SubKey_Name, _countof(SubKey_Name), TEXT("%d"), GetCurrentProcessId());	// формируем имя вложенного ключа реестра
			RegDeleteKey(hKey, SubKey_Name);	// удаляем вложенный ключ
		} 
		RegCloseKey(hKey), hKey = NULL;	// закрываем ключ реестра
	} 
} 

void SendText(LPCTSTR Message, DWORD Message_Size, BOOL DataCopy)
{
	/*-------Переменнные-------*/
	
	//дескрипторы
	HWND hRecvWnd; // дескриптор окна получателя
	HKEY hSubKey = NULL; // дескриптор вложенного ключа реестра
					 
	//для  вложенного ключа
	TCHAR SubKey_Name[20]; // имя вложенного ключа реестра
	DWORD SubKey_Name_sz; // для определения размера имени ключа
	DWORD SubKeys_quant = 0; // количество вложенных ключей
	LSTATUS lStatus; //для результатов с вложенными ключами

	DWORD lpcbData;//адрес переменной для размера буфера данных

	if ((Message != NULL) && (Message_Size > 0))
	{
		lStatus = RegQueryInfoKey(hKey, NULL, NULL, NULL, &SubKeys_quant, NULL, NULL, NULL, NULL, NULL, NULL, NULL);// определяем количество вложенных ключей

		if ((ERROR_SUCCESS == lStatus) && (SubKeys_quant > 0))
		{
			//начало перечисления ключей
			for (DWORD i = 0; i < SubKeys_quant; ++i)
			{		
				SubKey_Name_sz = _countof(SubKey_Name);
				lStatus = RegEnumKeyEx(hKey, i, SubKey_Name, &SubKey_Name_sz, NULL, NULL, NULL, NULL);//получим имя ключа по индексу

				if (lStatus == ERROR_SUCCESS)
				{			
					lStatus = RegOpenKeyEx(hKey, SubKey_Name, 0, KEY_QUERY_VALUE, &hSubKey);// открываем вложенный ключ
				} 

				if (lStatus == ERROR_SUCCESS)
				{
					
					lpcbData = sizeof(HWND); 
			
					lStatus = RegQueryValueEx(hSubKey, TEXT("hwnd"), NULL, NULL, (LPBYTE)&hRecvWnd, &lpcbData);// получаем дескриптор окна получателя

					if (lStatus == ERROR_SUCCESS)
					{
						SendMessage(hRecvWnd, WM_SETTEXT, 0, (LPARAM)Message);// отправляем сообщение WM_SETTEXT
					}
					RegCloseKey(hSubKey), hSubKey = NULL;// закрываем вложенный ключ реестра
				} 
			} 
		} 
	} 
} 


