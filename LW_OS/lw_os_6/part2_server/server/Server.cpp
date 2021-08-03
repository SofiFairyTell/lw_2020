#include <Windows.h>
#include <tchar.h>
#include <strsafe.h>
#include <process.h>


//LPCTSTR service_name = TEXT("DemoService"); //// ���������� ��� �������, ������������ SCM

//LPCTSTR SvcDisplayName = TEXT("DemoService");// ������� ��� ������� � ������ ����������


HANDLE hPipe = INVALID_HANDLE_VALUE; // ���������� ������


HANDLE hStopper = NULL; // ������� ��� ���������� ������ ������

HANDLE hThreads; // ����������� ��������� �������

// ������� ������, ��� �������������� ������� �� ������
unsigned __stdcall ThreadFuncPipe(void *lpParameter);

// ------------------------------------------------------------------------------------------------
BOOL OnSvcInit(DWORD dwArgc, LPTSTR *lpszArgv)
{
	SECURITY_DESCRIPTOR sd; // ���������� ������������

	// �������������� ���������� ������������
	BOOL RetRes = InitializeSecurityDescriptor(&sd, SECURITY_DESCRIPTOR_REVISION);

	if (FALSE != RetRes)
		RetRes = SetSecurityDescriptorDacl(&sd, TRUE, NULL, FALSE);

	if (FALSE != RetRes)
	{
		SECURITY_ATTRIBUTES sa = { sizeof(SECURITY_ATTRIBUTES) };
		sa.lpSecurityDescriptor = &sd;

		// /// //

		// ������ �����
		hPipe = CreateNamedPipe(TEXT("\\\\.\\pipe\\test_pipe"),
			PIPE_ACCESS_DUPLEX | FILE_FLAG_OVERLAPPED, // ���������, ��� ����� �������� ��� ������ � ������ ������
			PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT, // ��������� ����� ������ ������
			PIPE_UNLIMITED_INSTANCES, 0, 0, 0, &sa);

		if (INVALID_HANDLE_VALUE == hPipe)
		{
			_tprintf(TEXT("> �� ������� ������� �����.\n"));
			return FALSE;
		} 
	
	} 

	return RetRes;
} 

void OnSvcStop(void)
{
	// ��������� ������  ������
	SetEvent(hStopper);
} 
// ------------------------------------------------------------------------------------------------
DWORD SvcMain(DWORD dwArgc, LPTSTR *lpszArgv)
{
	// ������ ������� ��� ���������� ������ ������
	hStopper = CreateEventEx(NULL, NULL, CREATE_EVENT_MANUAL_RESET, EVENT_ALL_ACCESS);

	// ������ �����, � ������� ����� ������������ ������� �� ������
	hThreads = (HANDLE)_beginthreadex(NULL, 0, ThreadFuncPipe, NULL, 0, NULL);


	// ������� ���������� ����������������
	WaitForSingleObject(hThreads,INFINITE);

	// ��������� ���������� �������
	CloseHandle(hStopper);

	return 0;
} // SvcMain

// ------------------------------------------------------------------------------------------------

// ������ ���������
constexpr LPCTSTR Students[] = 
{
	L"�������� ������ ���������",
	L"�������� ����� ���������",
	L"���������� ������� ������������",
	L"���������� ������� ������������",
	L"��������� ��������� ���������",
	L"������� ����� �������������",
	L"��������� ����� ���������",
	L"������� ����� �����������"
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
	// ������ ������� ��� ������������ �������� � �������
	HANDLE hPipeEvent = CreateEventEx(NULL, NULL, 0, EVENT_ALL_ACCESS);

	// ������ �������
	HANDLE hEvents[2] = { hStopper, hPipeEvent };

	for (;;)
	{
		// �������������� ��������� OVERLAPPED ...
		OVERLAPPED oConnect = { 0 };

		oConnect.hEvent = hPipeEvent;
		
		ConnectNamedPipe(hPipe, &oConnect);// ������� �������-������ � �������� � ��� ����������
		
		DWORD dwResult = WaitForMultipleObjects(2, hEvents, FALSE, INFINITE);// ������� ���� �� ���� �������

		if ((WAIT_OBJECT_0 == dwResult) || (ERROR_SUCCESS != oConnect.Internal))
		{
			break; // (!) ������� �� �����
		} // if

		for (;;)
		{
			REQUEST Request; // ������
			DWORD nBytes;

			// ������ ������ �� ������
			BOOL bRet = ReadFile(hPipe, &Request, sizeof(Request), &nBytes, NULL);
			if (FALSE == bRet) break; // (!) ������: ������� �� �����

			TCHAR Response[100] = TEXT(""); // �����

			if (Request.index < _countof(Students))
			{
				/*��� ������� ����� ������������� �������� �� ������?? */
				StringCchCopy(Response, _countof(Response), Students[Request.index]);
			} // if

			// ������ ������ � �������� �����
			WriteFile(hPipe, Response, sizeof(Response), &nBytes, NULL);
		} // for

		DisconnectNamedPipe(hPipe); // ��������� ����������
	} // for

	// ��������� ���������� �������
	CloseHandle(hPipeEvent);

	// ��������� ���������� ������
	CloseHandle(hPipe);

	return 0;
} // ThreadFuncPipe

