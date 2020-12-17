#include <Windows.h>
#include <stdio.h>
#include <tchar.h>
#include <locale.h>
#include <strsafe.h>
#include <fstream>
#include <iostream>

//extern LPCTSTR service_name; // имя службы
//extern LPCTSTR SvcDisplayName; // отображаемое имя службы

LPCTSTR service_name = TEXT("DemoService"); //// внутреннее имя сервиса, используемое SCM
LPCTSTR SvcDisplayName = TEXT("DemoService");// внешнее имя сервиса в панели управления

SERVICE_STATUS service_status; // текущее состояние службы
SERVICE_STATUS_HANDLE hServiceStatus; // дескриптор состояния службы

std::ofstream  out;   // выходной файл для протокола работы сервиса
int  nCount;     // счетчик

// ------------------------------------------------------------------------------------------------

BOOL OnSvcInit(DWORD dwArgc, LPTSTR *lpszArgv);// эта функция вызывается при запуске службы

void OnSvcStop(void);// эта функция вызывается для остановки службы

DWORD SvcMain(DWORD dwArgc, LPTSTR *lpszArgv);// в этой функции реализован основной функционал

// ------------------------------------------------------------------------------------------------
DWORD WINAPI SvcHandler(DWORD fdwControl, DWORD dwEventType, LPVOID lpEventData, LPVOID lpContext)
{
	if (SERVICE_CONTROL_STOP == fdwControl || SERVICE_CONTROL_SHUTDOWN == fdwControl)
	{
		OnSvcStop(); // останавливаем службу
		service_status.dwCurrentState = SERVICE_STOP_PENDING; // новое состояние службы
	}
	
	SetServiceStatus(hServiceStatus, &service_status);// изменяем текущее состояние службы
	return NO_ERROR;
} 

// ------------------------------------------------------------------------------------------------
void WINAPI ServiceMain(DWORD dwArgc, LPTSTR *lpszArgv)
{
	// регистрируем обработчик управляющих команд для сервиса
	hServiceStatus = RegisterServiceCtrlHandlerEx(service_name, SvcHandler, NULL);

	if (!hServiceStatus)
	{
		out.open("C:\\ServiceFile.log");
		out << "Register service control handler failed.";
		out.close();

		return;
	}

	if (NULL != hServiceStatus)
	{
		// начальное состояние
		service_status.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
		service_status.dwCurrentState = SERVICE_START_PENDING;
		service_status.dwControlsAccepted = SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_SHUTDOWN;
		service_status.dwWin32ExitCode = ERROR_SERVICE_SPECIFIC_ERROR;
		service_status.dwServiceSpecificExitCode = 0;
		service_status.dwCheckPoint = 0;
		service_status.dwWaitHint = 0;

		// задаем начальное состояние службы
		SetServiceStatus(hServiceStatus, &service_status);

		// /// //

		if (OnSvcInit(dwArgc, lpszArgv) != FALSE)
		{
			// определяем сервис как работающий 
			service_status.dwCurrentState = SERVICE_RUNNING; // новое состояние
		
			SetServiceStatus(hServiceStatus, &service_status);// изменяем текущее состояние службы

			DWORD dwExitCode = SvcMain(dwArgc, lpszArgv);

			if (dwExitCode != 0) // возвращаем код ошибки
			{
				service_status.dwWin32ExitCode = ERROR_SERVICE_SPECIFIC_ERROR;
				service_status.dwServiceSpecificExitCode = dwExitCode;
			} 
			else
			{
				service_status.dwWin32ExitCode = NO_ERROR;
			} 
		} 

		service_status.dwCurrentState = SERVICE_STOPPED; // новое состояние
		SetServiceStatus(hServiceStatus, &service_status);// задаем конечное состояние службы
	} 
} 

