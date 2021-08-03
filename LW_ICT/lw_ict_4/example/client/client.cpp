#pragma comment(lib, "ws2_32.lib")
#include <winsock2.h>
#include <iostream>

#pragma warning(disable: 4996)

int main() 
{
	//Инициализация
	WSAData wsaData;//структура которую заполняем после инициализации

	WSAStartup(MAKEWORD(2, 2), &wsaData); //инициализация использования socket
	
	SOCKET s;//дексприптор сокета
	s = socket(AF_INET, SOCK_STREAM, 0);//создание сокета AF_INET - адрес типа IPv4, SOCK_STREAM - используем протокол TCP SOCK_DGRAM - используем UDP

	/*Настройка соединения*/
	SOCKADDR_IN sa;
	memset(&sa, 0, sizeof(sa));
	sa.sin_family = AF_INET;//будем работать только с адресами типа IPv4
	sa.sin_port = htons(1234);//порт который будем использовать
	
	char c;
	printf_s("s - server\nk - client\n");

	scanf_s("%c", &c);

	if ((c == 'k'))
	{
		sa.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");//подключение к локальному адресу
		connect(s, (SOCKADDR*)&sa, sizeof(sa));//подключение к серверу

		/*передача данных*/
		int mas[] = {1,2,3,4,5};
		send(s,(char *) mas, sizeof(mas), 0);

		char answer[25]; 
		recv(s, answer, sizeof(answer), 0);//получаем ответ от сервера
		printf_s(answer);
		Sleep(3000);
	}

	if ((c == 's'))
	{
		bind(s, (SOCKADDR*)&sa, sizeof(sa));

		listen(s, 100);//ждем входящие сообщения

		int buf[5];
		memset(buf, 0, sizeof(buf));


		SOCKET client_socket;
		SOCKADDR_IN client_addr;
		
		int client_addr_size = sizeof(client_addr);

		/*Ждем подключения*/
		while (client_socket = accept(s, (SOCKADDR*)&client_addr,&client_addr_size)) //функция accept вернет идентификатор сокета
		{
			if (client_socket == INVALID_SOCKET)
			{
				if (WSAEINTR == WSAGetLastError()) break;
			}
			else
			{
				printf_s("connect Ok\n");
				while (recv	(client_socket,(char*)buf,sizeof(buf),0)>0)//получаем данные от клиента
				{
					for (int i = 0; i < 5; i++)
					{
						printf_s("%d\n", buf[i]);
					}
					char answer[25] = "it's answer from server\0";
					send(client_socket, answer, sizeof(answer), 0);//отправляем ответ клиенту
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