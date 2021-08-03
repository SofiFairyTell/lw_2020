#include <windows.h>
#include <stdio.h>
#include <tchar.h>
#include <cstdio>
#include <iostream>
#include <vector>
#include <string>

#define MAX_KEY_LENGTH 255
#define MAX_VALUE_NAME 16383

typedef std::basic_string<TCHAR> tstring;

// RecursiveDepth:
// < 0: recurse the whole tree below hKey
// = 0: do not recurse the tree
// > 0: recurse specified levels below hKey
//
void QueryKey(HKEY hKey, int RecursiveDepth = -1)
{
	TCHAR    achKey[MAX_KEY_LENGTH];   // buffer for subkey name
	DWORD    cbName;                   // size of name string 
	TCHAR    achClass[MAX_PATH] = TEXT("");  // buffer for class name 
	DWORD    cchClassName = MAX_PATH;  // size of class string 
	DWORD    cSubKeys = 0;               // number of subkeys
	DWORD    cbMaxSubKey;              // longest subkey size 
	DWORD    cchMaxClass;              // longest class string 
	DWORD    cValues;              // number of values for key 
	DWORD    cchMaxValue;          // longest value name 
	DWORD    cbMaxValueData;       // longest value data 
	DWORD    cbSecurityDescriptor; // size of security descriptor 
	FILETIME ftLastWriteTime;      // last write time 

	DWORD i;
	LONG retCode;

	TCHAR achValue[MAX_VALUE_NAME];
	DWORD cchValue = MAX_VALUE_NAME;

	// Get the class name and the value count. 
	retCode = RegQueryInfoKey(
		hKey,                    // key handle 
		achClass,                // buffer for class name 
		&cchClassName,           // size of class string 
		NULL,                    // reserved 
		&cSubKeys,               // number of subkeys 
		&cbMaxSubKey,            // longest subkey size 
		&cchMaxClass,            // longest class string 
		&cValues,                // number of values for this key 
		&cchMaxValue,            // longest value name 
		&cbMaxValueData,         // longest value data 
		&cbSecurityDescriptor,   // security descriptor 
		&ftLastWriteTime);       // last write time 

	if (retCode != ERROR_SUCCESS)
		return;

	// Enumerate the key values. 

	if (cValues)
	{
		printf("\nNumber of values: %u\n", cValues);

		std::vector<BYTE> buffer(cbMaxValueData + 1);

		for (i = 0; i < cValues; ++i)
		{
			cchValue = MAX_VALUE_NAME;
			retCode = RegEnumValue(hKey, i, achValue, &cchValue, NULL, NULL, NULL, NULL);
			if (retCode == ERROR_SUCCESS)
			{
				DWORD lpData = cbMaxValueData;
				retCode = RegQueryValueEx(hKey, achValue, 0, NULL, &buffer[0], &lpData);
				if (retCode == ERROR_SUCCESS)
					_tprintf(TEXT("(%u) %s : %.*s\n"), i + 1, achValue, lpData, &buffer[0]);
			}
		}
	}

	// Enumerate the subkeys.

	if ((cSubKeys) && (RecursiveDepth != 0))
	{
		printf("\nNumber of subkeys: %d\n", cSubKeys);

		int NextDepth = RecursiveDepth;
		if (NextDepth > 0) --NextDepth;

		std::vector<tstring> vecKeys;
		vecKeys.reserve(cSubKeys);

		for (i = 0; i < cSubKeys; ++i)
		{
			cbName = MAX_KEY_LENGTH;
			retCode = RegEnumKeyEx(hKey, i, achKey, &cbName, NULL, NULL, NULL, &ftLastWriteTime);
			if (retCode == ERROR_SUCCESS)
			{
				_tprintf(TEXT("(%d) %s\n"), i + 1, achKey);
				vecKeys.push_back(achKey);
			}
		}

		for (std::vector<tstring>::iterator iter = vecKeys.begin(), end = vecKeys.end(); iter != end; ++iter)
		{
			HKEY hTestKey;
			if (RegOpenKeyEx(hKey, iter->c_str(), 0, KEY_ALL_ACCESS | KEY_WOW64_64KEY, &hTestKey) == ERROR_SUCCESS)
			{
				QueryKey(hTestKey, NextDepth);
				RegCloseKey(hTestKey);
			}
		}
	}
}

int main(int argc, char *argv[])
{
	const TCHAR *subkey = TEXT("SOFTWARE\\");
	HKEY hTestKey;

	if (RegOpenKeyEx(HKEY_CURRENT_USER, subkey, 0, KEY_ALL_ACCESS | KEY_WOW64_64KEY, &hTestKey) == ERROR_SUCCESS)
	{
		QueryKey(hTestKey, 1);
		RegCloseKey(hTestKey);
	}
	printf("\nSOFTWARE\\HKEY_LOCAL_MACHINE\\s: \n");
	const TCHAR *subkey2 = TEXT("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall");
	if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, subkey2, 0, KEY_ALL_ACCESS | KEY_WOW64_32KEY, &hTestKey) == ERROR_SUCCESS)
	{
		QueryKey(hTestKey, 1);
		RegCloseKey(hTestKey);
	}

	std::cin.get();
	return 0;
}