// ------------------------------------------------------------------------------------------------
int _tmain(int argc, LPTSTR argv[])
{
	_tsetlocale(LC_ALL, TEXT(""));

	if (argc < 2)
	{
		_tprintf(TEXT("> Не указан параметр.\n"));
		return 0; // завершаем работу приложения
	} // if


	if (_tcscmp(argv[1], TEXT("/runservice")) == 0) // начало работы службы
	{
		//  инициализируем структуру сервисов
		SERVICE_TABLE_ENTRY service_table[] =
		{
			{(LPTSTR)service_name, ServiceMain},// имя сервиса и функция сервиса
			{NULL, NULL} // больше сервисов нет
		};
//отследить в отладчике не получится, потому что требуется создание канала диспетчером, а им канал здесь не создается 
		if (!StartServiceCtrlDispatcher(service_table))
		{
			out.open("C:\\ServiceFile.log");
			out << "Start service control dispatcher failed.";
			out.close();
			return 0;
		}
		else
		{
			out.open("C:\\ServiceFile.log");
			out << "Start service control dispatcher okey.";
			out.close();
		}
	}
	
	if (_tcscmp(argv[1], TEXT("/create")) == 0) // создание службы
	{
			// связываемся с менеджером сервисов
			SC_HANDLE hServiceControlManager = OpenSCManager
			(NULL, //локальная машина
				NULL, //активная база данных сервисов
				SC_MANAGER_CREATE_SERVICE //возможно создание сервиса
			);
			if (hServiceControlManager == NULL)
			{
				std::cout << "Open service control manager failed." << std::endl
					<< "The last error code: " << GetLastError() << std::endl
					<< "Press any key to exit." << std::endl;
				std::cin.get();

				return -1;
			}

			TCHAR CmdLine[MAX_PATH + 13]; // командная строка

			// определяем путь и имя исполняемого файла
			GetModuleFileName(NULL, CmdLine, _countof(CmdLine));
			// добавляем аргумент командной строки
			StringCchCat(CmdLine, _countof(CmdLine), TEXT(" /runservice"));

			// создаем службу
			SC_HANDLE hService = CreateService(
				hServiceControlManager, //дескриптор менеджера сервисов
				service_name, //внтуренее имя сервиса
				SvcDisplayName, //внешнее имя отображаемое
				0,//нет полных прав контроля сервиса
				SERVICE_WIN32_OWN_PROCESS, // сервис является процессом
				SERVICE_DEMAND_START, // служба запускается "вручную"
				SERVICE_ERROR_NORMAL, CmdLine, NULL, NULL, NULL, NULL, NULL);

			if (hService == NULL)
			{
				std::cout << "Create service failed." << std::endl
					<< "The last error code: " << GetLastError() << std::endl
					<< "Press any key to exit." << std::endl;
				std::cin.get();

				// закрываем дескриптор менеджера сервисов
				CloseServiceHandle(hServiceControlManager);

				return 0;
			}
			else
			{
				std::cout << "Service is installed." << std::endl;

				std::cin.get();

				// закрываем дескрипторы
				CloseServiceHandle(hService);
				CloseServiceHandle(hServiceControlManager);

				return 0;
			}

		}

	if (_tcscmp(argv[1], TEXT("/delete")) == 0) // удаление службы
	{
		// открываем SCM
		SC_HANDLE hServiceControlManager = OpenSCManager(NULL, NULL, SC_MANAGER_CREATE_SERVICE);//третий параметр - соединение с менеджером

		if (hServiceControlManager == NULL)
		{
			std::cout << "Open service control manager failed." << std::endl
				<< "The last error code: " << GetLastError() << std::endl
				<< "Press any key to continue." << std::endl;
			std::cin.get();
			return -1;
		}
		else
		{
			std::cout << "Service is opened." << std::endl;
			std::cin.get();
		}


		// открываем службу для удаления
		SC_HANDLE hService = OpenService(hServiceControlManager, service_name, SERVICE_ALL_ACCESS | DELETE);

		if (hService == NULL)
		{
			std::cout << "Open service failed." << std::endl
				<< "The last error code: " << GetLastError() << std::endl
				<< "Press any key to exit." << std::endl;
			std::cin.get();

			// закрываем дескриптор менеджера сервисов
			CloseServiceHandle(hServiceControlManager);
			return 0;
		}

		// получаем состояние сервиса
		if (!QueryServiceStatus(hService, &service_status))
		{
			std::cout << "Query service status failed." << std::endl
				<< "The last error code: " << GetLastError() << std::endl
				<< "Press any key to exit." << std::endl;
			std::cin.get();

			// закрываем дескрипторы
			CloseServiceHandle(hServiceControlManager);
			CloseServiceHandle(hService);

			return 0;
		}

		// если сервис работает, то останавливаем его
		if (service_status.dwCurrentState != SERVICE_STOPPED)
		{
			std::cout << "Service is working. It will be stoped" << std::endl;
			if (!ControlService(hService, SERVICE_CONTROL_STOP, &service_status))
			{
				std::cout << "Control service failed." << std::endl
					<< "The last error code: " << GetLastError() << std::endl
					<< "Press any key to exit." << std::endl;
				std::cin.get();

				// закрываем дескрипторы
				CloseServiceHandle(hServiceControlManager);
				CloseServiceHandle(hService);

				return 0;
			}
			// ждем, пока сервис остановится
			Sleep(500);
		}

		if (!DeleteService(hService))
		{
			std::cout << "Delete service failed." << std::endl
				<< "The last error code: " << GetLastError() << std::endl
				<< "Press any key to exit." << std::endl;
			std::cin.get();

			// закрываем дескрипторы
			CloseServiceHandle(hServiceControlManager);
			CloseServiceHandle(hService);

			return 0;
		}
		else
		{
			std::cout << "The service is deleted." << std::endl
				<< "Press any key to exit." << std::endl;
			std::cin.get();
		}


		CloseServiceHandle(hService);
		CloseServiceHandle(hServiceControlManager); // закрываем дескриптор
		return 0; // завершаем работу приложения
	} // if

	_tprintf(TEXT("> Неизвестный параметр.\n"));
}