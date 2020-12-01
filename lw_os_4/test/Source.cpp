
#include <Windows.h>
#include <WindowsX.h>
#include <tchar.h>
#include <strsafe.h>
#include <richedit.h> //why ?

#include <atlconv.h>
#include <string>

int _tmain()
{
	HANDLE File = CreateFile(L"C:\\test\\new_file.txt", GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_NEW, NULL, NULL);
	const WORD BOM = 0xFEFF;
	DWORD BytesWritten = 0;
	WriteFile(File, &BOM, sizeof(BOM), &BytesWritten, NULL);
	const wchar_t* Text = L"text";
	WriteFile(File, Text, wcslen(Text) * sizeof(wchar_t), &BytesWritten, NULL);
	CloseHandle(File);

	File = CreateFile(L"C:\\test\\new_file.txt", GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, NULL, NULL);
	DWORD BytesRead = 0;
	WORD FileBOM = 0;
	ReadFile(File, &FileBOM, sizeof(FileBOM), &BytesRead, NULL);
	wchar_t FileText[4 + 1] = L"";
	if (FileBOM == BOM) {
		ReadFile(File, &FileText, (4 + 1) * sizeof(wchar_t), &BytesRead, NULL);
		FileText[4] = L'\0';
	}
	wprintf(L"%s", FileText);
	CloseHandle(File);
}