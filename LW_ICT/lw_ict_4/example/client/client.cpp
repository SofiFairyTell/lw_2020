#pragma comment(lib, "ws2_32.lib")
#include <winsock2.h>
#include <iostream>

#pragma warning(disable: 4996)

int main() 
{
	//�������������
	WSAData wsaData;//��������� ������� ��������� ����� �������������

	WSAStartup(MAKEWORD(2, 2), &wsaData); //������������� ������������� socket
	
	SOCKET s;//����������� ������
	s = socket(AF_INET, SOCK_STREAM, 0);//�������� ������ AF_INET - ����� ���� IPv4, SOCK_STREAM - ���������� �������� TCP SOCK_DGRAM - ���������� UDP

	/*��������� ����������*/
	SOCKADDR_IN sa;
	memset(&sa, 0, sizeof(sa));
	sa.sin_family = AF_INET;//����� �������� ������ � �������� ���� IPv4
	sa.sin_port = htons(1234);//���� ������� ����� ������������
	
	char c;
	printf_s("s - server\nk - client\n");

	scanf_s("%c", &c);

	if ((c == 'k'))
	{
		sa.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");//����������� � ���������� ������
		connect(s, (SOCKADDR*)&sa, sizeof(sa));//����������� � �������

		/*�������� ������*/
		int mas[] = {1,2,3,4,5};
		send(s,(char *) mas, sizeof(mas), 0);

		char answer[25]; 
		recv(s, answer, sizeof(answer), 0);//�������� ����� �� �������
		printf_s(answer);
		Sleep(3000);
	}

	if ((c == 's'))
	{
		bind(s, (SOCKADDR*)&sa, sizeof(sa));

		listen(s, 100);//���� �������� ���������

		int buf[5];
		memset(buf, 0, sizeof(buf));


		SOCKET client_socket;
		SOCKADDR_IN client_addr;
		
		int client_addr_size = sizeof(client_addr);

		/*���� �����������*/
		while (client_socket = accept(s, (SOCKADDR*)&client_addr,&client_addr_size)) //������� accept ������ ������������� ������
		{
			if (client_socket == INVALID_SOCKET)
			{
				if (WSAEINTR == WSAGetLastError()) break;
			}
			else
			{
				printf_s("connect Ok\n");
				while (recv	(client_socket,(char*)buf,sizeof(buf),0)>0)//�������� ������ �� �������
				{
					for (int i = 0; i < 5; i++)
					{
						printf_s("%d\n", buf[i]);
					}
					char answer[25] = "it's answer from server\0";
					send(client_socket, answer, sizeof(answer), 0);//���������� ����� �������
				}
			}
		
		}
	}



	//WORD DLLVersion = MAKEWORD(2, 1);
	/*if (WSAStartup(DLLVersion, &wsaData) != 0) {
		std::cout << "Error" << std::endl;
		exit(1);
	}

	SOCKADDR_IN addr;
	int sizeofaddr = sizeof(addr);
	addr.sin_addr.s_addr = inet_addr("127.0.0.1");
	addr.sin_port = htons(1111);
	addr.sin_family = AF_INET;

	SOCKET Connection = socket(AF_INET, SOCK_STREAM, NULL);
	if (connect(Connection, (SOCKADDR*)&addr, sizeof(addr)) != 0) {
		std::cout << "Error: failed connect to server.\n";
		return 1;
	}
	std::cout << "Connected!\n";
	char msg[256];
	recv(Connection, msg, sizeof(msg), NULL);
	std::cout << msg << std::endl;

	system("pause");*/

	closesocket(s);
	return 0;
}