//#include <iostream>
//#include <WS2tcpip.h>
//
//#pragma comment (lib,"ws2_32.lib" )
//
//using namespace std;
//
//void main()
//{
//	//Startup Winsock
//	WSADATA data;
//	WORD version = MAKEWORD(2, 2);
//	int wsOK = WSAStartup(version, &data);
//	if (wsOK != 0)
//	{
//		cout << "Can't start Winsock !" << wsOK;
//		return;
//	}
//	//Bind socket to ip adress and port
//	SOCKET in = socket(AF_INET, SOCK_DGRAM, 0);
//	sockaddr_in ServerHint;
//	ServerHint.sin_addr.S_un.S_addr = ADDR_ANY;
//	ServerHint.sin_family = AF_INET;
//	ServerHint.sin_port = htons(54000); // Convert from little to big endian
//
//	if (bind(in, (sockaddr*)&ServerHint, sizeof(ServerHint)) == SOCKET_ERROR)
//	{
//		cout << "Can't bind socket!" << WSAGetLastError() << endl;
//		return;
//	}
//
//	sockaddr_in client; //Client 
//	int clientLength = sizeof(client);
//	ZeroMemory(&client, clientLength); //Client information
//
//
//	char buff[1024];
//	
//
//	//Enter a loop
//	while (true)
//	{
//		ZeroMemory(buff, 1024); //The buffer message gets recieved into. It will become 0 every time we recieve a message.
//		//Wait for message
//		int bytesIn = recvfrom(in, buff, 1024, 0, (sockaddr*)&client, &clientLength);
//		if (bytesIn == SOCKET_ERROR)
//		{
//			cout << "Error receiving from client!" << WSAGetLastError() << endl;
//			continue;
//		}
//		//Display message and client info
//		char clientIP[256];
//		ZeroMemory(clientIP, 256);
//		inet_ntop(AF_INET,&client.sin_addr,clientIP,256);
//		cout << "Message received from" << clientIP << ":" << buff << endl;
//	}
//	//Close socket
//	closesocket(in);
//	//Shutdown Windsock
//	WSACleanup();
//}