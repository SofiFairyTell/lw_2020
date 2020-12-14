#include <Windows.h>
#include <tchar.h>
#include <strsafe.h>
#include <process.h>

// задаем имя службы
LPCTSTR gSvcName = TEXT("SampleWinService");
// задаем отображаемое имя службы
LPCTSTR gSvcDisplayName = TEXT("Пример службы Windows");


HANDLE hPipe = INVALID_HANDLE_VALUE; // дескриптор канала


HANDLE hStopper = NULL; // событие для завершения работы службы

HANDLE hThreads[3]; // дескрипторы созданных потоков

// функции потока, где обрабатываются запросы из канала
unsigned __stdcall ThreadFuncPipe(void *lpParameter);

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
	
	} // if

	return bRet;
} // OnSvcInit

void OnSvcStop(void)
{
	// завершаем работу остальных потоков
	SetEvent(hStopper);
} // OnSvcStop
// ------------------------------------------------------------------------------------------------
DWORD SvcMain(DWORD dwArgc, LPTSTR *lpszArgv)
{
	// создаём событие для завершения работы службы
	hStopper = CreateEventEx(NULL, NULL, CREATE_EVENT_MANUAL_RESET, EVENT_ALL_ACCESS);

	// создаём поток, в котором будем обрабатывать запросы из канала
	hThreads[0] = (HANDLE)_beginthreadex(NULL, 0, ThreadFuncPipe, NULL, 0, NULL);


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

