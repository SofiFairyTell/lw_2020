#include <Windows.h>
#include <stdio.h>
#include <tchar.h>
#include <locale.h>
#include <strsafe.h>

extern LPCTSTR gSvcName; // имя службы
extern LPCTSTR gSvcDisplayName; // отображаемое имя службы

SERVICE_STATUS gSvcStatus; // текущее состояние службы
SERVICE_STATUS_HANDLE gSvcStatusHandle; // дескриптор состояния службы

// ------------------------------------------------------------------------------------------------

// эта функция вызывается при запуске службы
BOOL OnSvcInit(DWORD dwArgc, LPTSTR *lpszArgv);
// эта функция вызывается для остановки службы
void OnSvcStop(void);
// в этой функции реализован основной функционал
DWORD SvcMain(DWORD dwArgc, LPTSTR *lpszArgv);

// ------------------------------------------------------------------------------------------------
DWORD WINAPI SvcHandler(DWORD fdwControl, DWORD dwEventType, LPVOID lpEventData, LPVOID lpContext)
{
	if (SERVICE_CONTROL_STOP == fdwControl || SERVICE_CONTROL_SHUTDOWN == fdwControl)
	{
		OnSvcStop(); // останавливаем службу
		gSvcStatus.dwCurrentState = SERVICE_STOP_PENDING; // новое состояние службы
	} // if

	// изменяем текущее состояние службы
	SetServiceStatus(gSvcStatusHandle, &gSvcStatus);
	return NO_ERROR;
} // ServiceControlHandler

// ------------------------------------------------------------------------------------------------
void WINAPI ServiceMain(DWORD dwArgc, LPTSTR *lpszArgv)
{
	// регистрируем функцию-обработчик
	gSvcStatusHandle = RegisterServiceCtrlHandlerEx(gSvcName, SvcHandler, NULL);

	if (NULL != gSvcStatusHandle)
	{
		// начальное состояние
		gSvcStatus.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
		gSvcStatus.dwCurrentState = SERVICE_START_PENDING;
		gSvcStatus.dwControlsAccepted = SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_SHUTDOWN;
		gSvcStatus.dwWin32ExitCode = NO_ERROR;
		gSvcStatus.dwServiceSpecificExitCode = 0;
		gSvcStatus.dwCheckPoint = 0;
		gSvcStatus.dwWaitHint = 0;

		// задаем начальное состояние службы
		SetServiceStatus(gSvcStatusHandle, &gSvcStatus);

		// /// //

		if (OnSvcInit(dwArgc, lpszArgv) != FALSE)
		{
			gSvcStatus.dwCurrentState = SERVICE_RUNNING; // новое состояние
			// изменяем текущее состояние службы
			SetServiceStatus(gSvcStatusHandle, &gSvcStatus);

			DWORD dwExitCode = SvcMain(dwArgc, lpszArgv);

			if (dwExitCode != 0) // возвращаем код ошибки
			{
				gSvcStatus.dwWin32ExitCode = ERROR_SERVICE_SPECIFIC_ERROR;
				gSvcStatus.dwServiceSpecificExitCode = dwExitCode;
			} // if
			else
			{
				gSvcStatus.dwWin32ExitCode = NO_ERROR;
			} // else
		} // if

		// /// //

		gSvcStatus.dwCurrentState = SERVICE_STOPPED; // новое состояние
		// задаем конечное состояние службы
		SetServiceStatus(gSvcStatusHandle, &gSvcStatus);
	} // if
} // ServiceMain

// ------------------------------------------------------------------------------------------------
int _tmain(int argc, LPTSTR argv[])
{
	_tsetlocale(LC_ALL, TEXT(""));

	if (argc < 2)
	{
		_tprintf(TEXT("> Не указан параметр.\n"));
		return 0; // завершаем работу приложения
	} // if

	// /// //

	if (_tcscmp(argv[1], TEXT("/runservice")) == 0) // начало работы службы
	{
		SERVICE_TABLE_ENTRY svcDispatchTable[] =
		{
			{(LPTSTR)gSvcName, ServiceMain},
			{NULL, NULL}
		};

		StartServiceCtrlDispatcher(svcDispatchTable);
		return 0; // завершаем работу приложения
	} // if

	// /// //

	if (_tcscmp(argv[1], TEXT("/create")) == 0) // создание службы
	{
		// открываем SCM
		SC_HANDLE hSCM = OpenSCManager(NULL, NULL, SC_MANAGER_CREATE_SERVICE);

		if (NULL == hSCM)
		{
			_tprintf(TEXT("> Не удалось открыть диспетчер управления службами Windows.\n"));
			return -1; // завершаем работу приложения
		} // if


		TCHAR szCmdLine[MAX_PATH + 13]; // командная строка

		// определяем путь и имя исполняемого файла
		GetModuleFileName(NULL, szCmdLine, _countof(szCmdLine));
		// добавляем аргумент командной строки
		StringCchCat(szCmdLine, _countof(szCmdLine), TEXT(" /runservice"));

		// создаем службу
		SC_HANDLE hSvc = CreateService(hSCM, gSvcName, gSvcDisplayName, 0,
			SERVICE_WIN32_OWN_PROCESS, // служба, которая работает в собственном процессе
			SERVICE_DEMAND_START, // служба запускается "вручную"
			SERVICE_ERROR_NORMAL, szCmdLine, NULL, NULL, NULL, NULL, NULL);

		if (NULL != hSvc)
		{
			_tprintf(TEXT("> Служба успешно создана!\n"));
			CloseServiceHandle(hSvc); // закрываем дескриптор
		} // if
		else
		{
			_tprintf(TEXT("> Не удалось создать службу.\n"));
		} // else

		CloseServiceHandle(hSCM); // закрываем дескриптор
		return 0; // завершаем работу приложения
	} // if

	// /// //

	if (_tcscmp(argv[1], TEXT("/delete")) == 0) // удаление службы
	{
		// открываем SCM
		SC_HANDLE hSCM = OpenSCManager(NULL, NULL, SC_MANAGER_CREATE_SERVICE);

		if (NULL == hSCM)
		{
			_tprintf(TEXT("> Не удалось открыть диспетчер управления службами Windows.\n"));
			return -1; // завершаем работу приложения
		} // if

		// открываем службу для удаления
		SC_HANDLE hSvc = OpenService(hSCM, gSvcName, DELETE);

		if (NULL != hSvc)
		{
			BOOL bRet = DeleteService(hSvc); // удаляем службу

			if (FALSE != bRet) _tprintf(TEXT("> Служба успешно удалена!\n"));
			else _tprintf(TEXT("> Не удалось удалить службу.\n"));

			CloseServiceHandle(hSvc); // закрываем дескриптор
		} // if
		else
		{
			_tprintf(TEXT("> Не удалось удалить службу.\n"));
		} // else

		CloseServiceHandle(hSCM); // закрываем дескриптор
		return 0; // завершаем работу приложения
	} // if

	// /// //

	_tprintf(TEXT("> Неизвестный параметр.\n"));
} // _tmain
