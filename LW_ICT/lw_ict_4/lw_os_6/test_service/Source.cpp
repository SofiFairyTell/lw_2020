
#include <Windows.h>
#include <tchar.h>
#include <strsafe.h>
#include <process.h>


// задаем имя службы
LPCTSTR gSvcName = TEXT("SamWinService");
// задаем отображаемое имя службы
LPCTSTR gSvcDisplayName = TEXT("служба Windows");

HANDLE hReadMailslot = INVALID_HANDLE_VALUE; // дескриптор почтового ящика сервера

HANDLE hPipe = INVALID_HANDLE_VALUE; // дескриптор канала

HANDLE hShared = NULL; // дескриптор проекции файла
HANDLE hSharedEventServer = NULL; // событие для уведомления сервера о появлении данных в файле
HANDLE hSharedEventClient = NULL; // событие для уведомления клиента о появлении данных в файле
HANDLE hSharedMutex = NULL; // мьютекс для синхронизации доступа к проекции файла (используется только клиентами)

HANDLE hStopper = NULL; // событие для завершения работы службы

HANDLE hThreads[3]; // дескрипторы созданных потоков

BOOL ReadFileMapping(HANDLE hFileMapping, LPVOID lpData, DWORD cb);
BOOL WriteFileMapping(HANDLE hFileMapping, LPCVOID lpData, DWORD cb);


// функции потока, где обрабатываются запросы из почтового ящика
unsigned __stdcall ThreadFuncMailslot(void *lpParameter);
// функции потока, где обрабатываются запросы из канала
unsigned __stdcall ThreadFuncPipe(void *lpParameter);
// функции потока, где обрабатываются запросы из проекции файла
unsigned __stdcall ThreadFuncShared(void *lpParameter);

// ------------------------------------------------------------------------------------------------
BOOL OnSvcInit(DWORD dwArgc, LPTSTR *lpszArgv)
{
	SECURITY_DESCRIPTOR sd; // дескриптор безопасности

	// инициализируем дескриптор безопасности
	BOOL bRet = InitializeSecurityDescriptor(&sd, SECURITY_DESCRIPTOR_REVISION);

	if (FALSE != bRet)
		bRet = SetSecurityDescriptorDacl(&sd, TRUE, NULL, FALSE);

	if (FALSE != bRet)
	{
		SECURITY_ATTRIBUTES sa = { sizeof(SECURITY_ATTRIBUTES) };
		sa.lpSecurityDescriptor = &sd;

		// /// //

		// создаём почтовый ящик
		hReadMailslot = CreateMailslot(TEXT("\\\\.\\mailslot\\SampleMailslot\\0"), 0, MAILSLOT_WAIT_FOREVER, &sa);

		if (INVALID_HANDLE_VALUE == hReadMailslot)
		{
			// не удалось создать почтовый ящик
			return FALSE;
		} // if

		// /// //

		// создаём канал
		hPipe = CreateNamedPipe(TEXT("\\\\.\\pipe\\SamplePipe"),
			PIPE_ACCESS_DUPLEX | FILE_FLAG_OVERLAPPED, // указываем, что канал доступен для чтения и записи данных
			PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT, // указываем режим работы канала
			PIPE_UNLIMITED_INSTANCES, 0, 0, 0, &sa);

		if (INVALID_HANDLE_VALUE == hPipe)
		{
			// не удалось создать канал
			return FALSE;
		} // if

		// /// //

		// создаём проекцию страничного файла (файла подкачки)
		hShared = CreateFileMapping(INVALID_HANDLE_VALUE, &sa, PAGE_READWRITE, 0, 100 * sizeof(TCHAR), TEXT("Global\\SampleShared"));

		if (NULL == hShared)
		{
			// не удалось создать проекцию страничного файла
			return FALSE;
		} // if

		// создаём событие для уведомления сервера о появлении данных в файле
		hSharedEventServer = CreateEventEx(&sa, TEXT("Global\\SampleSharedEventServer"), 0, EVENT_ALL_ACCESS);

		if (NULL == hSharedEventServer)
		{
			// не удалось создать событие
			return FALSE;
		} // if

		// создаём событие для уведомления клиента о появлении данных в файле
		hSharedEventClient = CreateEventEx(&sa, TEXT("Global\\SampleSharedEventClient"), 0, EVENT_ALL_ACCESS);

		if (NULL == hSharedEventClient)
		{
			// не удалось создать событие
			return FALSE;
		} // if

		// создаём мьютекс для синхронизации доступа к проекции файла
		hSharedMutex = CreateMutexEx(&sa, TEXT("Global\\SampleSharedMutex"), 0, MUTEX_ALL_ACCESS);

		if (NULL == hSharedMutex)
		{
			// не удалось создать мьютекс
			return FALSE;
		} // if
	} // if

	return bRet;
} // OnSvcInit

// ------------------------------------------------------------------------------------------------
void OnSvcStop(void)
{
	// закрываем дескриптор почтового ящика сервера,
	// чтобы завершить поток ThreadFuncMailslot
	CloseHandle(hReadMailslot);

	// завершаем работу остальных потоков
	SetEvent(hStopper);
} // OnSvcStop

