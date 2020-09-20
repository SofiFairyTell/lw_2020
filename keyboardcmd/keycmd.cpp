#include <Windows.h>
#include <stdio.h>
#include <iostream>
#include <conio.h>
#include "keyfoo.h"
#define _CRT_SECURE_NO_WARNINGS 

int main()
{
	setlocale(LC_ALL, "");
	char path[100] = { 0 }, file[100];
	while (true)
	{
		int c = _getch();
		std::cout << (char)c << ' ' << std::hex << (int)c << std::endl;
		switch (c)
		{
		case 's':
			system("explorer");
			break;

		case 'w':
			keyfoo::chdir();//открыть папку по указанию ее пути
			break;
	
		case 'f':
			keyfoo::filecreate();//создать файл с текстом
			break;
		case 'k':
			keyfoo::removefile();
			break;
		case 'e':
			{
			/*HWND hWnd;
			hWnd = FindWindow(NULL, _T("explorer.exe"));
			if (IsWindow(hWnd))
			SendMessage(hWnd, WM_SYSCOMMAND, SC_CLOSE, 0);*/
			//HANDLE explorer;
			//explorer = OpenProcess(PROCESS_ALL_ACCESS, false, 2120);
			//TerminateProcess(explorer, 1);
			//keyfoo::GetProcessList();
			HWND hwnd; 
			hwnd = FindWindow("Ћабораторные работы", NULL);//»щем проводник <br>      
			if (hwnd!=NULL)//ѕроводник открыт! 
				{ 
				SendMessage(hwnd,WM_DESTROY,NULL,NULL);
				SendMessage(hwnd,WM_NCDESTROY,NULL,NULL);           
				Sleep(100);//ѕодождем чуть-чуть :)     
				}      
			}
		break;
		}
			
		
		//case 'e':
		//	//  вот тут надо использовать WinApi дл€ закрыти€ окна;
		//	// не совсем уверен, но примерно так:
		//{
			
		//}
		//// а может можно и так
		//system("taskkill /f /im explorer.exe");
	}
}