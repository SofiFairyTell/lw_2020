//// ������.cpp: ���������� ����� ����� ��� ����������� ����������.
////
//#define _WINSOCK_DEPRECATED_NO_WARNINGS
//#define _CRT_SECURE_NO_WARNINGS
//#pragma comment(lib,"ws2_32.lib")
//#pragma comment(lib,"user32.lib")
//#include "winsock2.h"
//#include <iostream>
//#include <fstream>
//#include <stdio.h>
//using namespace std;
//
//
//int main(int argc, char* argv[])
//{
//	setlocale(LC_ALL, "Russian");
//	WSADATA wsaData;
//	if (WSAStartup(MAKEWORD(2,0), &wsaData)!=NO_ERROR) { //  <<------������
//		cout<<"���..."<<endl;
//		return 1;
//	};
//
//	for(int j=0; j<strlen(argv[1]); j++) { //  <<------������
//		if (argv[1][j]<'0' || argv[1][j]>'9') {
//			cerr<<"�����-�� �� ����� �� ����� ��� �� �����"<<endl;
//			return 2;
//		}
//	}
//	//�������� ����� �������
//	SOCKET sserver;
//	//������������ ����� � ��������� ���������
//	u_short number=(u_short)atoi(argv[1]);
//	{
//		cout<<"���� "<<number<<": ";
//		sserver = socket(AF_INET,SOCK_STREAM, 0/*�� �����.TCP, �� ��� ����*/);
//		if (sserver==SOCKET_ERROR) {//  <<------������
//			cout<<"����� ������� �� �������..."<<endl;
//			wchar_t MessageError[1000];
//			FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL, WSAGetLastError(), MAKELANGID(LANG_RUSSIAN, SUBLANG_DEFAULT), MessageError, 1000, NULL);
//			wprintf(L"������: %s", MessageError);
//			return 1;
//		}
//		sockaddr_in inf_addr;
//		memset(&inf_addr,0,sizeof(inf_addr));//������� ����� �� ������ ������
//		inf_addr.sin_port=htons(number);
//		inf_addr.sin_family=AF_INET;
//		inf_addr.sin_addr.s_addr = INADDR_ANY;
//		if (bind(sserver, (sockaddr*)&inf_addr, sizeof(inf_addr))== SOCKET_ERROR) {
//			cout<<"�������� ������ ����� bind() � �������.����� �� �������..."<<endl;
//			wchar_t MessageError[1000];
//			FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL, WSAGetLastError(), MAKELANGID(LANG_RUSSIAN, SUBLANG_DEFAULT), MessageError, 1000, NULL);
//			wprintf(L"������: %s", MessageError);
//			return 1;
//		};
//		//� ������� ������� �����
//		if (listen(sserver, SOMAXCONN)== SOCKET_ERROR) {
//			cout<<"��������� ����� "<<number<<" �� �������..."<<endl;
//			wchar_t MessageError[1000];
//			FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL, WSAGetLastError(), MAKELANGID(LANG_RUSSIAN, SUBLANG_DEFAULT), MessageError, 1000, NULL);
//			wprintf(L"������: %s", MessageError);
//			return 1;
//		}
//		for(;;) {
//			sockaddr_in tmp_addr;
//			int len=sizeof(tmp_addr);
//			memset(&tmp_addr,0,sizeof(tmp_addr));
//			SOCKET tempsock=accept(sserver, (sockaddr*)&tmp_addr, &len); 
//
//			if (tempsock!=INVALID_SOCKET) {
//				cout<<"��������� ����!!!"<<endl;
//				cout<<"Ip �������: "<< inet_ntoa(tmp_addr.sin_addr)<<"\n"<<"����(�������): "<<htons(tmp_addr.sin_port)<<endl;
//
//				ifstream f(argv[2],ios::binary);
//				if (!f.is_open()) continue;
//				//��������� ��� �����
//				char ss[100];
//				strncpy(ss, argv[2], strlen(argv[2])+1);
//				send(tempsock, (char*)&ss, strlen(argv[2]), 0);
//				cout<<"\n������� ��� ������������� ����� "<<ss<<"..."<<endl;
//				//� ������� �������� ����
//				f.seekg(0, ios::end);//��������� � ����� �� �����
//				int length=f.tellg();//������� ��������� �� ����� � ���������(������ ����� ������)
//				f.seekg(0, ios::beg);//��������� �� ������ �����
//				char *buffer = new char[length];//������� �����
//				f.read(buffer,length);//������ � ����
//				send(tempsock, buffer, length, 0);//� ������� ��������
//				f.close();
//				cout<<"\n������� ���� "<<ss<<"!"<<endl;
//				closesocket(tempsock);//���� ��������� recv ��������� 0(��� �� SOCKET_ERROR)
//			}
//			else {
//				wchar_t MessageError[1000];
//				FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL, WSAGetLastError(), MAKELANGID(LANG_RUSSIAN, SUBLANG_DEFAULT), MessageError, 1000, NULL);
//				wprintf(L"������: %s", MessageError);
//			}
//		}
//	}
//	WSACleanup();
//	system("pause");
//	return 0;
//}
//