// ------------------------------------------------------------------------------------------------
DWORD SvcMain(DWORD dwArgc, LPTSTR *lpszArgv)
{
	// создаём событие для завершения работы службы
	hStopper = CreateEventEx(NULL, NULL, CREATE_EVENT_MANUAL_RESET, EVENT_ALL_ACCESS);

	// создаём поток, в котором будем обрабатывать запросы из почтового ящика
	hThreads[0] = (HANDLE)_beginthreadex(NULL, 0, ThreadFuncMailslot, NULL, 0, NULL);
	// создаём поток, в котором будем обрабатывать запросы из канала
	hThreads[1] = (HANDLE)_beginthreadex(NULL, 0, ThreadFuncPipe, NULL, 0, NULL);
	// создаём поток, в котором будем обрабатывать запросы из проекции файла
	hThreads[2] = (HANDLE)_beginthreadex(NULL, 0, ThreadFuncShared, NULL, 0, NULL);

	// ожидаем завершения созданных потоков
	WaitForMultipleObjects(_countof(hThreads), hThreads, TRUE, INFINITE);

	// закрываем дескриптор события
	CloseHandle(hStopper);

	return 0;
} // SvcMain

// ------------------------------------------------------------------------------------------------

// список студентов
constexpr LPCTSTR szStudents[] = {
	TEXT("Адонин Николай Николаевич ПИ-31"),
	TEXT("Аладьина Анастасия Сергеевна ИТ-31"),
	TEXT("Аникеев Денис Владимирович ИТ-31"),
	TEXT("Арцханов Адам Борисович ИТ-31"),
	TEXT("Бабакина Юлия Александровна ИТ-31"),
	TEXT("Баженов Владислав Владимирович ИТ-31"),
	TEXT("Болотов Александр Олегович ПИ-31"),
	TEXT("Галеев Вячеслав Анатольевич ПИ-31"),
	TEXT("Ершов Антон Юрьевич ПИ-31"),
	TEXT("Зеленин Евгений Павлович ИТ-31"),
	TEXT("Иванищева Яна Викторовна ПИ-31"),
	TEXT("Ильченко Владислав Андреевич ИТ-31"),
	TEXT("Исламшина Екатерина Сергеевна ПИ-31"),
	TEXT("Кириенко Наталья Владимировна ИТ-31"),
	TEXT("Клыч Артем Арманович ИТ-31"),
	TEXT("Коршунов Артем Геннадьевич ИТ-31"),
	TEXT("Лофицкая Алина Владимировна ПИ-31"),
	TEXT("Мавлянов Руслан Джалилович ИТ-31"),
	TEXT("Пацуков Михаил Романович ПИ-31"),
	TEXT("Победа Михаил Романович ПИ-31"),
	TEXT("Поляков Валентин Евгеньевич ИТ-31"),
	TEXT("Романенко Дарья Александровна ПИ-31"),
	TEXT("Ромащенко Николай Андреевич ИТ-31"),
	TEXT("Симонова Мария Петровна ИТ-31"),
	TEXT("Сирогитан Иван Игоревич ПИ-31"),
	TEXT("Станкевич Ангелина Дмитриевна ИТ-31"),
	TEXT("Стрельников Антон Павлович ИТ-31"),
	TEXT("Сыровацкий Евгений Юрьевич ИТ-31"),
	TEXT("Талалаев Андрей Витальевич ПИ-31"),
	TEXT("Ткаченко Софья Александровна ПИ-31"),
	TEXT("Шаповалов Дмитрий Иванович ИТ-31"),
	TEXT("Шестаков Вадим Вячеславович ПИ-31"),
	TEXT("Эймонт Сергей Вячеславович ИТ-31")
};

#pragma pack(push, 1)

struct REQUEST
{
	DWORD dwProcessId;
	DWORD dwIndex;
}; // struct REQUEST

#pragma pack(pop)

// ------------------------------------------------------------------------------------------------
unsigned __stdcall ThreadFuncMailslot(void *lpParameter)
{
	TCHAR szNameMailslot[256]; // имя почтового ящика для ответа клиенту

	for (;;)
	{
		REQUEST Request; // запрос
		DWORD nBytes;

		// читаем сообщение из почтового ящика
		BOOL bRet = ReadFile(hReadMailslot, &Request, sizeof(Request), &nBytes, NULL);

		if (FALSE != bRet)
		{
			// формируем имя почтового ящика для ответа клиенту
			StringCchPrintf(szNameMailslot, _countof(szNameMailslot), TEXT("\\\\.\\mailslot\\SampleMailslot\\%u"), Request.dwProcessId);
			// открываем почтовый ящик клиента
			HANDLE hWriteMailslot = CreateFile(szNameMailslot, GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);

			if (INVALID_HANDLE_VALUE != hWriteMailslot)
			{
				TCHAR szResponse[100] = TEXT(""); // ответ

				if (Request.dwIndex < _countof(szStudents))
				{
					StringCchCopy(szResponse, _countof(szResponse), szStudents[Request.dwIndex]);
				} // if

				// запись данных в почтовый ящик
				WriteFile(hWriteMailslot, szResponse, sizeof(szResponse), &nBytes, NULL);

				// закрываем почтовый ящик
				CloseHandle(hWriteMailslot);
			} // if
		} // if
		else
		{
			// получим код последней ошибки
			DWORD dwError = GetLastError();

			if (ERROR_INVALID_HANDLE == dwError)
			{
				/* почтовый ящик был закрыт */
				break; // (!) выходим из цикла
			} // if
			else if (ERROR_INSUFFICIENT_BUFFER == dwError)
			{
				/* размер буфера слишком мал */
			} // if
		} // else
	} // for

	return 0;
} // ThreadFuncMailslot

