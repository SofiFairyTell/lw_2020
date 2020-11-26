#pragma once
#include <Windows.h>
#include <WindowsX.h>
#include <tchar.h>
#include <strsafe.h>
#include <richedit.h> //why ?
#include "resource.h"
#include <atlconv.h>


// оконная процедура главного окна
LRESULT CALLBACK MainWindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
/*Обработчики сообщений WM_CREATE WM_DESTROY WM_SIZE WM_COMMAND */

BOOL OnCreate(HWND hwnd, LPCREATESTRUCT lpCreateStruct);
void OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify);

/*Работа с файлами инициализации*/
void LoadProfile(LPCTSTR lpFileName);//загрузка параметров
void SaveProfile(LPCTSTR lpFileName);//сохранение параметров

/*Асинхронные операции ввод/вывода*/
BOOL OpenFileAsync(HWND hwndCtl); //открытие и чтение
BOOL SaveFileAsync(HWND hwndCtl, BOOL fSaveAs = FALSE);// открытие и запись

/*Асинхронные операции чтения/записи*/
BOOL ReadAsync(HANDLE hFile, LPVOID lpBuffer, DWORD dwOffset, DWORD dwSize, LPOVERLAPPED lpOverlapped);//чтение
BOOL WriteAsync(HANDLE hFile, LPCVOID lpBuffer, DWORD dwOffset, DWORD dwSize, LPOVERLAPPED lpOverlapped);//запись

/*Функции ожидания и проверки ввод/вывода*/

BOOL FinishIo(LPOVERLAPPED lpOverlapped);
BOOL TryFinishIo(LPOVERLAPPED lpOverlapped);

// функция, которая вызывается в цикле обработки сообщений,
// пока в очереди нет сообщений
// для асинхронности
void OnIdle(HWND hwnd);

/*Переменные*/
POINT WindowPosition; // положение окна
SIZE WindowSize; // размер окна

TCHAR FileName[MAX_PATH] = TEXT(""); // имя редактируемого текстового файла
HANDLE hFile = INVALID_HANDLE_VALUE; // дескриптор редактируемого текстового файла

CHARFORMAT cf;//параметры символов
PARAFORMAT pf;//параметры абзацев
LOGFONT logFont; // параметры шрифта
HFONT hFont = NULL; // дескриптор шрифта

LPSTR lpBuffReWri = NULL; // указатель на буфер для чтения/записи текстового файла
OVERLAPPED ovlRead = { 0 }, ovlWrite = { 0 };// структуры управления асинхронным доступом к файлу