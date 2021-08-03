#include <iostream>
#include <WS2tcpip.h>

// Include the Winsock library (lib) file
#pragma comment (lib, "ws2_32.lib")

#pragma warning(disable: 4996)
// Saves us from typing std::cout << etc. etc. etc.
using namespace std;
#pragma pack(1)
struct SLP_msg
{
	int filelen;		 //длина сообщения
	int numberfrag;		//номер фрагмента
	WCHAR username[20]; //имя отправителя
	WCHAR text[10];		//текст сообщения
};
#pragma pack()
// Main entry point into the server
void main()
{
	////////////////////////////////////////////////////////////
	// INITIALIZE WINSOCK
	////////////////////////////////////////////////////////////

	// Structure to store the WinSock version. This is filled in
	// on the call to WSAStartup()
	WSADATA data;

	// To start WinSock, the required version must be passed to
	// WSAStartup(). This server is going to use WinSock version
	// 2 so I create a word that will store 2 and 2 in hex i.e.
	// 0x0202
	WORD version = MAKEWORD(2, 2);

	// Start WinSock
	int wsOk = WSAStartup(version, &data);
	if (wsOk != 0)
	{
		// Not ok! Get out quickly
		cout << "Can't start Winsock! " << wsOk;
		return;
	}

	////////////////////////////////////////////////////////////
	// SOCKET CREATION AND BINDING
	////////////////////////////////////////////////////////////

	// Create a socket, notice that it is a user datagram socket (UDP)
	SOCKET in = socket(AF_INET, SOCK_DGRAM, 0);

	// Create a server hint structure for the server
	sockaddr_in serverHint;
	serverHint.sin_addr.S_un.S_addr = ADDR_ANY; // Us any IP address available on the machine
	serverHint.sin_family = AF_INET; // Address format is IPv4
	serverHint.sin_port = htons(54000); // Convert from little to big endian

	WCHAR username[255] = L"";
	WCHAR text[255] = L"";

	// Try and bind the socket to the IP and port
	if (bind(in, (sockaddr*)&serverHint, sizeof(serverHint)) == SOCKET_ERROR)
	{
		cout << "Can't bind socket! " << WSAGetLastError() << endl;
		return;
	}

	////////////////////////////////////////////////////////////
	// MAIN LOOP SETUP AND ENTRY
	////////////////////////////////////////////////////////////

	sockaddr_in client; // Use to hold the client information (port / ip address)
	int clientLength = sizeof(client); // The size of the client information

	char buf[1024];
	SLP_msg msgt = { 0 };

	// Enter a loop
/*
	while (true)
	{
		ZeroMemory(&client, clientLength); // Clear the client structure
		ZeroMemory(buf, 1024); // Clear the receive buffer

		// Wait for message
		int bytesIn = recvfrom(in, buf, 1024, 0, (sockaddr*)&client, &clientLength);
		if (bytesIn == SOCKET_ERROR)
		{
			cout << "Error receiving from client " << WSAGetLastError() << endl;
			continue;
		}

		// Display message and client info
		char clientIp[256]; // Create enough space to convert the address byte array
		ZeroMemory(clientIp, 256); // to string of characters

		// Convert from byte array to chars
		inet_ntop(AF_INET, &client.sin_addr, clientIp, 256);

		// Display the message / who sent it
		cout << "Message recv from " << clientIp << " : " << buf << endl;
	}
	*/
	
	while (true)
	{
		ZeroMemory(&client, clientLength); // Clear the client structure
		ZeroMemory(buf, 1024); // Clear the receive buffer

		// Wait for message
		int bytesIn = recvfrom(in, (char*)&msgt, sizeof(msgt), NULL, (sockaddr*)&client, &clientLength);
		if (bytesIn == SOCKET_ERROR)
		{
			cout << "Error receiving from client " << WSAGetLastError() << endl;
			continue;
		}

		// Display message and client info
		char clientIp[256]; // Create enough space to convert the address byte array
		ZeroMemory(clientIp, 256); // to string of characters

		// Convert from byte array to chars
		inet_ntop(AF_INET, &client.sin_addr, clientIp, 256);

		// Display the message / who sent it
		//cout << "Message recv from " << clientIp << " : " << buf << endl;


		int len = msgt.filelen / 2;
		if (msgt.filelen % 2 == 0)
		{
			msgt.numberfrag--;
		}
		for (int i = 0; i < 255; i++)
		{
			if (i == msgt.numberfrag * 2)
			{
				for (int j = 0; j < 2; j++)
				{
					text[i + j] = msgt.text[j];
				}
			}
		}
		if (msgt.filelen % 5 == 0)
		{
			msgt.numberfrag++;
		}
		if (len == msgt.numberfrag)
		{
			wcscat(username, msgt.username);
			wcout << "Message recv from " << username << " : " << text << endl;
			
			wcscat(username, L": ");
			wcscat(username, text);
			wcout << "Message recv from " << clientIp << " : " << username << endl;
			
			memset(username, NULL, 255);
			memset(text,NULL,255);
		}
		else
		{
			wcscat(username, msgt.username);
			wcout << "Message recv from " << username << " : " << text << endl;
			//wcscat(username, L": ");
			//wcscat(username, text);
			//wcout << "Message recv from " << clientIp << " : " << username << endl;
			memset(username, NULL, 255);
			memset(text, NULL, 255);
		}
	}
	
	
	// Close socket
	closesocket(in);

	// Shutdown winsock
	WSACleanup();
}
