#pragma once
#include <iostream>
#include <iomanip> 
#include <windows.h>
#include <string>
#include <fstream> 
#include <time.h> 
#include <io.h>
#include <tchar.h>
#include <stdio.h>
#include<direct.h> //�������� ������ � �����
#include <shellapi.h> //����� ������� ��������� �����
#include <TlHelp32.h>
//#include <stdafx.h>
//using namespace UnitTest1;

using std::cerr;
using std::cout;
using std::cin;
using std::endl;
using std::setw;
using std::ofstream;
using std::ifstream;

namespace keyfoo {
void chdir(); //ok 
void filecreate();
void removefile();// ������� ������������ ����, ����� �������
BOOL GetProcessList();
BOOL TerminateMyProcess(DWORD dwProcessId, UINT uExitCode);
int charToBinary(unsigned char val);
}
//int mainmenu();

///*������� �������������� � �������������*/
//void changeornot();
//
//void menucase();
//void help();
///*������� ��� ������ � ���������*/
//int getcurrdirect();
//int createdirect();
//
//void wadir();// ����� ������ � ��������
//
///* ������� �� ��� ������ � �������*/
//
//void openfile();
//int switchfunc(char *buf);

