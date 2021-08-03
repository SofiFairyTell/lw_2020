#include <iostream>
#include <string>
#include <cstring>
#include <iostream>
#include <fstream>
#include <stdio.h>
#define _CRT_SECURE_NO_WARNINGS
char* userStrstr(const char* haystack,  char* needle)
{
	for (const char* hp = haystack; hp != haystack + strlen(haystack); ++hp)
	{
		const char* np = needle;
		const char* tmp = hp;
		int pos = 1;
		for (; np != needle + strlen(needle); ++np)
		{
			if (*tmp != *np)
			{

				break;
			}
			else
			{
				++pos;
				++tmp;
			}
		}
		if (np == needle + strlen(needle))
		{
			std::cout <<"\n ������ �����: ������� �� " << pos+1<<"������� �� ������ ������\n";
			return needle;
		}
	}
	return 0;
}
int BMSearch(char* str, char* substr)
{
	int sl, ssl;
	int res = -1;
	sl = strlen(str);
	ssl = strlen(substr);
	int i, Pos;
	int BMT[256];
	for (i = 0; i < 256; i++)
		BMT[i] = ssl;
	for (i = ssl - 1; i >= 0; i--)
		if (BMT[((short)substr[i])] == ssl)
			BMT[(short)(substr[i])] = ssl - i - 1;
	Pos = ssl - 1;
	while (Pos < sl)
		if (substr[ssl - 1] != str[Pos])
			Pos = Pos + BMT[((short)str[Pos])];
		else
			for (i = ssl - 2; i >= 0; i--) {
				if (substr[i] != str[Pos - ssl + i + 1]) {
					Pos += BMT[(short)(str[Pos - ssl + i + 1])] - 1;
					std::cout << "\n �������� ������-����: ������� �� " << Pos << " ������� �� ������ ������\n";
					break;
				}
				else
					if (i == 0)
						return Pos - ssl + 1;
			}
	
	return res;
}

int KMPSearch(char* str, char* substr)
{
	int sl, ssl;
	int res = -1;
	sl = strlen(str);//����� �������� ������
	ssl = strlen(substr);//����� ���������
	int i, j = 0, k = -1;
	int* d;
	d = new int[1000]; //������ ��� ��������-�������
	d[0] = -1;  
	while (j < ssl - 1) 
	{
		while (k >= 0 && substr[j] != substr[k])
			k = d[k];
		j++;
		k++;
		if (substr[j] == substr[k])
			d[j] = d[k];
		else
			d[j] = k;
	}
	i = 0;
	j = 0;
	while (j < ssl && i < sl) {
		while (j >= 0 && str[i] != substr[j])
			j = d[j];//�������� �������� ��������������� � ������
		i++;
		j++;
	}
	delete[] d;
	res = j == ssl ? i - ssl : -1;
	//std::cout << "\n �������� �����, ������� � ������: ������� �� " << i - ssl + 1 << " ������� �� ������ ������\n";
	std::cout << "\n �������� �����, ������� � ������: ������� �� "<<res+1<<" ������� �� ������ ������\n";
	return res;
}

int main()
{
	setlocale(LC_ALL, "");
	//char path = "C:\\test\\tests.txt";
	char path[100];
	char line[100];
	char search[100];
	std::cout << "Enter path:\t";
	std::cin >> path;
	std::cout << "Enter search word:\t";
	std::cin >> search;
	std::ifstream input(path);	
	
	while (!input.is_open())
	{
		std::cerr << "File error" << std::endl;
		std::cin.ignore();
		break;
	}
	if (input)
	{
		while (!input.eof())
		{
			input.getline(line, 80);
			//std::cout << "������ 1";
			std::cout << "�������� � ������-���������: " << strlen(line) << ". �������� � ������� ������: " << strlen(search) << std::endl;
			if (userStrstr(line, search))
			{
				std::cout << "��������� = "<< ((strlen(line) - strlen(search)+1)*strlen(search)) <<std::endl;
				std::cout <<"Your word: " <<search << " in "<<"' "<<line<<" '"<<std::endl;
			}
		
			if (KMPSearch(line, search)!= -1)
			{

				std::cout << "��������� = " << (strlen(line) + strlen(search)) <<std::endl;
				std::cout << "Your word: " << search << " in " << "' " << line << " '" << std::endl;
			}
			if (BMSearch(line, search)!= -1)
			{
				std::cout << "��������� = " << (strlen(line) + strlen(search)) << std::endl;
				std::cout << "Your word: " << search << " in " << "' " << line << " '" << std::endl;
			}
		}
	}
	input.close();
}