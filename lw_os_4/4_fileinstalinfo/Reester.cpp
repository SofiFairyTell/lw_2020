#include <Windows.h>
#include <tchar.h>
#include <stdio.h>
#include <locale.h>
#include <iostream>
#include <shlwapi.h>
#pragma comment(lib, "shlwapi.lib")


// функция для вывода списка установленных программ
void InstallAppList(REGSAM samDesired);
// функция для вывода списка программ автозапуска
void AutorunAppList(HKEY hRootKey, REGSAM samDesired);

void SubFind(LSTATUS retCode, DWORD CMVlen, HKEY hSubKey);

LSTATUS RegGetValueSZ(HKEY hKey, LPCTSTR lpValueName, LPTSTR lpszData, DWORD cch, LPDWORD lpcchNeeded);
// функция для определения, является ли операционная система 64-разрядной версией Windows
BOOL IsWin64();

// ----------------------------------------------------------------------------------------------
int _tmain()
{
	setlocale(LC_ALL, "");

	if (IsWin64()) // 64-разрядная версия Windows
	{
		std::cout<<"Список установленных программ:\n\n";

		// выведем список установленных программ (Win64)
		InstallAppList(KEY_ENUMERATE_SUB_KEYS | KEY_QUERY_VALUE | KEY_WOW64_64KEY);

		std::cout << "Список программ автозапуска HKLM\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run:\n\n";

		AutorunAppList(HKEY_LOCAL_MACHINE, KEY_ENUMERATE_SUB_KEYS | KEY_QUERY_VALUE | KEY_WOW64_64KEY);
		
		std::cout << "Список программ автозапуска HKCU\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run:\n\n";

		AutorunAppList(HKEY_CURRENT_USER, KEY_ENUMERATE_SUB_KEYS | KEY_QUERY_VALUE | KEY_WOW64_64KEY);
	} 

} 
// ----------------------------------------------------------------------------------------------
void InstallAppList(REGSAM AccessRights)
{
	HKEY hKey; // дескриптор ключа реестра

	// открываем ключ реестра
	LSTATUS retCode = RegOpenKeyEx(HKEY_LOCAL_MACHINE,
		TEXT("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall"), REG_OPTION_NON_VOLATILE, AccessRights, &hKey);

	if (ERROR_SUCCESS == retCode)
	{
		DWORD cSubKeys, cbMaxSubKey,i; // вложенные ключи,их максимальный размер,переменная для перехода по ним

		retCode = RegQueryInfoKey(hKey, NULL, NULL, NULL, &cSubKeys, &cbMaxSubKey, NULL, NULL, NULL, NULL, NULL, NULL);

		if ((ERROR_SUCCESS == retCode) && (cSubKeys > 0))
		{
			printf("\nNumber of values: %u\n", cSubKeys);
			// выделим память под буфер для имени вложенного ключа
			LPTSTR szSubKeyName = new TCHAR[cbMaxSubKey + 1];

			for ( i = 0;  i< cSubKeys; ++i)
			{
				HKEY hSubKey = NULL; // дескриптор вложенного ключа реестра
				// получим имя вложенного ключа с индексом dwIndex
				DWORD cchValue = cbMaxSubKey + 1;

				if (ERROR_SUCCESS == RegEnumKeyEx(hKey, i, szSubKeyName, &cchValue, NULL, NULL, NULL, NULL))
				{
					//начнем поиск по вложенным ключам 
					if (ERROR_SUCCESS == RegOpenKeyEx(hKey, szSubKeyName, REG_OPTION_NON_VOLATILE, AccessRights, &hSubKey))
					{
						DWORD cMaxValueLen = 0;//для определения максимальной длины значения
						retCode = RegQueryInfoKey(hSubKey, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, &cMaxValueLen, NULL, NULL);
						SubFind(retCode, cMaxValueLen, hSubKey);//демонстрация содержимого по вложенному ключу			
						RegCloseKey(hSubKey), hSubKey = NULL;// закрываем дескриптор вложенного ключа реестра
					}
				}		
			} 
			delete[] szSubKeyName;
		} 
	
		RegCloseKey(hKey), hKey = NULL;// закрываем дескриптор ключа реестра
	} 
}

