#include <Windows.h>
#include <stdio.h>
#include <iostream>
#include <conio.h>
#include "keyfoo.h"
#define _CRT_SECURE_NO_WARNINGS 
#define s 1110011 

int main()
{
	setlocale(LC_ALL, "");
	char path[100] = { 0 }, file[100];
	while (true)
	{
		int c = _getch();
		//std::cout << (char)c<< keyfoo::charToBinary(c) << ' ';
		std::cout << keyfoo::charToBinary(c) << ' ';
		switch (keyfoo::charToBinary(c))
		{
		case 1110011:
			system("explorer");//������� ���������
			break;

		case 1110111:
			keyfoo::chdir();//������� ����� �� �������� �� ����
			break;

		case 1100110:
			keyfoo::filecreate();//������� ���� � �������
			break;
		case 1101011:
			keyfoo::removefile();//������� ����� �� �� ������ � ���� � ������� �����
			break;
		case 1100101:
		{
			//keyfoo::GetProcessList();
			HWND hwnd;
			hwnd = FindWindow("CabinetWClass", NULL);//���� ��������� <br>      
			//std::cout << hwnd;
			if (hwnd != NULL)//��������� ������! 
			{
				//SendMessage(hwnd,WM_DESTROY,NULL,NULL);
				//SendMessage(hwnd,WM_NCDESTROY,NULL,NULL);
				SendMessage(hwnd, WM_SYSCOMMAND, SC_CLOSE, 0);
			}
		}
		break;
		}
	}
}