#include <iostream>
#include <WS2tcpip.h>
#include <Windows.h> 
#include <Windowsx.h>
#include <CommCtrl.h> 
#include <tchar.h> 
#include <Psapi.h> 
#include <strsafe.h> 
#include <process.h>
#include <string>
#include <vector>
#include <stdlib.h>

#pragma comment (lib, "ws2_32.lib")// Include the Winsock library (lib) file
#pragma warning(disable: 4996)
// Saves us from typing std::cout << etc. etc. etc.
using namespace std;

SOCKET out = NULL;
#pragma pack(push, 1)
struct SLP_msg
{
	int filelen;		 //длина сообщения
	int numberfrag;		//номер фрагмента
	WCHAR username[20]; //имя отправителя
	WCHAR text[10];		//текст сообщения
};
#pragma pack(pop)

void wmain(int argc, wchar_t* argv[]) // We can pass in a command line option!! 
{
	
	#pragma region Start Socket
		WSADATA data;
		WORD version = MAKEWORD(2, 2);
		// Start WinSock
		int wsOk = WSAStartup(version, &data);
		if (wsOk != 0)
		{
			cout << "Can't start Winsock! " << wsOk;
			return;
		}
	#pragma endregion 
	// Create a hint structure for the server
	sockaddr_in server = { 0 };
	server.sin_family = AF_INET; // AF_INET = IPv4 addresses
	server.sin_port = htons(7581); // Little to big endian conversion
	server.sin_addr = htonl(INADDR_ANY);//0.0.0.0

	err = bind()
	//inet_pton(AF_INET, "127.0.0.1", &server.sin_addr); // Convert from string to byte array

	// Socket creation, note that the socket type is datagram
	out = socket(AF_INET, SOCK_DGRAM, 0);

	// Write out to that socket
	if (argv[0])
	{
		DWORD ERR = GetLastError();
	}
	else
	{
		DWORD ERR = GetLastError();
	}

	wstring s(argv[1]); //username
	wstring text_s(argv[2]); //сообщение


	WCHAR usernames[20] = L"";
	//WCHAR text[255] = L"";
	
	wchar_t *text = new wchar_t[text_s.size()];
	wchar_t *username = new wchar_t[s.size()];
	
	wcscpy(text, text_s.c_str());
	wcscpy(username, s.c_str());

	SLP_msg msg; //буфер для отправки сообщений
	memset(msg.text, NULL, 10);
	/*const CHAR * username = s.c_str();
	const CHAR * text_c = text_s.c_str();*/

	StringCchPrintf(msg.username, 20, text); //передача имени в структуру
	StringCchPrintf(msg.text, 20,username); //передача текста в структуру

	int n = 0;
	int lentext = wcslen(text);//вычисление длины текста 
	msg.filelen = lentext;
	int num; //номер пакета?? количество??
	int fragment = (lentext / 5); //длина 10 макс, значит по 5 байт?

	if (lentext % 5 == 0)
	{
		num = 1;
	}
	else num = 0;
	int sendOk;
	for (int i = 0; i < lentext; i += 5)
	{
		WCHAR frag[5] = L"";
		for (int j = 0; j < 5; j++)
		{
			frag[j] = text[j + i];
		}
		msg.numberfrag = num;
		num++;
		StringCchPrintf(msg.text, 6, frag);
		sendOk = sendto(out, (const char*)&msg, sizeof(msg),NULL, (struct sockaddr*)&server, sizeof(server));
		ZeroMemory(msg.text, 10);
	}

	//int sendOk = sendto(out, s.c_str(), s.size() + 1, 0, (sockaddr*)&server, sizeof(server));

	if (sendOk == SOCKET_ERROR)
	{
		cout << "That didn't work! " << WSAGetLastError() << endl;
	}

	// Close the socket
	closesocket(out);

	// Close down Winsock
	WSACleanup();
}