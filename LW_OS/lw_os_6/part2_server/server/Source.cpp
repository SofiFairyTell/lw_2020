//#include <Windows.h>
//#include <tchar.h>
//#include <strsafe.h>
//#include <process.h>
//
///*���������� ��� �����*/
//LPCTSTR service_name = TEXT("WinService");// ������ ��� ������
//LPCTSTR service_name_display = TEXT("������ ���������");// ������ ������������ ��� ������
//
///*�����������*/
//HANDLE hNamedPipe = INVALID_HANDLE_VALUE; // ���������� ������������� ������
//
//HANDLE hStopper = NULL; // ������� ��� ���������� ������ ������
//
//HANDLE hThread; // ����������� ���������� ������
//
//SECURITY_DESCRIPTOR sd; // ���������� ������������
//SECURITY_ATTRIBUTES sa; //�������� ������������
//
//BOOL RetRes;
//// ������� ������, ��� �������������� ������� �� ������
//unsigned __stdcall ThreadFuncPipe(void *lpParameter);
//
//// ------------------------------------------------------------------------------------------------
//BOOL OnSvcInit(DWORD dwArgc, LPTSTR *lpszArgv)
//{
//	/*������������� ��������� ������������*/
//	sa.nLength = sizeof(sa);
//
//	// �������������� ���������� ������������
//	RetRes = InitializeSecurityDescriptor(&sd, SECURITY_DESCRIPTOR_REVISION);
//
//	if (FALSE != RetRes)
//		//��������� ������ � ������������� ������ ���� �������������
//		RetRes = SetSecurityDescriptorDacl(&sd, TRUE, NULL, FALSE);
//	sa.lpSecurityDescriptor = &sd;
//
//	if (FALSE != RetRes)
//	{
//		// ������ �����
//		hNamedPipe = CreateNamedPipe(
//			TEXT("\\\\.\\pipe\\test_pipe"),//��� ������
//			PIPE_ACCESS_DUPLEX | FILE_FLAG_OVERLAPPED, // ������ �� ������ � ����� � ������ + �������������
//			PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT, //���������� �������� ���������
//			PIPE_UNLIMITED_INSTANCES, //������������ ��������� ����������� ������
//			0,		//������ ��������� ������ �� ���������
//			0,		//������ �������� ������ �� ���������
//			0,		//������� ������ ���� �����. 
//			&sa);	//������ ��� ���� �������������
//
//		if (hNamedPipe == INVALID_HANDLE_VALUE)
//		{
//			_tprintf(TEXT("> �� ������� ������� �����.\n"));
//			return FALSE;
//		}
//	}
//	return RetRes;
//}
////--������� OVERLAPPED
//void OnSvcStop(void)
//{
//	SetEvent(hStopper);// ��������� ������ ��������� �������
//}
//// ------------------------------------------------------------------------------------------------
//DWORD SvcMain(DWORD dwArgc, LPTSTR *lpszArgv)
//{
//	// ������ ������� ��� ���������� ������ ������
//	hStopper = CreateEventEx(NULL, NULL, CREATE_EVENT_MANUAL_RESET, EVENT_ALL_ACCESS);
//
//	// ������ �����, � ������� ����� ������������ ������� �� ������
//	hThread = (HANDLE)_beginthreadex(NULL, 0, ThreadFuncPipe, NULL, 0, NULL);
//
//	WaitForSingleObject(hThread, INFINITE);// ������� ���������� ���������� ������
//
//	CloseHandle(hStopper);// ��������� ���������� �������
//
//	return 0;
//}
//
//// ------------------------------------------------------------------------------------------------
//
//// ������ ���������
//constexpr LPCTSTR szStudents[] = {
//	TEXT("������ ������� ���������� ��-31"),
//	TEXT("�������� ��������� ��������� ��-31"),
//	TEXT("������� ����� ������������ ��-31"),
//	TEXT("�������� ���� ��������� ��-31"),
//	TEXT("�������� ���� ������������� ��-31"),
//	TEXT("������� ��������� ������������ ��-31"),
//	TEXT("������� ��������� �������� ��-31"),
//	TEXT("������ �������� ����������� ��-31"),
//	TEXT("����� ����� ������� ��-31"),
//	TEXT("������� ������� �������� ��-31"),
//	TEXT("��������� ��� ���������� ��-31"),
//	TEXT("�������� ��������� ��������� ��-31"),
//	TEXT("��������� ��������� ��������� ��-31"),
//	TEXT("�������� ������� ������������ ��-31"),
//	TEXT("���� ����� ��������� ��-31"),
//	TEXT("�������� ����� ����������� ��-31"),
//	TEXT("�������� ����� ������������ ��-31"),
//	TEXT("�������� ������ ���������� ��-31"),
//	TEXT("������� ������ ��������� ��-31"),
//	TEXT("������ ������ ��������� ��-31"),
//	TEXT("������� �������� ���������� ��-31"),
//	TEXT("��������� ����� ������������� ��-31"),
//	TEXT("��������� ������� ��������� ��-31"),
//	TEXT("�������� ����� �������� ��-31"),
//	TEXT("��������� ���� �������� ��-31"),
//	TEXT("��������� �������� ���������� ��-31"),
//	TEXT("����������� ����� �������� ��-31"),
//	TEXT("���������� ������� ������� ��-31"),
//	TEXT("�������� ������ ���������� ��-31"),
//	TEXT("�������� ����� ������������� ��-31"),
//	TEXT("��������� ������� �������� ��-31"),
//	TEXT("�������� ����� ������������ ��-31"),
//	TEXT("������ ������ ������������ ��-31")
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
//	/*����������*/
//	DWORD res;
//
//
//	// ������ ������� ��� ������������ �������� � �������
//	HANDLE hPipeEvent = CreateEventEx(NULL, NULL, 0, EVENT_ALL_ACCESS);
//
//	// ������ �������
//	HANDLE hEvents[2] = { hStopper, hPipeEvent };
//
//	for (;;)
//	{
//		// �������������� ��������� OVERLAPPED ...
//		OVERLAPPED oConnect = { 0 };
//		oConnect.hEvent = hPipeEvent;
//
//		// ������� �������-������ � �������� � ��� ����������
//		if (!ConnectNamedPipe(hNamedPipe, &oConnect))
//		{
//			_tprintf(TEXT("> ������: %d\n"), GetLastError());
//			CloseHandle(hNamedPipe);
//			return 0;
//		}
//
//
//		res = WaitForMultipleObjects(2, hEvents, FALSE, INFINITE);// ������� ���� �� ���� �������
//
//		if ((WAIT_OBJECT_0 == res) || (ERROR_SUCCESS != oConnect.Internal))
//		{
//			break; // (!) ������� �� �����
//		}
//
//		for (;;)
//		{
//			REQUEST Request; // ����� ���������
//			DWORD dwBytesRead;
//
//			//������ ��������� �� �������
//			RetRes = ReadFile(hNamedPipe, &Request, sizeof(Request), &dwBytesRead, NULL);
//
//			if (RetRes == FALSE) break; // (!) ������: ������� �� �����
//
//			TCHAR Response[100] = TEXT(""); // �����
//
//			if (Request.INDEX < _countof(szStudents))
//			{
//				StringCchCopy(Response, _countof(Response), szStudents[Request.INDEX]);
//			} // if
//
//			// ������ ������ � �������� �����
//			WriteFile(hNamedPipe, Response, sizeof(Response), &dwBytesRead, NULL);
//		} // for
//
//		DisconnectNamedPipe(hNamedPipe); // ��������� ����������
//	} // for
//
//	// ��������� ���������� �������
//	CloseHandle(hPipeEvent);
//
//	// ��������� ���������� ������
//	CloseHandle(hNamedPipe);
//
//	return 0;
//} // ThreadFuncPipe
//