// ------------------------------------------------------------------------------------------------
unsigned __stdcall ThreadFuncPipe(void *lpParameter)
{
	// создаём событие для ассинхронных операций с каналом
	HANDLE hPipeEvent = CreateEventEx(NULL, NULL, 0, EVENT_ALL_ACCESS);

	// массив собитий
	HANDLE hEvents[2] = { hStopper, hPipeEvent };

	for (;;)
	{
		// инициализируем структуру OVERLAPPED ...
		OVERLAPPED oConnect = { 0 };
		oConnect.hEvent = hPipeEvent;
		// ожидаем процесс-клиент и образуем с ним соединение
		ConnectNamedPipe(hPipe, &oConnect);

		// ожидаем одно из двух событий
		DWORD dwResult = WaitForMultipleObjects(2, hEvents, FALSE, INFINITE);

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

			TCHAR szResponse[100] = TEXT(""); // ответ

			if (Request.dwIndex < _countof(szStudents))
			{
				StringCchCopy(szResponse, _countof(szResponse), szStudents[Request.dwIndex]);
			} // if

			// запись данных в почтовый канал
			WriteFile(hPipe, szResponse, sizeof(szResponse), &nBytes, NULL);
		} // for

		DisconnectNamedPipe(hPipe); // разрываем соединение
	} // for

	// закрываем дескриптор события
	CloseHandle(hPipeEvent);

	// закрываем дескриптор канала
	CloseHandle(hPipe);

	return 0;
} // ThreadFuncPipe

// ------------------------------------------------------------------------------------------------
unsigned __stdcall ThreadFuncShared(void *lpParameter)
{
	// массив собитий
	HANDLE events[2] = { hStopper, hSharedEventServer };

	for (;;)
	{
		// ожидаем одно из двух событий
		DWORD dwResult = WaitForMultipleObjects(2, events, FALSE, INFINITE);

		if (WAIT_OBJECT_0 == dwResult) // завершение работы службы
		{
			break; // выходим из цикла
		} // if
		else if ((WAIT_OBJECT_0 + 1) == dwResult) // новый запрос
		{
			REQUEST Request; // запрос

			// читаем данные
			BOOL bRet = ReadFileMapping(hShared, &Request, sizeof(Request));
			if (FALSE == bRet) break; // (!) ошибка: выходим из цикла

			TCHAR szResponse[100] = TEXT(""); // ответ

			if (Request.dwIndex < _countof(szStudents))
			{
				StringCchCopy(szResponse, _countof(szResponse), szStudents[Request.dwIndex]);
			} // if

			// записываем данные
			WriteFileMapping(hShared, szResponse, sizeof(szResponse));

			// уведомляем клента о наличие данных
			SetEvent(hSharedEventClient);
		} // if
	} // for

	// закрываем дескриптор проекции файла
	CloseHandle(hShared);
	// закрываем дескриптор события
	CloseHandle(hSharedEventServer);
	// закрываем дескриптор события
	CloseHandle(hSharedEventClient);
	// закрываем дескриптор мьютекса
	CloseHandle(hSharedMutex);

	return 0;
} // ThreadFuncShared

// ------------------------------------------------------------------------------------------------
BOOL WriteFileMapping(HANDLE hFileMapping, LPCVOID lpData, DWORD cb)
{
	// проецируем файл на регион в адресном пространстве процесса 
	PBYTE pbFile = (PBYTE)MapViewOfFile(hFileMapping, FILE_MAP_WRITE, 0, 0, cb);
	if (NULL == pbFile) return FALSE;

	// записываем в выделенный регион передаваемые данные
	CopyMemory(pbFile, lpData, cb);

	// принудительно записываем данные из региона на диск
	BOOL bRet = FlushViewOfFile(pbFile, cb);

	// освобождаем выделенный регион
	UnmapViewOfFile(pbFile);

	return bRet;
} // WriteFileMapping

// ------------------------------------------------------------------------------------------------
BOOL ReadFileMapping(HANDLE hFileMapping, LPVOID lpData, DWORD cb)
{
	// проецируем файл на регион в адресном пространстве процесса
	PBYTE pbFile = (PBYTE)MapViewOfFile(hFileMapping, FILE_MAP_READ, 0, 0, cb);
	if (NULL == pbFile) return FALSE;

	// считываем из выделенного региона переданные данные
	CopyMemory(lpData, pbFile, cb);

	// освобождаем выделенный регион
	UnmapViewOfFile(pbFile);

	return TRUE;
} // ReadFileMapping
