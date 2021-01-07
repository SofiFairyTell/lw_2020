#include <winsock2.h>
#include <Windows.h>
#include <WindowsX.h>
#include <tchar.h>
#include <locale.h>
#include <CommCtrl.h>
#include <Strsafe.h>
#include <string.h>
#include <process.h>
#include <iostream>

#pragma comment ( lib, "ws2_32.lib" )
using namespace std;
#pragma warning(disable: 4996);
#define SHUTDOWN(sock) if(sock != NULL) {shutdown(sock, SD_BOTH); closesocket(sock); sock = NULL;}

struct hostent *hst;
SRWLOCK SrwLock;
WSADATA wsaData;
SOCKET  s;
HANDLE hThread;
HWND hWnd;
HANDLE handleOfClients[20];

sockaddr_in SIN = { 0 };

#pragma pack(push,1)
struct Package
{
	CHAR recipients[4096];
	CHAR message[1024];
};
#pragma pack(pop)

char *host;
int nclients = 0;
Package msg;

SOCKET SocketClients[1024];
char *BaseNameClients[1024];
void SendHostnames(char string[]);
int SendMessage(SOCKET s
	, char* buf, size_t length);
int ReceiveMessage(SOCKET s, char* buf, size_t length);

unsigned int WINAPI ClientTread(LPVOID client_socket)
{
	SOCKET my_sock;
	my_sock = ((SOCKET *)client_socket)[0];
	char *NameClients;
	for (;;)
	{
		int n = ReceiveMessage(my_sock, (char *)&msg, sizeof(msg));
		if (n != SOCKET_ERROR)
		{
			if (strcmp(msg.recipients, "1") == 0)
			{
				int size = 0;
				for (int i = 0; i < nclients; ++i)
				{
					size = size + strlen(BaseNameClients[i]) + 1;
				}
				NameClients = new char[size + 1];
				strcpy(NameClients, "");
				for (int i = 0; i < nclients; ++i)
				{
					char c = strlen(BaseNameClients[i]);
					StringCchPrintf(NameClients, size + 1, "%s%c%s", NameClients, c, BaseNameClients[i]);
				}
				StringCchCopy(msg.message, 2, "");
				StringCchCopy(msg.recipients, strlen(NameClients) + 1, NameClients);
				StringCchCat(msg.recipients, 1, '\0');
				SendMessage(my_sock, (char *)&msg, sizeof(msg));
				delete[] NameClients;
			}
			else
			{
				if (strcmp(msg.recipients, "0") == 0)
				{
					break;
				}
				SendHostnames(msg.recipients);
			}
		}
		else break;
	}
	int currentNameLen = 0;
	getsockname(my_sock, NULL, &currentNameLen);
	char *CurrentName = new char[currentNameLen + 1];
	int err = getsockname(my_sock, (sockaddr*)CurrentName, &currentNameLen);
	if (err != SOCKET_ERROR)
	{
		AcquireSRWLockExclusive(&SrwLock);
		for (int i = 0; i < nclients; ++i)
		{
			if (strcpy(CurrentName, BaseNameClients[i]) == 0)
			{
				CloseHandle(handleOfClients[i]);
				for (int j = i; j < nclients; ++j)
				{
					BaseNameClients[j] = BaseNameClients[j + 1];
					SocketClients[j] = SocketClients[j + 1];
					handleOfClients[j] = handleOfClients[j + 1];
				}
			}
		}
		ReleaseSRWLockShared(&SrwLock);
	}
	delete[]BaseNameClients[nclients];
	nclients--;
	SHUTDOWN(my_sock);
	return 0;
}

int _tmain()
{
	setlocale(LC_ALL, "Russian");
	int err = WSAStartup(MAKEWORD(2, 2), &wsaData);
	s = socket(AF_INET, SOCK_STREAM, 0);
	if (s == INVALID_SOCKET)
	{
		printf("Socket error: %d\n", WSAGetLastError());
		WSACleanup();
	}
	SIN.sin_family = AF_INET;
	SIN.sin_port = htons(5150);
	SIN.sin_addr.s_addr = INADDR_ANY;

	err = bind(s, (sockaddr *)&SIN, sizeof(SIN));

	if (SOCKET_ERROR != err)
	{
		err = listen(s, 5);
	}

	if (SOCKET_ERROR != err)
	{
		for (;;)
		{
			sockaddr_in client_addr;
			int client_addr_size = sizeof(client_addr);
			SOCKET client_socket = accept(s, (sockaddr *)&client_addr, &client_addr_size);
			if (INVALID_SOCKET == client_socket)
			{
				nclients++;
				if (WSAEINTR == WSAGetLastError()) break;
			}
			else
			{
				SocketClients[nclients] = client_socket;
				hst = gethostbyaddr((char *)&client_addr.sin_addr.s_addr, 4, AF_INET);
				printf("%s(%s) подключился\n",
					(hst) ? hst->h_name : "", inet_ntoa(client_addr.sin_addr));
				BaseNameClients[nclients] = new char[strlen(hst->h_name)];
				strcpy(BaseNameClients[nclients], hst->h_name);
				nclients++;
				InitializeSRWLock(&SrwLock);
				handleOfClients[nclients] = (HANDLE)_beginthreadex(NULL, NULL, ClientTread, &client_socket, NULL, NULL);
			}
		}
		WaitForMultipleObjects(nclients, handleOfClients, TRUE, INFINITE);
		SHUTDOWN(s);
	}
}

int ReceiveMessage(SOCKET s, char* buf, size_t length)
{
	int all = length;
	int received = 0;
	while (all > 0)
	{
		received = recv(s, buf, all, 0);
		if (received < 0) return SOCKET_ERROR;
		buf += received;
		all -= received;
	}
	return length;
}

int SendMessage(SOCKET s, char* buf, size_t length)
{
	int all = length;
	int Sent = 0;
	while (all > 0)
	{
		Sent = send(s, buf, all, 0);
		if (Sent < 0) return SOCKET_ERROR;
		buf += Sent;
		all -= Sent;
	}
	return length;
}

void SendHostnames(char string[])
{
	unsigned int i = 0;
	unsigned int numOfClients = 0;
	do
	{
		int n = string[i];
		char *clientName = new char[n + 1];
		for (int k = 0; k < n; k++)
		{
			clientName[k] = string[i + 1 + k];
		}
		clientName[n] = '\0';
		cout << clientName << endl;
		for (int j = 0; j < nclients; ++j)
		{
			if (strcmp(clientName, BaseNameClients[j]) == 0)
			{
				int n = SendMessage(SocketClients[j], (char*)&msg, sizeof(msg));
			}
		}
		numOfClients++;
		delete[]clientName;
		i += n + 1;
	} while (i < 4096 && string[i] != '\0');
	cout << numOfClients << endl;
}
