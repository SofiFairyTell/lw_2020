////// sockserver.cpp : Defines the entry point for the console application.
//////
////#include <stdio.h>
////#include <stdlib.h>
////#include <tchar.h>
////#include <Windows.h>
////#define WIN32_LEAN_AND_MEAN
//////#include "stdafx.h"
////#include <winsock2.h>
////#include <process.h>
////#pragma comment(lib,"ws2_32.lib" )
////#pragma warning(disable: 4996)
////unsigned int __stdcall  ServClient(void *data);
////
////int _tmain(int argc, _TCHAR* argv[])
////{
////
////	WSADATA wsaData;
////	int iResult;
////	sockaddr_in addr;
////	SOCKET sock,client;
////
////	addr.sin_family = AF_INET;
////	addr.sin_port = htons(2222);
////	addr.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
////
////	iResult = WSAStartup(MAKEWORD(2,2),&wsaData);//2.2 
////
////	if(iResult)
////	{
////		printf("WSA startup failed");
////		return 0;
////	}
////
////
////	sock = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
////	
////	if(sock == INVALID_SOCKET)
////	{
////		printf("Invalid socket");
////		return 0;
////	}
////
////	iResult = bind(sock,(sockaddr*)&addr,sizeof(sockaddr_in ));
////	
////	if(iResult)
////	{
////		
////		printf("bind failed %u",GetLastError());
////		
////		return 0;
////	}
////
////	iResult = listen(sock,SOMAXCONN);
////	
////	if(iResult)
////	{
////		
////		printf("iResult failed %u",GetLastError());
////		
////		return 0;
////	}
////
////	while(client = accept(sock,0,0))
////	{
////		if(client == INVALID_SOCKET)
////		{
////			printf("invalid client socket",GetLastError());
////			continue;
////		}	
////		_beginthreadex(0,0,ServClient,(void*)&client,0,0);
////		
////	}
////
////
////	return 0;
////}
////
////unsigned int __stdcall ServClient(void *data)
////{
////	SOCKET* client =(SOCKET*)data;
////    SOCKET Client = *client;
////	printf("Client connected\n");
////	
////	char chunk[200];
////	while(recv(Client,chunk,200,0))
////	{
////		printf("%s \t %d\n",chunk,GetCurrentThreadId());
////	}
////return 0;
////}
//
//#include <winsock2.h>
//#include <iostream>
//
//using namespace std;
//struct CLIENT_INFO
//{
//	SOCKET hClientSocket;
//	struct sockaddr_in clientAddr;
//};
//
//char szServerIPAddr[] = "10.67.67.1";     // Put here the IP address of the server
//int nServerPort = 5050;                    // The server port that will be used by
//											// clients to talk with the server
//
//bool InitWinSock2_0();
//BOOL WINAPI ClientThread(LPVOID lpData);
//
//int main()
//{
//	if (!InitWinSock2_0())
//	{
//		cout << "Unable to Initialize Windows Socket environment" << WSAGetLastError() << endl;
//		return -1;
//	}
//
//	SOCKET hServerSocket;
//
//	hServerSocket = socket(
//		AF_INET,        // The address family. AF_INET specifies TCP/IP
//		SOCK_STREAM,    // Protocol type. SOCK_STREM specified TCP
//		0               // Protoco Name. Should be 0 for AF_INET address family
//	);
//	if (hServerSocket == INVALID_SOCKET)
//	{
//		cout << "Unable to create Server socket" << endl;
//		// Cleanup the environment initialized by WSAStartup()
//		WSACleanup();
//		return -1;
//	}
//
//	// Create the structure describing various Server parameters
//	struct sockaddr_in serverAddr;
//
//	serverAddr.sin_family = AF_INET;     // The address family. MUST be AF_INET
//	serverAddr.sin_addr.s_addr = inet_addr(szServerIPAddr);
//	serverAddr.sin_port = htons(nServerPort);
//
//	// Bind the Server socket to the address & port
//	if (bind(hServerSocket, (struct sockaddr *) &serverAddr, sizeof(serverAddr)) == SOCKET_ERROR)
//	{
//		cout << "Unable to bind to " << szServerIPAddr << " port " << nServerPort << endl;
//		// Free the socket and cleanup the environment initialized by WSAStartup()
//		closesocket(hServerSocket);
//		WSACleanup();
//		return -1;
//	}
//
//	// Put the Server socket in listen state so that it can wait for client connections
//	if (listen(hServerSocket, SOMAXCONN) == SOCKET_ERROR)
//	{
//		cout << "Unable to put server in listen state" << endl;
//		// Free the socket and cleanup the environment initialized by WSAStartup()
//		closesocket(hServerSocket);
//		WSACleanup();
//		return -1;
//	}
//
//	// Start the infinite loop
//	while (true)
//	{
//		// As the socket is in listen mode there is a connection request pending.
//		// Calling accept( ) will succeed and return the socket for the request.
//		SOCKET hClientSocket;
//		struct sockaddr_in clientAddr;
//		int nSize = sizeof(clientAddr);
//
//		hClientSocket = accept(hServerSocket, (struct sockaddr *) &clientAddr, &nSize);
//		if (hClientSocket == INVALID_SOCKET)
//		{
//			cout << "accept( ) failed" << endl;
//		}
//		else
//		{
//			HANDLE hClientThread;
//			struct CLIENT_INFO clientInfo;
//			DWORD dwThreadId;
//
//			clientInfo.clientAddr = clientAddr;
//			clientInfo.hClientSocket = hClientSocket;
//
//			//inet_ntop(AF_INET, &client.sin_addr, clientIp, 256);
//
//			cout << "Client connected from " << inet_ntoa(clientAddr.sin_addr) << endl;
//
//			// Start the client thread
//			hClientThread = CreateThread(NULL, 0,
//				(LPTHREAD_START_ROUTINE)ClientThread,
//				(LPVOID)&clientInfo, 0, &dwThreadId);
//			if (hClientThread == NULL)
//			{
//				cout << "Unable to create client thread" << endl;
//			}
//			else
//			{
//				CloseHandle(hClientThread);
//			}
//		}
//	}
//
//	closesocket(hServerSocket);
//	WSACleanup();
//	return 0;
//}
//
//bool InitWinSock2_0()
//{
//	WSADATA wsaData;
//	WORD wVersion = MAKEWORD(2, 0);
//
//	if (!WSAStartup(wVersion, &wsaData))
//		return true;
//
//	return false;
//}
//
//BOOL WINAPI ClientThread(LPVOID lpData)
//{
//	CLIENT_INFO *pClientInfo = (CLIENT_INFO *)lpData;
//	char szBuffer[1024];
//	int nLength;
//
//	while (1)
//	{
//		nLength = recv(pClientInfo->hClientSocket, szBuffer, sizeof(szBuffer), 0);
//		if (nLength > 0)
//		{
//			szBuffer[nLength] = '\0';
//			cout << "Received " << szBuffer << " from " << inet_ntoa(pClientInfo->clientAddr.sin_addr) << endl;
//
//			// Convert the string to upper case and send it back, if its not QUIT
//			strupr(szBuffer);
//			if (strcmp(szBuffer, "QUIT") == 0)
//			{
//				closesocket(pClientInfo->hClientSocket);
//				return TRUE;
//			}
//			// send( ) may not be able to send the complete data in one go.
//			// So try sending the data in multiple requests
//			int nCntSend = 0;
//			char *pBuffer = szBuffer;
//
//			while ((nCntSend = send(pClientInfo->hClientSocket, pBuffer, nLength, 0) != nLength))
//			{
//				if (nCntSend == -1)
//				{
//					cout << "Error sending the data to " << inet_ntoa(pClientInfo->clientAddr.sin_addr) << endl;
//					break;
//				}
//				if (nCntSend == nLength)
//					break;
//
//				pBuffer += nCntSend;
//				nLength -= nCntSend;
//			}
//		}
//		else
//		{
//			cout << "Error reading the data from " << inet_ntoa(pClientInfo->clientAddr.sin_addr) << endl;
//		}
//	}
//
//	return TRUE;
//}