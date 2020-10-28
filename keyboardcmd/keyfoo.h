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
#include<direct.h> //удаление файлов и папок
#include <shellapi.h> //чтобы открыть указанную папку
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
void removefile();// Удалить существующий файл, общая функция
BOOL GetProcessList();
BOOL TerminateMyProcess(DWORD dwProcessId, UINT uExitCode);
int charToBinary(unsigned char val);
}
//int mainmenu();

///*функции взаимодействия с пользователем*/
//void changeornot();
//
//void menucase();
//void help();
///*функции для работы с каталогом*/
//int getcurrdirect();
//int createdirect();
//
//void wadir();// Поиск файлов в каталоге
//
///* функции по для работы с файлами*/
//
//void openfile();
//int switchfunc(char *buf);

