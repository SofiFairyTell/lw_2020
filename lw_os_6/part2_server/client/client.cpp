#include <Windows.h>
#include <tchar.h>
#include <stdio.h>
#include <conio.h>
#include <strsafe.h>
#include <locale.h>

#pragma pack(push, 1)

struct REQUEST
{
	DWORD PID;
	DWORD index;
}; 

#pragma pack(pop)

REQUEST Request; // запрос

DWORD nBytes;
TCHAR Response[100]; // ответ

TCHAR data[8][255] = { L"" };

void getData();
// ------------------------------------------------------------------------------------------------
int _tmain(int argc, LPCTSTR argv[])
{
	_tsetlocale(LC_ALL, TEXT(""));

	_tprintf(TEXT("> јргументы командной строки:\n"));
	_tprintf(TEXT("> \n"));

	for (int i = 0; i < argc; ++i)
	{
		_tprintf(TEXT("> %s\n"), argv[i]);
	} 

	_tprintf(TEXT("\n"));



	if (2 == argc)
	{
		if (_tcsicmp(argv[1], TEXT("/get_data_list")) == 0) // именованные каналы
		{
			getData();	
			for (int i = 0; i < sizeof(data); i++)
			{
				_tprintf(TEXT(">%d %s\n"), i++, data[i]);
			}
		} 
	} 
	if (argc == 3)
	{
		if (_tcsicmp(argv[1], TEXT("/get_by_number")) == 0)
				{
					getData();
						if (argv[2])
						{
							int input = _ttoi(argv[2]) - 1;//страшна€ глупость. но, а как иначе? ѕользователь ввел 2, но дл€ массива с 0 это 3-й элемент
							for (int i = 0; i < sizeof(data); i++)
							{
								if (input == i)
									{
										_tprintf(TEXT("> %s\n"), data[i]);
									}
							}
						}
				}
	}
	
	
} 

void getData()
{
	int count = 0;
	for (;;)
	{
		// соединение с именованным каналом
		//pipe - это  мен€ть нельз€.
		BOOL bRet = CallNamedPipe(TEXT("\\\\.\\pipe\\test_pipe"),
			&Request, sizeof(Request), Response, sizeof(Response), &nBytes, NMPWAIT_WAIT_FOREVER);

		if (FALSE != bRet)
		{
			if (_T('\0') == Response[0])
			{
				break; // выходим из цикла
			}
			StringCchCat(data[count++], _countof(data[count++]), Response);//заполним список данными полученными от сервера

			//_tprintf(TEXT("> %d %s\n"), (Request.index + 1), Response);

			Sleep(200);
		}
		else
		{
			_tprintf(TEXT("> ќшибка: %d\n"), GetLastError());
			break;
		}
		++Request.index;

	}
}