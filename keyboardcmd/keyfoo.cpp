#define _CRT_SECURE_NO_WARNINGS
#include "keyfoo.h"


/*********************создание файла в определенной папке******************/
void filecreate()
{
	const int MAX = 2000;//максимальная длина строки
	char str[MAX], f_name[MAX_PATH]; //строка , имя файла
	//changeornot();
	cout << setw(5) << "Имя файла: ";
	cin >> f_name;

	ofstream fout(f_name); // создаём объект класса ofstream для записи 
	cin.get(str, MAX, '/');//пока не будет введен конечный символ будет ввод текста продолжен
	fout << str; // запись строки в файл
	fout.close(); // закрываем файл
}
/************************************************************************/

/***************функция смены диска*************************************/
void chdir()
{
	TCHAR sPath[MAX_PATH], sPathTo2[MAX_PATH];
	cout << setw(5) << "введите путь: ";
	cin >> sPathTo2;
	if (strcmp(sPathTo2, "test") == 0)
	{
		strcpy_s(sPathTo2, "C:\\testdirect\\");
	}
	strcpy_s(sPath, sPathTo2);//прим. от 25/09/2019 регистр имеет значение для диска 
	SetCurrentDirectory(sPath);

}
/**********************************************************************/

/*********************функция удаления файла в определенной папке*******/
void removefile() //added 02/09/2019 21:37
{
	char ans[4], fname[MAX_PATH], dirname[MAX_PATH];
	//changeornot();
	do
	{
		cout << setw(5) << "Удаляется directory (dir) или file (fil)?";
		cin >> ans;
	} while ((strcmp(ans, "dir") > 0) && (strcmp(ans, "fil") > 0));
	cin.clear();
	if ((strcmp(ans, "fil") == 0))
	{
		cout << "Файл для удаления: ";
		cin >> fname;
		if (remove(fname))
		{
			cout << "Ошибка удаления файла" << endl;
		}
		else
			cout << "Файл удален" << endl;
	}
	else
	{
		if ((strcmp(ans, "dir") == 0))
		{
			cout << "Папка для удаления: ";
			cin >> dirname;
			if (_rmdir(dirname) == -1) //10/12/2019 15:38 не убирать эту строчку, иначе не удаляет файл
			{
				cout << "Ошибка удаления папки" << endl;
				cout << "error" << strerror(errno) << endl;
			}
			else
				cout << "Папка удалена" << endl;
		}
	}
}
/**********************************************************************/