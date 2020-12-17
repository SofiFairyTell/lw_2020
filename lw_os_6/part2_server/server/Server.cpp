#include <Windows.h>
#include <tchar.h>
#include <strsafe.h>
#include <process.h>


//LPCTSTR service_name = TEXT("DemoService"); //// внутреннее имя сервиса, используемое SCM

//LPCTSTR SvcDisplayName = TEXT("DemoService");// внешнее имя сервиса в панели управления


HANDLE hPipe = INVALID_HANDLE_VALUE; // дескриптор канала


HANDLE hStopper = NULL; // событие для завершения работы службы

HANDLE hThreads; // дескрипторы созданных потоков

// функции потока, где обрабатываются запросы из канала
unsigned __stdcall ThreadFuncPipe(void *lpParameter);

// ------------------------------------------------------------------------------------------------
BOOL OnSvcInit(DWORD dwArgc, LPTSTR *lpszArgv)
{
	SECURITY_DESCRIPTOR sd; // дескриптор безопасности

	// инициализируем дескриптор безопасности
	BOOL RetRes = InitializeSecurityDescriptor(&sd, SECURITY_DESCRIPTOR_REVISION);

	if (FALSE != RetRes)
		RetRes = SetSecurityDescriptorDacl(&sd, TRUE, NULL, FALSE);

	if (FALSE != RetRes)
	{
		SECURITY_ATTRIBUTES sa = { sizeof(SECURITY_ATTRIBUTES) };
		sa.lpSecurityDescriptor = &sd;

		// /// //

		// создаём канал
		hPipe = CreateNamedPipe(TEXT("\\\\.\\pipe\\test_pipe"),
			PIPE_ACCESS_DUPLEX | FILE_FLAG_OVERLAPPED, // указываем, что канал доступен для чтения и записи данных
			PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT, // указываем режим работы канала
			PIPE_UNLIMITED_INSTANCES, 0, 0, 0, &sa);

		if (INVALID_HANDLE_VALUE == hPipe)
		{
			_tprintf(TEXT("> Не удалось создать канал.\n"));
			return FALSE;
		} 
	
	} 

	return RetRes;
} 

void OnSvcStop(void)
{
	// завершаем работу  потока
	SetEvent(hStopper);
} 
// ------------------------------------------------------------------------------------------------
DWORD SvcMain(DWORD dwArgc, LPTSTR *lpszArgv)
{
	// создаём событие для завершения работы службы
	hStopper = CreateEventEx(NULL, NULL, CREATE_EVENT_MANUAL_RESET, EVENT_ALL_ACCESS);

	// создаём поток, в котором будем обрабатывать запросы из канала
	hThreads = (HANDLE)_beginthreadex(NULL, 0, ThreadFuncPipe, NULL, 0, NULL);


	// ожидаем завершения созданныхпотоков
	WaitForSingleObject(hThreads,INFINITE);

	// закрываем дескриптор события
	CloseHandle(hStopper);

	return 0;
} // SvcMain

// ------------------------------------------------------------------------------------------------

// список студентов
constexpr LPCTSTR Students[] = 
{
	L"Абаньшин Виктор Андреевич",
	L"Агафонов Данил Сергеевич",
	L"Анистратов Дмитрий Владимирович",
	L"Анистратов Евгений Владимирович",
	L"Бендриков Александр Сергеевич",
	L"Богунов Артем Александрович",
	L"Курбатова Софья Андреевна",
	L"Мануков Давид Альбертович"
};

#pragma pack(push, 1)

struct REQUEST
{
	DWORD PID;
	DWORD index;
}; // struct REQUEST

#pragma pack(pop)


// ------------------------------------------------------------------------------------------------
unsigned __stdcall ThreadFuncPipe(void *lpParameter)
{
	// создаём событие для ассинхронных операций с каналом
	HANDLE hPipeEvent = CreateEventEx(NULL, NULL, 0, EVENT_ALL_ACCESS);

	// массив событий
	HANDLE hEvents[2] = { hStopper, hPipeEvent };

	for (;;)
	{
		// инициализируем структуру OVERLAPPED ...
		OVERLAPPED oConnect = { 0 };

		oConnect.hEvent = hPipeEvent;
		
		ConnectNamedPipe(hPipe, &oConnect);// ожидаем процесс-клиент и образуем с ним соединение
		
		DWORD dwResult = WaitForMultipleObjects(2, hEvents, FALSE, INFINITE);// ожидаем одно из двух событий

		if ((WAIT_OBJECT_0 == dwResult) || (ERROR_SUCCESS != oConnect.Internal))
		{
			break; // (!) выходим из цикла
		} // if

		for (;;)
		{
			REQUEST Request; // запрос
			DWORD nBytes;

			// чтение данных из канала
			BOOL bRet = ReadFile(hPipe, &Request, sizeof(Request), &nBytes, NULL);
			if (FALSE == bRet) break; // (!) ошибка: выходим из цикла

			TCHAR Response[100] = TEXT(""); // ответ

			if (Request.index < _countof(Students))
			{
				StringCchCopy(Response, _countof(Response), Students[Request.index]);
			} // if

			// запись данных в почтовый канал
			WriteFile(hPipe, Response, sizeof(Response), &nBytes, NULL);
		} // for

		DisconnectNamedPipe(hPipe); // разрываем соединение
	} // for

	// закрываем дескриптор события
	CloseHandle(hPipeEvent);

	// закрываем дескриптор канала
	CloseHandle(hPipe);

	return 0;
} // ThreadFuncPipe

