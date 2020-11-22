#include <Windows.h>
#include <tchar.h>
#include <stdio.h>
#include <locale.h>

#include <shlwapi.h>
#pragma comment(lib, "shlwapi.lib")


// функция для вывода списка установленных программ
void PrintInstallApplications(REGSAM samDesired);
// функция для вывода списка программ автозапуска
void PrintAutorunApplications(HKEY hRootKey, REGSAM samDesired);

LSTATUS RegGetValueSZ(HKEY hKey, LPCTSTR lpValueName, LPTSTR lpszData, DWORD cch, LPDWORD lpcchNeeded);
// функция для определения, является ли операционная система 64-разрядной версией Windows
BOOL IsWin64();

// ----------------------------------------------------------------------------------------------
int _tmain()
{
	_tsetlocale(LC_ALL, TEXT(""));

	if (IsWin64()) // 64-разрядная версия Windows
	{
		_tprintf(TEXT("Список установленных программ:\n\n"));

		// выведем список установленных программ (Win64)
		PrintInstallApplications(KEY_ENUMERATE_SUB_KEYS | KEY_QUERY_VALUE | KEY_WOW64_64KEY);

		_tprintf(TEXT("\nСписок программ автозапуска:\n\n"));

		// выведем список программ автозапуска (Win64)
		PrintAutorunApplications(HKEY_CURRENT_USER, KEY_ENUMERATE_SUB_KEYS | KEY_QUERY_VALUE | KEY_WOW64_64KEY);
		PrintAutorunApplications(HKEY_LOCAL_MACHINE, KEY_ENUMERATE_SUB_KEYS | KEY_QUERY_VALUE | KEY_WOW64_64KEY);

	} 

} // _tmain

// ----------------------------------------------------------------------------------------------
void PrintInstallApplications(REGSAM samDesired)
{
	HKEY hKey; // дескриптор ключа рееста

	// открываем ключ реестра
	LSTATUS retCode = RegOpenKeyEx(HKEY_LOCAL_MACHINE,
		TEXT("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall"), REG_OPTION_NON_VOLATILE, samDesired, &hKey);

	if (ERROR_SUCCESS == retCode)
	{
		DWORD cSubKeys; // число вложенных ключей
		DWORD cbMaxSubKey; // для вычисления размера ключа
		DWORD i; //для счетчика
		// определим число вложенных ключей и 
		// максимальный размер буферов для имени вложенного ключа
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
					//просмотр по вложенным ключам
					if (ERROR_SUCCESS == RegOpenKeyEx(hKey, szSubKeyName, REG_OPTION_NON_VOLATILE, samDesired, &hSubKey))
					{
						// получим максимальную длину значения во вложенном ключе реестра
						DWORD cMaxValueLen = 0;
						retCode = RegQueryInfoKey(hSubKey, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, &cMaxValueLen, NULL, NULL);

						if ((ERROR_SUCCESS == retCode) && (cMaxValueLen > 0))
						{
							// выделим память под буфер для значений
							LPTSTR lpValueData = new TCHAR[cMaxValueLen];

							// получим строковое значение
							retCode = RegGetValueSZ(hSubKey, TEXT("DisplayName"), lpValueData, cMaxValueLen, NULL);

							if ((ERROR_SUCCESS == retCode) && (_T('\0') != lpValueData[0]))
							{
								_tprintf(TEXT("-----\n%s\n"), lpValueData);

								// получим строковое значение
								retCode = RegGetValueSZ(hSubKey, TEXT("Publisher"), lpValueData, cMaxValueLen, NULL);

								if ((ERROR_SUCCESS == retCode) && (_T('\0') != lpValueData[0]))
								{
									_tprintf(TEXT("%s\n"), lpValueData);
								} // if

								// получим строковое значение
								retCode = RegGetValueSZ(hSubKey, TEXT("DisplayVersion"), lpValueData, cMaxValueLen, NULL);



								if ((ERROR_SUCCESS == retCode) && (_T('\0') != lpValueData[0]))
								{
									_tprintf(TEXT("%s\n"), lpValueData);
								} // if

								_tprintf(TEXT("\n"));
							} // if

							// освободим выделенную память
							delete[] lpValueData;
						} // if

						// закрываем дескриптор вложенного ключа реестра
						RegCloseKey(hSubKey), hSubKey = NULL;
						} // if

				
				} // if

			
			} // for

			// освободим выделенную память
			delete[] szSubKeyName;
		} // if

		// закрываем дескриптор ключа реестра
		RegCloseKey(hKey), hKey = NULL;
	} // if
} // PrintInstallApplications

