
#include <Windows.h>
#include <tchar.h>
#include <strsafe.h>
#include <process.h>


// ������ ��� ������
LPCTSTR gSvcName = TEXT("SamWinService");
// ������ ������������ ��� ������
LPCTSTR gSvcDisplayName = TEXT("������ Windows");

HANDLE hReadMailslot = INVALID_HANDLE_VALUE; // ���������� ��������� ����� �������

HANDLE hPipe = INVALID_HANDLE_VALUE; // ���������� ������

HANDLE hShared = NULL; // ���������� �������� �����
HANDLE hSharedEventServer = NULL; // ������� ��� ����������� ������� � ��������� ������ � �����
HANDLE hSharedEventClient = NULL; // ������� ��� ����������� ������� � ��������� ������ � �����
HANDLE hSharedMutex = NULL; // ������� ��� ������������� ������� � �������� ����� (������������ ������ ���������)

HANDLE hStopper = NULL; // ������� ��� ���������� ������ ������

HANDLE hThreads[3]; // ����������� ��������� �������

BOOL ReadFileMapping(HANDLE hFileMapping, LPVOID lpData, DWORD cb);
BOOL WriteFileMapping(HANDLE hFileMapping, LPCVOID lpData, DWORD cb);


// ������� ������, ��� �������������� ������� �� ��������� �����
unsigned __stdcall ThreadFuncMailslot(void *lpParameter);
// ������� ������, ��� �������������� ������� �� ������
unsigned __stdcall ThreadFuncPipe(void *lpParameter);
// ������� ������, ��� �������������� ������� �� �������� �����
unsigned __stdcall ThreadFuncShared(void *lpParameter);

// ------------------------------------------------------------------------------------------------
BOOL OnSvcInit(DWORD dwArgc, LPTSTR *lpszArgv)
{
	SECURITY_DESCRIPTOR sd; // ���������� ������������

	// �������������� ���������� ������������
	BOOL bRet = InitializeSecurityDescriptor(&sd, SECURITY_DESCRIPTOR_REVISION);

	if (FALSE != bRet)
		bRet = SetSecurityDescriptorDacl(&sd, TRUE, NULL, FALSE);

	if (FALSE != bRet)
	{
		SECURITY_ATTRIBUTES sa = { sizeof(SECURITY_ATTRIBUTES) };
		sa.lpSecurityDescriptor = &sd;

		// /// //

		// ������ �������� ����
		hReadMailslot = CreateMailslot(TEXT("\\\\.\\mailslot\\SampleMailslot\\0"), 0, MAILSLOT_WAIT_FOREVER, &sa);

		if (INVALID_HANDLE_VALUE == hReadMailslot)
		{
			// �� ������� ������� �������� ����
			return FALSE;
		} // if

		// /// //

		// ������ �����
		hPipe = CreateNamedPipe(TEXT("\\\\.\\pipe\\SamplePipe"),
			PIPE_ACCESS_DUPLEX | FILE_FLAG_OVERLAPPED, // ���������, ��� ����� �������� ��� ������ � ������ ������
			PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT, // ��������� ����� ������ ������
			PIPE_UNLIMITED_INSTANCES, 0, 0, 0, &sa);

		if (INVALID_HANDLE_VALUE == hPipe)
		{
			// �� ������� ������� �����
			return FALSE;
		} // if

		// /// //

		// ������ �������� ����������� ����� (����� ��������)
		hShared = CreateFileMapping(INVALID_HANDLE_VALUE, &sa, PAGE_READWRITE, 0, 100 * sizeof(TCHAR), TEXT("Global\\SampleShared"));

		if (NULL == hShared)
		{
			// �� ������� ������� �������� ����������� �����
			return FALSE;
		} // if

		// ������ ������� ��� ����������� ������� � ��������� ������ � �����
		hSharedEventServer = CreateEventEx(&sa, TEXT("Global\\SampleSharedEventServer"), 0, EVENT_ALL_ACCESS);

		if (NULL == hSharedEventServer)
		{
			// �� ������� ������� �������
			return FALSE;
		} // if

		// ������ ������� ��� ����������� ������� � ��������� ������ � �����
		hSharedEventClient = CreateEventEx(&sa, TEXT("Global\\SampleSharedEventClient"), 0, EVENT_ALL_ACCESS);

		if (NULL == hSharedEventClient)
		{
			// �� ������� ������� �������
			return FALSE;
		} // if

		// ������ ������� ��� ������������� ������� � �������� �����
		hSharedMutex = CreateMutexEx(&sa, TEXT("Global\\SampleSharedMutex"), 0, MUTEX_ALL_ACCESS);

		if (NULL == hSharedMutex)
		{
			// �� ������� ������� �������
			return FALSE;
		} // if
	} // if

	return bRet;
} // OnSvcInit

