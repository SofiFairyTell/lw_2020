//#include <Windows.h>
//#include <tchar.h>
//#include <strsafe.h>
//#include <process.h>
//
///*Переменные для служб*/
//LPCTSTR service_name = TEXT("WinService");// задаем имя службы
//LPCTSTR service_name_display = TEXT("Список студентов");// задаем отображаемое имя службы
//
///*Дескрипторы*/
//HANDLE hNamedPipe = INVALID_HANDLE_VALUE; // дескриптор именнованного канала
//
//HANDLE hStopper = NULL; // событие для завершения работы службы
//
//HANDLE hThread; // дескрипторы созданного потока
//
//SECURITY_DESCRIPTOR sd; // дескриптор безопасности
//SECURITY_ATTRIBUTES sa; //атрибуты безопасности
//
//BOOL RetRes;
//// функции потока, где обрабатываются запросы из канала
//unsigned __stdcall ThreadFuncPipe(void *lpParameter);
//
//// ------------------------------------------------------------------------------------------------
//BOOL OnSvcInit(DWORD dwArgc, LPTSTR *lpszArgv)
//{
//	/*Инициализация атрибутов безопасности*/
//	sa.nLength = sizeof(sa);
//
//	// инициализируем дескриптор безопасности
//	RetRes = InitializeSecurityDescriptor(&sd, SECURITY_DESCRIPTOR_REVISION);
//
//	if (FALSE != RetRes)
//		//разрешаем доступ к именнованному каналу всем пользователям
//		RetRes = SetSecurityDescriptorDacl(&sd, TRUE, NULL, FALSE);
//	sa.lpSecurityDescriptor = &sd;
//
//	if (FALSE != RetRes)
//	{
//		// создаём канал
//		hNamedPipe = CreateNamedPipe(
//			TEXT("\\\\.\\pipe\\test_pipe"),//имя канала
//			PIPE_ACCESS_DUPLEX | FILE_FLAG_OVERLAPPED, // читаем из канала и пишем в канала + асинхронность
//			PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT, //синхронная передача сообщений
//			PIPE_UNLIMITED_INSTANCES, //максимальное количесто экземпляров канала
//			0,		//размер выходного буфера по умолчанию
//			0,		//размер входного буфера по умолчанию
//			0,		//сколько клиент ждет связь. 
//			&sa);	//доступ для всех пользователей
//
//		if (hNamedPipe == INVALID_HANDLE_VALUE)
//		{
//			_tprintf(TEXT("> Не удалось создать канал.\n"));
//			return FALSE;
//		}
//	}
//	return RetRes;
//}
////--удалила OVERLAPPED
//void OnSvcStop(void)
//{
//	SetEvent(hStopper);// завершаем работу остальных потоков
//}
//// ------------------------------------------------------------------------------------------------
//DWORD SvcMain(DWORD dwArgc, LPTSTR *lpszArgv)
//{
//	// создаём событие для завершения работы службы
//	hStopper = CreateEventEx(NULL, NULL, CREATE_EVENT_MANUAL_RESET, EVENT_ALL_ACCESS);
//
//	// создаём поток, в котором будем обрабатывать запросы из канала
//	hThread = (HANDLE)_beginthreadex(NULL, 0, ThreadFuncPipe, NULL, 0, NULL);
//
//	WaitForSingleObject(hThread, INFINITE);// ожидаем завершения созданного потока
//
//	CloseHandle(hStopper);// закрываем дескриптор события
//
//	return 0;
//}
//
//// ------------------------------------------------------------------------------------------------
//
//// список студентов
//constexpr LPCTSTR szStudents[] = {
//	TEXT("Адонин Николай Николаевич ПИ-31"),
//	TEXT("Аладьина Анастасия Сергеевна ИТ-31"),
//	TEXT("Аникеев Денис Владимирович ИТ-31"),
//	TEXT("Арцханов Адам Борисович ИТ-31"),
//	TEXT("Бабакина Юлия Александровна ИТ-31"),
//	TEXT("Баженов Владислав Владимирович ИТ-31"),
//	TEXT("Болотов Александр Олегович ПИ-31"),
//	TEXT("Галеев Вячеслав Анатольевич ПИ-31"),
//	TEXT("Ершов Антон Юрьевич ПИ-31"),
//	TEXT("Зеленин Евгений Павлович ИТ-31"),
//	TEXT("Иванищева Яна Викторовна ПИ-31"),
//	TEXT("Ильченко Владислав Андреевич ИТ-31"),
//	TEXT("Исламшина Екатерина Сергеевна ПИ-31"),
//	TEXT("Кириенко Наталья Владимировна ИТ-31"),
//	TEXT("Клыч Артем Арманович ИТ-31"),
//	TEXT("Коршунов Артем Геннадьевич ИТ-31"),
//	TEXT("Лофицкая Алина Владимировна ПИ-31"),
//	TEXT("Мавлянов Руслан Джалилович ИТ-31"),
//	TEXT("Пацуков Михаил Романович ПИ-31"),
//	TEXT("Победа Михаил Романович ПИ-31"),
//	TEXT("Поляков Валентин Евгеньевич ИТ-31"),
//	TEXT("Романенко Дарья Александровна ПИ-31"),
//	TEXT("Ромащенко Николай Андреевич ИТ-31"),
//	TEXT("Симонова Мария Петровна ИТ-31"),
//	TEXT("Сирогитан Иван Игоревич ПИ-31"),
//	TEXT("Станкевич Ангелина Дмитриевна ИТ-31"),
//	TEXT("Стрельников Антон Павлович ИТ-31"),
//	TEXT("Сыровацкий Евгений Юрьевич ИТ-31"),
//	TEXT("Талалаев Андрей Витальевич ПИ-31"),
//	TEXT("Ткаченко Софья Александровна ПИ-31"),
//	TEXT("Шаповалов Дмитрий Иванович ИТ-31"),
//	TEXT("Шестаков Вадим Вячеславович ПИ-31"),
//	TEXT("Эймонт Сергей Вячеславович ИТ-31")
//};
//
//#pragma pack(push, 1)
//
//struct REQUEST
//{
//	DWORD PID;
//	DWORD INDEX;
//}; // struct REQUEST
//
//#pragma pack(pop)
//
//
//// ------------------------------------------------------------------------------------------------
//unsigned __stdcall ThreadFuncPipe(void *lpParameter)
//{
//	/*Переменные*/
//	DWORD res;
//
//
//	// создаём событие для ассинхронных операций с каналом
//	HANDLE hPipeEvent = CreateEventEx(NULL, NULL, 0, EVENT_ALL_ACCESS);
//
//	// массив событий
//	HANDLE hEvents[2] = { hStopper, hPipeEvent };
//
//	for (;;)
//	{
//		// инициализируем структуру OVERLAPPED ...
//		OVERLAPPED oConnect = { 0 };
//		oConnect.hEvent = hPipeEvent;
//
//		// ожидаем процесс-клиент и образуем с ним соединение
//		if (!ConnectNamedPipe(hNamedPipe, &oConnect))
//		{
//			_tprintf(TEXT("> Ошибка: %d\n"), GetLastError());
//			CloseHandle(hNamedPipe);
//			return 0;
//		}
//
//
//		res = WaitForMultipleObjects(2, hEvents, FALSE, INFINITE);// ожидаем одно из двух событий
//
//		if ((WAIT_OBJECT_0 == res) || (ERROR_SUCCESS != oConnect.Internal))
//		{
//			break; // (!) выходим из цикла
//		}
//
//		for (;;)
//		{
//			REQUEST Request; // текст сообщения
//			DWORD dwBytesRead;
//
//			//читаем сообщение от клиента
//			RetRes = ReadFile(hNamedPipe, &Request, sizeof(Request), &dwBytesRead, NULL);
//
//			if (RetRes == FALSE) break; // (!) ошибка: выходим из цикла
//
//			TCHAR Response[100] = TEXT(""); // ответ
//
//			if (Request.INDEX < _countof(szStudents))
//			{
//				StringCchCopy(Response, _countof(Response), szStudents[Request.INDEX]);
//			} // if
//
//			// запись данных в почтовый канал
//			WriteFile(hNamedPipe, Response, sizeof(Response), &dwBytesRead, NULL);
//		} // for
//
//		DisconnectNamedPipe(hNamedPipe); // разрываем соединение
//	} // for
//
//	// закрываем дескриптор события
//	CloseHandle(hPipeEvent);
//
//	// закрываем дескриптор канала
//	CloseHandle(hNamedPipe);
//
//	return 0;
//} // ThreadFuncPipe
//