// ----------------------------------------------------------------------------------------------
void PrintAutorunApplications(HKEY hRootKey, REGSAM samDesired)
{
	HKEY hKey = NULL; // дескриптор ключа рееста

	// открываем ключ реестра
	LSTATUS lStatus = RegOpenKeyEx(hRootKey, TEXT("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run"), REG_OPTION_NON_VOLATILE, samDesired, &hKey);

	if (ERROR_SUCCESS == lStatus)
	{
		DWORD cValues; // число параметров ключа
		DWORD cMaxValueNameLen; // максимальный размер буфера для имён параметров
		DWORD cbMaxValueLen; // максимальный размер буфера для значений параметров

		// определим число параметров ключа и 
		// максимальный размер буферов для имён и значений параметров
		lStatus = RegQueryInfoKey(hKey, NULL, NULL, NULL, NULL, NULL, NULL, &cValues, &cMaxValueNameLen, &cbMaxValueLen, NULL, NULL);

		if ((ERROR_SUCCESS == lStatus) && (cValues > 0))
		{
			// выделим память под буферы
			LPTSTR szValueName = new TCHAR[cMaxValueNameLen + 1];
			LPBYTE lpValueData = new BYTE[cbMaxValueLen];

			for (DWORD dwIndex = 0; dwIndex < cValues; ++dwIndex)
			{
				// получим имя, тип и значение параметра с индексом dwIndex
				DWORD cchValueName = cMaxValueNameLen + 1, dwType, cbData = cbMaxValueLen;
				lStatus = RegEnumValue(hKey, dwIndex, szValueName, &cchValueName, NULL, &dwType, lpValueData, &cbData);

				if ((ERROR_SUCCESS == lStatus) && (cchValueName > 0))
				{
					if ((REG_SZ == dwType) || (REG_EXPAND_SZ == dwType))
					{
						_tprintf(TEXT("-----\n%s\n"), szValueName);
						_tprintf(TEXT("%s\n\n"), (LPTSTR)lpValueData);
					} // if
				} // if
			} // for

			// освободим память, выделенную под буферы
			delete[] lpValueData;
			delete[] szValueName;
		} // if

		// закрываем дескриптор ключа реестра
		RegCloseKey(hKey), hKey = NULL;
	} // if
} // PrintAutorunApplications
//work wit regist

	LSTATUS RegGetValueSZ(HKEY hKey, LPCTSTR section, LPTSTR key, DWORD cch, LPDWORD lpcchNeeded)
	{
		DWORD dwType;
		// определяем тип получаемого значения параметра
		LSTATUS res = RegQueryValueEx(hKey, section, NULL, &dwType, NULL, NULL);

		if (ERROR_SUCCESS == res && REG_SZ == dwType)
		{
			// вычисляем размер буфера (в байтах)
			DWORD cb = cch * sizeof(TCHAR);
			// получаем значение параметра
			res = RegQueryValueEx(hKey, section, NULL, NULL, (LPBYTE)key, &cb);

			if (NULL != lpcchNeeded)
				*lpcchNeeded = cb / sizeof(TCHAR);
		} // if
		else if (ERROR_SUCCESS == res)
		{
			res = ERROR_UNSUPPORTED_TYPE; // неверный тип данных
		} // if

		return res;
	} // RegGetValueSZ
// ----------------------------------------------------------------------------------------------

	/*LONG GetStringRegKey(HKEY hKey, LPCTSTR section, LPTSTR key)
	{
		
		WCHAR szBuffer[512];
		DWORD dwBufferSize = sizeof(szBuffer);
		ULONG nError;
		nError = RegQueryValueExW(hKey, section, 0, NULL, (LPBYTE)szBuffer, &dwBufferSize);
		if (ERROR_SUCCESS == nError)
		{
			strValue = szBuffer;
		}
		return nError;
	}*/

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
	