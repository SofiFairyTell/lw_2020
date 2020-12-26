// Сервер.cpp: определяет точку входа для консольного приложения.
//
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#pragma comment(lib,"ws2_32.lib")
#pragma comment(lib,"user32.lib")
#include "winsock2.h"
#include <iostream>
#include <fstream>
#include <stdio.h>
using namespace std;


int main(int argc, char* argv[])
{
	setlocale(LC_ALL, "Russian");
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2,0), &wsaData)!=NO_ERROR) { //  <<------ошибка
		cout<<"Увы..."<<endl;
		return 1;
	};

	for(int j=0; j<strlen(argv[1]); j++) { //  <<------ошибка
		if (argv[1][j]<'0' || argv[1][j]>'9') {
			cerr<<"Какое-то из чисел не целое или не число"<<endl;
			return 2;
		}
	}
	//создадим сокет сервера
	SOCKET sserver;
	//прослушиваем порты в указанном диапазоне
	u_short number=(u_short)atoi(argv[1]);
	{
		cout<<"Порт "<<number<<": ";
		sserver = socket(AF_INET,SOCK_STREAM, 0/*по умолч.TCP, то что надо*/);
		if (sserver==SOCKET_ERROR) {//  <<------ошибка
			cout<<"Сокет создать не удалось..."<<endl;
			wchar_t MessageError[1000];
			FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL, WSAGetLastError(), MAKELANGID(LANG_RUSSIAN, SUBLANG_DEFAULT), MessageError, 1000, NULL);
			wprintf(L"Ошибка: %s", MessageError);
			return 1;
		}
		sockaddr_in inf_addr;
		memset(&inf_addr,0,sizeof(inf_addr));//очистим буфер на всякий случай
		inf_addr.sin_port=htons(number);
		inf_addr.sin_family=AF_INET;
		inf_addr.sin_addr.s_addr = INADDR_ANY;
		if (bind(sserver, (sockaddr*)&inf_addr, sizeof(inf_addr))== SOCKET_ERROR) {
			cout<<"Привязка прилож через bind() к локальн.компу не удалась..."<<endl;
			wchar_t MessageError[1000];
			FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL, WSAGetLastError(), MAKELANGID(LANG_RUSSIAN, SUBLANG_DEFAULT), MessageError, 1000, NULL);
			wprintf(L"Ошибка: %s", MessageError);
			return 1;
		};
		//и наконец случаем порты
		if (listen(sserver, SOMAXCONN)== SOCKET_ERROR) {
			cout<<"прослушка порта "<<number<<" не удалась..."<<endl;
			wchar_t MessageError[1000];
			FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL, WSAGetLastError(), MAKELANGID(LANG_RUSSIAN, SUBLANG_DEFAULT), MessageError, 1000, NULL);
			wprintf(L"Ошибка: %s", MessageError);
			return 1;
		}
		for(;;) {
			sockaddr_in tmp_addr;
			int len=sizeof(tmp_addr);
			memset(&tmp_addr,0,sizeof(tmp_addr));
			SOCKET tempsock=accept(sserver, (sockaddr*)&tmp_addr, &len); 

			if (tempsock!=INVALID_SOCKET) {
				cout<<"Прослушал порт!!!"<<endl;
				cout<<"Ip клиента: "<< inet_ntoa(tmp_addr.sin_addr)<<"\n"<<"Порт(клиента): "<<htons(tmp_addr.sin_port)<<endl;

				ifstream f(argv[2],ios::binary);
				if (!f.is_open()) continue;
				//передадим имя файла
				char ss[100];
				strncpy(ss, argv[2], strlen(argv[2])+1);
				send(tempsock, (char*)&ss, strlen(argv[2]), 0);
				cout<<"\nПередал имя отправляемого файла "<<ss<<"..."<<endl;
				//и наконец передаем файл
				f.seekg(0, ios::end);//указатель в файле на конец
				int length=f.tellg();//столько прочитаем из файла и передадим(размер файла узнаем)
				f.seekg(0, ios::beg);//указатель на начало файла
				char *buffer = new char[length];//создаем буфер
				f.read(buffer,length);//читаем в него
				send(tempsock, buffer, length, 0);//и наконец передаем
				f.close();
				cout<<"\nПередал файл "<<ss<<"!"<<endl;
				closesocket(tempsock);//чтоб тамошнему recv передался 0(или же SOCKET_ERROR)
			}
			else {
				wchar_t MessageError[1000];
				FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL, WSAGetLastError(), MAKELANGID(LANG_RUSSIAN, SUBLANG_DEFAULT), MessageError, 1000, NULL);
				wprintf(L"Ошибка: %s", MessageError);
			}
		}
	}
	WSACleanup();
	system("pause");
	return 0;
}