void SubFind(LSTATUS retCode, DWORD CMVlen, HKEY hSubKey)
{
	if ((ERROR_SUCCESS == retCode) && (CMVlen > 0))
	{
		// выделим память под буфер для значений
		LPTSTR lpData = new TCHAR[CMVlen];

		// получим строковое значение
		retCode = RegGetValueSZ(hSubKey, TEXT("DisplayName"), lpData, CMVlen, NULL);

		if ((ERROR_SUCCESS == retCode) && (_T('\0') != lpData[0]))
		{
			_tprintf(TEXT("-----\n%s\n"), lpData);

			// получим строковое значение
			retCode = RegGetValueSZ(hSubKey, TEXT("Publisher"), lpData, CMVlen, NULL);

			if ((ERROR_SUCCESS == retCode) && (_T('\0') != lpData[0]))
			{
				_tprintf(TEXT("%s\n"), lpData);
			} 

			// получим строковое значение
			retCode = RegGetValueSZ(hSubKey, TEXT("DisplayVersion"), lpData, CMVlen, NULL);

			if ((ERROR_SUCCESS == retCode) && (_T('\0') != lpData[0]))
			{
				_tprintf(TEXT("%s\n"), lpData);
			} // if
		
			std::cout <<  "\n";
		} 

		delete[] lpData;// освободим выделенную память
	}
}


void AutorunAppList(HKEY hRootKey, REGSAM AccessRights)
{
	HKEY hKey = NULL; // дескриптор ключа рееста

	// открываем ключ реестра
	LSTATUS lStatus = RegOpenKeyEx(hRootKey, TEXT("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run"), REG_OPTION_NON_VOLATILE, AccessRights, &hKey);

	if (ERROR_SUCCESS == lStatus)
	{
		DWORD Val, MaxNameLen, MaxVallLen; // число параметров ключа, буфер для имен, буфер для значений

		// определим число параметров ключа и 
		// максимальный размер буферов для имён и значений параметров
		lStatus = RegQueryInfoKey(hKey, NULL, NULL, NULL, NULL, NULL, NULL, &Val, &MaxNameLen, &MaxVallLen, NULL, NULL);

		if ((ERROR_SUCCESS == lStatus) && (Val > 0))
		{
			// выделим память под буферы
			LPTSTR NameKey = new TCHAR[MaxNameLen + 1];
			LPBYTE lpData = new BYTE[MaxVallLen];

			for (DWORD i = 0; i < Val; ++i)
			{
					
				DWORD cchValueName = MaxNameLen + 1, dwType, cbData = MaxVallLen;
				lStatus = RegEnumValue(hKey, i, NameKey, &cchValueName, NULL, &dwType, lpData, &cbData);

				if ((ERROR_SUCCESS == lStatus) && (cchValueName > 0))
				{
					//reg_sz - тип данных. его значение UNICODE-строка
					if ((REG_SZ == dwType) || (REG_EXPAND_SZ == dwType))
					{
						_tprintf(TEXT("-----\n%s\n"), NameKey);//имя параметра
						_tprintf(TEXT("%s\n\n"), (LPTSTR)lpData);//значение параметра
					} 
				}
			}
					
			delete[] lpData;//освобождение памяти
			delete[] NameKey;//освобождение памяти
		} 

		RegCloseKey(hKey), hKey = NULL;		// закрываем дескриптор ключа реестра
	} 
} 

  //work wit regist

	LSTATUS RegGetValueSZ(HKEY hKey, LPCTSTR section, LPTSTR key, DWORD cch, LPDWORD lpcchNeeded)
	{
		DWORD dwType;
		LSTATUS res = RegQueryValueEx(hKey, section, NULL, &dwType, NULL, NULL);
			if (res != ERROR_SUCCESS)
			{
					res = ERROR_UNSUPPORTED_TYPE; // неверный тип данных
			}
			else
				if (ERROR_SUCCESS == res)
				{
					DWORD cb = cch * sizeof(TCHAR);
						// получаем значение параметра
					res = RegQueryValueEx(hKey, section, NULL, NULL, (LPBYTE)key, &cb);
					if (NULL != lpcchNeeded) *lpcchNeeded = cb / sizeof(TCHAR);
				}
		return res;
	} 


	BOOL IsWin64()
	{
#ifdef _WIN64

		return TRUE;

#else // _WIN64

		// (!) функция IsWow64Process поддерживается не во всех версиях Windows.

		typedef BOOL(WINAPI *LPFN_ISWOW64PROCESS) (HANDLE, PBOOL);
		LPFN_ISWOW64PROCESS fnIsWow64Process = (LPFN_ISWOW64PROCESS)GetProcAddress(GetModuleHandle(TEXT("kernel32")), "IsWow64Process");

		if (NULL != fnIsWow64Process)
		{
			BOOL bIsWow64;
			return (fnIsWow64Process(GetCurrentProcess(), &bIsWow64) != FALSE) && (bIsWow64 != FALSE);
		} // if

		return FALSE;

#endif // !_WIN64
	}
	