// ------------------------------------------------------------------------------------------------
void OnSvcStop(void)
{
	// ��������� ���������� ��������� ����� �������,
	// ����� ��������� ����� ThreadFuncMailslot
	CloseHandle(hReadMailslot);

	// ��������� ������ ��������� �������
	SetEvent(hStopper);
} // OnSvcStop

// ------------------------------------------------------------------------------------------------
DWORD SvcMain(DWORD dwArgc, LPTSTR *lpszArgv)
{
	// ������ ������� ��� ���������� ������ ������
	hStopper = CreateEventEx(NULL, NULL, CREATE_EVENT_MANUAL_RESET, EVENT_ALL_ACCESS);

	// ������ �����, � ������� ����� ������������ ������� �� ��������� �����
	hThreads[0] = (HANDLE)_beginthreadex(NULL, 0, ThreadFuncMailslot, NULL, 0, NULL);
	// ������ �����, � ������� ����� ������������ ������� �� ������
	hThreads[1] = (HANDLE)_beginthreadex(NULL, 0, ThreadFuncPipe, NULL, 0, NULL);
	// ������ �����, � ������� ����� ������������ ������� �� �������� �����
	hThreads[2] = (HANDLE)_beginthreadex(NULL, 0, ThreadFuncShared, NULL, 0, NULL);

	// ������� ���������� ��������� �������
	WaitForMultipleObjects(_countof(hThreads), hThreads, TRUE, INFINITE);

	// ��������� ���������� �������
	CloseHandle(hStopper);

	return 0;
} // SvcMain

// ------------------------------------------------------------------------------------------------

// ������ ���������
constexpr LPCTSTR szStudents[] = {
	TEXT("������ ������� ���������� ��-31"),
	TEXT("�������� ��������� ��������� ��-31"),
	TEXT("������� ����� ������������ ��-31"),
	TEXT("�������� ���� ��������� ��-31"),
	TEXT("�������� ���� ������������� ��-31"),
	TEXT("������� ��������� ������������ ��-31"),
	TEXT("������� ��������� �������� ��-31"),
	TEXT("������ �������� ����������� ��-31"),
	TEXT("����� ����� ������� ��-31"),
	TEXT("������� ������� �������� ��-31"),
	TEXT("��������� ��� ���������� ��-31"),
	TEXT("�������� ��������� ��������� ��-31"),
	TEXT("��������� ��������� ��������� ��-31"),
	TEXT("�������� ������� ������������ ��-31"),
	TEXT("���� ����� ��������� ��-31"),
	TEXT("�������� ����� ����������� ��-31"),
	TEXT("�������� ����� ������������ ��-31"),
	TEXT("�������� ������ ���������� ��-31"),
	TEXT("������� ������ ��������� ��-31"),
	TEXT("������ ������ ��������� ��-31"),
	TEXT("������� �������� ���������� ��-31"),
	TEXT("��������� ����� ������������� ��-31"),
	TEXT("��������� ������� ��������� ��-31"),
	TEXT("�������� ����� �������� ��-31"),
	TEXT("��������� ���� �������� ��-31"),
	TEXT("��������� �������� ���������� ��-31"),
	TEXT("����������� ����� �������� ��-31"),
	TEXT("���������� ������� ������� ��-31"),
	TEXT("�������� ������ ���������� ��-31"),
	TEXT("�������� ����� ������������� ��-31"),
	TEXT("��������� ������� �������� ��-31"),
	TEXT("�������� ����� ������������ ��-31"),
	TEXT("������ ������ ������������ ��-31")
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
	TCHAR szNameMailslot[256]; // ��� ��������� ����� ��� ������ �������

	for (;;)
	{
		REQUEST Request; // ������
		DWORD nBytes;

		// ������ ��������� �� ��������� �����
		BOOL bRet = ReadFile(hReadMailslot, &Request, sizeof(Request), &nBytes, NULL);

		if (FALSE != bRet)
		{
			// ��������� ��� ��������� ����� ��� ������ �������
			StringCchPrintf(szNameMailslot, _countof(szNameMailslot), TEXT("\\\\.\\mailslot\\SampleMailslot\\%u"), Request.dwProcessId);
			// ��������� �������� ���� �������
			HANDLE hWriteMailslot = CreateFile(szNameMailslot, GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);

			if (INVALID_HANDLE_VALUE != hWriteMailslot)
			{
				TCHAR szResponse[100] = TEXT(""); // �����

				if (Request.dwIndex < _countof(szStudents))
				{
					StringCchCopy(szResponse, _countof(szResponse), szStudents[Request.dwIndex]);
				} // if

				// ������ ������ � �������� ����
				WriteFile(hWriteMailslot, szResponse, sizeof(szResponse), &nBytes, NULL);

				// ��������� �������� ����
				CloseHandle(hWriteMailslot);
			} // if
		} // if
		else
		{
			// ������� ��� ��������� ������
			DWORD dwError = GetLastError();

			if (ERROR_INVALID_HANDLE == dwError)
			{
				/* �������� ���� ��� ������ */
				break; // (!) ������� �� �����
			} // if
			else if (ERROR_INSUFFICIENT_BUFFER == dwError)
			{
				/* ������ ������ ������� ��� */
			} // if
		} // else
	} // for

	return 0;
} // ThreadFuncMailslot

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
		// ������� �������-������ � �������� � ��� ����������
		ConnectNamedPipe(hPipe, &oConnect);

		// ������� ���� �� ���� �������
		DWORD dwResult = WaitForMultipleObjects(2, hEvents, FALSE, INFINITE);

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

			TCHAR szResponse[100] = TEXT(""); // �����

			if (Request.dwIndex < _countof(szStudents))
			{
				StringCchCopy(szResponse, _countof(szResponse), szStudents[Request.dwIndex]);
			} // if

			// ������ ������ � �������� �����
			WriteFile(hPipe, szResponse, sizeof(szResponse), &nBytes, NULL);
		} // for

		DisconnectNamedPipe(hPipe); // ��������� ����������
	} // for

	// ��������� ���������� �������
	CloseHandle(hPipeEvent);

	// ��������� ���������� ������
	CloseHandle(hPipe);

	return 0;
} // ThreadFuncPipe

