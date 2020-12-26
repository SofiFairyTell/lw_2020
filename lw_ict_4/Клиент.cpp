// Клиент.cpp: определяет точку входа для консольного приложения.
//
/*"localhost"*///   <<----------------  в ком.строку!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//#include "stdafx.h"
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#pragma comment(lib,"ws2_32.lib")
#pragma comment(lib,"user32.lib")
#include "winsock2.h"
#include "string.h"
#include <iostream>

#include <fstream>
using namespace std;

int main(int argc, char* argv[])
{
	setlocale(LC_ALL, "Russian");
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2,0), &wsaData)!=NO_ERROR) { //  <<------ошибка
		cout<<"Увы..."<<endl;
		return -1;
	};
	//проверим параметры консоли
	if (argc!=3) { //  <<------ошибка
		cerr<<"Неверное число параметров консоли!"<<endl;
		return 1; //параметра не 3
	}
	for(int j=0; j<strlen(argv[2]); j++) { //  <<------ошибка
		if (argv[2][j]<'0' || argv[2][j]>'9') {
		cerr<<"Какое-то из чисел не целое или не число"<<endl;
		return 2;
		}
	}
	//создадим хост
	hostent* host;
	if (isalpha(*argv[1])) host = gethostbyname(argv[1]);
	else {
		long inadr=inet_addr(argv[1]);
		host = gethostbyaddr((char*)&inadr, 40, AF_INET);
	}
	if (!host) {//  <<------ошибка
		cout<<"Хост создать не удалось..."<<endl;
		WSACleanup();
		return -1;
	}
	SOCKET sclient;
	//связываемся с портами в указанном диапазоне
	u_short number=(u_short)atoi(argv[2]);
	{
		cout<<"Порт "<<number<<": ";
		sclient = socket(AF_INET,SOCK_STREAM, 0/*по умолч.TCP вроде*/);
		if (sclient==SOCKET_ERROR) {//  <<------ошибка
			cout<<"Сокет создать не удалось..."<<endl;
			return 1;
		}
		sockaddr_in inf_addr;
		memset(&inf_addr,0,sizeof(inf_addr));//очистим буфер на всякий случай
		inf_addr.sin_port=htons(number);
		inf_addr.sin_family=AF_INET;
		inf_addr.sin_addr = *((in_addr*)*host->h_addr_list)/* S_un.S_addr=inet_addr(inet_ntoa(*(in_addr*)(*ch))); */;
		if (connect(sclient, (sockaddr*)&inf_addr, sizeof(inf_addr))!= SOCKET_ERROR) {
			cout<<"Связался с сервером!!Уряяя!!"<<endl;

			//примм имя принимаемого файла
			char* nm, namme[100];
			int n=recv(sclient, (char*)&namme, sizeof(namme), 0);
			nm=new char[n+1];
			strncpy(nm, namme, n); nm[n]='\0';
			//открываем файл с принятым именем и принимаем его
			ofstream f(nm,ios::binary);
			char buffer[1024];
			int i=0;
			cout<<"Прием";
			while (1) {
				n=recv(sclient, buffer, sizeof(buffer), 0);
				cout<<"."; //1 килобайт-1 точка получается
				if (n<=0) break;
				f.write(buffer,n);//пишем в файл, сколько прочитали
			};
			f.close();
			cout<<"\nПринял файл "<<nm<<"!"<<endl;
			
			/*servent *serv=getservbyport( htons(number), 0); 
				if (serv) cout << "Имя сервера: " << serv->s_name << "  , протокол " << serv->s_proto << endl;
				else {
					wchar_t MessageError[1000];
					FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL, WSAGetLastError(), MAKELANGID(LANG_RUSSIAN, SUBLANG_DEFAULT), MessageError, 1000, NULL);
					wprintf(L"Ошибка: %s", MessageError);
				}*/
		}
		else {//  <<------ошибка
			cout<<"Connect не сработал..."<<endl;
			WSACleanup();
			return -1;
		}
	}
	closesocket(sclient);
	WSACleanup();
	system("pause");
	return 0;
}

