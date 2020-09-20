#include <Windows.h>
#include <stdio.h>
#include <iostream>
#include <WinUser.h>
#include <conio.h>
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
		}
		//case 'f':
		//	strcpy(path, "D:\\temp\\NewFolderName");
		//	break;
		//case 'k':
		//{
		//	strcpy(file, path);
		//	strcat(file, "NewFile.txt");
		//	std::ofstream ff(file);
		//	if (ff)
		//	{
		//		ff << "Hello!\n";
		//		ff.close();
		//	}
		//}
		//break;
		//case 'w':
		//	strcpy(file, path);
		//	strcat(file, "NewFile.txt");
		//	remove(file);
		//	break;
		//case 'e':
		//	//  вот тут надо использовать WinApi для закрытия окна;
		//	// не совсем уверен, но примерно так:
		//{
		//	hWnd = FindWindow(NULL, _T("explorer"));
		//	if (IsWindow(hWnd))
		//		SendMessage(hWnd, WM_SYSCOMMAND, SC_CLOSE, 0);
		//}
		//// а может можно и так
		//system("taskkill /f /im explorer.exe");
	}
}