// ------------------------------------------------------------------------------------------------
unsigned __stdcall ThreadFuncShared(void *lpParameter)
{
	// ������ �������
	HANDLE events[2] = { hStopper, hSharedEventServer };

	for (;;)
	{
		// ������� ���� �� ���� �������
		DWORD dwResult = WaitForMultipleObjects(2, events, FALSE, INFINITE);

		if (WAIT_OBJECT_0 == dwResult) // ���������� ������ ������
		{
			break; // ������� �� �����
		} // if
		else if ((WAIT_OBJECT_0 + 1) == dwResult) // ����� ������
		{
			REQUEST Request; // ������

			// ������ ������
			BOOL bRet = ReadFileMapping(hShared, &Request, sizeof(Request));
			if (FALSE == bRet) break; // (!) ������: ������� �� �����

			TCHAR szResponse[100] = TEXT(""); // �����

			if (Request.dwIndex < _countof(szStudents))
			{
				StringCchCopy(szResponse, _countof(szResponse), szStudents[Request.dwIndex]);
			} // if

			// ���������� ������
			WriteFileMapping(hShared, szResponse, sizeof(szResponse));

			// ���������� ������ � ������� ������
			SetEvent(hSharedEventClient);
		} // if
	} // for

	// ��������� ���������� �������� �����
	CloseHandle(hShared);
	// ��������� ���������� �������
	CloseHandle(hSharedEventServer);
	// ��������� ���������� �������
	CloseHandle(hSharedEventClient);
	// ��������� ���������� ��������
	CloseHandle(hSharedMutex);

	return 0;
} // ThreadFuncShared

// ------------------------------------------------------------------------------------------------
BOOL WriteFileMapping(HANDLE hFileMapping, LPCVOID lpData, DWORD cb)
{
	// ���������� ���� �� ������ � �������� ������������ �������� 
	PBYTE pbFile = (PBYTE)MapViewOfFile(hFileMapping, FILE_MAP_WRITE, 0, 0, cb);
	if (NULL == pbFile) return FALSE;

	// ���������� � ���������� ������ ������������ ������
	CopyMemory(pbFile, lpData, cb);

	// ������������� ���������� ������ �� ������� �� ����
	BOOL bRet = FlushViewOfFile(pbFile, cb);

	// ����������� ���������� ������
	UnmapViewOfFile(pbFile);

	return bRet;
} // WriteFileMapping

// ------------------------------------------------------------------------------------------------
BOOL ReadFileMapping(HANDLE hFileMapping, LPVOID lpData, DWORD cb)
{
	// ���������� ���� �� ������ � �������� ������������ ��������
	PBYTE pbFile = (PBYTE)MapViewOfFile(hFileMapping, FILE_MAP_READ, 0, 0, cb);
	if (NULL == pbFile) return FALSE;

	// ��������� �� ����������� ������� ���������� ������
	CopyMemory(lpData, pbFile, cb);

	// ����������� ���������� ������
	UnmapViewOfFile(pbFile);

	return TRUE;
} // ReadFileMapping
