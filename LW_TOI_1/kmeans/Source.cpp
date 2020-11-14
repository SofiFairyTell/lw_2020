#include <iostream>
#include <locale.h>
#include <fstream>
#include <vector>
#include <string>
#include <cstring>
#include<Windows.h>
#include <stdlib.h>
#include <iomanip>

#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
using namespace std;
#define MAXITER 30

#pragma warning (disable:4996)
string Message[10];
string MessageStemming[10];
string STOP[165];
vector<unsigned char> RV, R1, R2;
string StemmWordCpy, del, StemmWordCpy1;
int globfl = 0, globCount = 0, kolZagolovkov = 0;

void file_inp(string fName, string *Message, int flag)
{
	FILE *fin;
	int read = 0;
	unsigned char b = ' ';
	char a[50];
	strcpy_s(a, fName.c_str());
	fin = fopen(a, "r+b");
	if (fin != NULL)
	{

		fseek(fin, 0, SEEK_END);
		long filesize = ftell(fin);
		fseek(fin, 0, SEEK_SET);
		int i = 0;
		string stro = "";
		if (flag != 1)
			cout << "ƒанные из файла " << fName << endl;
		do
		{
			fread(&b, sizeof(unsigned char), 1, fin);
			read++;
			while ((b != '\r') && (read <= filesize))
			{
				stro += b;
				fread(&b, sizeof(unsigned char), 1, fin);
				read++;
			}
			Message[i] = stro;
			if (flag != 1)
				cout << stro << endl;
			i++;
			stro = "";
			fread(&b, sizeof(unsigned char), 1, fin);
			read++;
			if (b != '\n')
				stro += b;
		} while (read < filesize);
		if (flag != 1)
			kolZagolovkov = i;
	}
}

void DelSTOP()
{
	int i, j;
	const char* t; const char* t1; const char* t_; const char* t1_;
	string w;
	cout << "--------------------------------------------------------------" << endl;
	cout << "«аголовки без —“ќѕ-символов!" << endl;
	cout << "--------------------------------------------------------------" << endl;
	for (i = 0; i < kolZagolovkov; i++)
	{
		string stroka = Message[i];

		for (j = 0; j < 165; j++)
		{
			t = strstr(stroka.c_str(), STOP[j].c_str());
			if (t != NULL)
			{
				w = STOP[j];
				t1 = strstr(w.c_str(), " ");
				t1_ = t1 + 1;
				strcpy((char*)t1, (char*)t1_);

				t_ = t + 1 + strlen(w.c_str());
				strcpy((char*)t + 1, (char*)t_);
			}
		}
		string ex = "";
		for (int t = 0; t < strlen(stroka.c_str()); t++)
			ex += stroka[t];
		Message[i] = ex;
		cout << Message[i] << endl;
	}
}


void CutWord(string StemmWord)
{
	RV.clear();
	int sizeWord = StemmWord.size(), flag = -1;
	vector<unsigned char>vocal(10);
	vocal[0] = 'а'; vocal[1] = 'е'; vocal[2] = 'и'; vocal[3] = 'о'; vocal[4] = 'у'; vocal[5] = 'ы'; vocal[6] = 'э'; vocal[7] = 'ю';
	vocal[8] = '€'; vocal[9] = 'Є';
	for (int i = 0; i < sizeWord; i++)
	{
		for (int j = 0; j < 10; j++)
		{

			if ((char)StemmWord[i] == (char)vocal[j])
			{
				flag = i;
				break;
			}
		}
		if (flag != -1)
			break;
	}
	for (int i = flag + 1; i < StemmWord.size(); i++)
		RV.push_back(StemmWord[i]);
}

int Step1()
{
	string suff;
	char word = ' ';
	vector<string>PerfGerund1(3);
	PerfGerund1[0] = "в"; PerfGerund1[1] = "вши"; PerfGerund1[2] = "вшись";
	vector<string>PerfGerund2(6);
	PerfGerund2[4] = "ив"; PerfGerund2[3] = "ивши"; PerfGerund2[0] = "ившись"; PerfGerund2[5] = "ыв"; PerfGerund2[2] = "ывши"; PerfGerund2[1] = "ывшись";
	if (RV[RV.size() - 1] == 'a' || RV[RV.size() - 1] == '€')
	{
		if (RV.size() >= 5)
		{
			for (int i = 6; i > 1; i--)
				suff.push_back(RV[RV.size() - i]);
			if (suff == PerfGerund1[2])
			{
				RV.resize(RV.size() - 6);
				return(0);
			}
		}
		if (RV.size() >= 3)
		{
			suff.clear();
			for (int i = 4; i > 1; i--)
				suff.push_back(RV[RV.size() - i]);
			if (suff == PerfGerund1[1])
			{
				RV.resize(RV.size() - 4);
				return(0);
			}
		}
		if (RV.size() >= 1)
		{
			suff.clear();
			suff.push_back(RV[RV.size() - 2]);
			if (suff == PerfGerund1[0])
			{
				RV.resize(RV.size() - 2);
				return(0);
			}
		}
	}
	else
	{
		if (RV.size() >= 6)
		{
			suff.clear();
			for (int i = 6; i > 0; i--)
				suff.push_back(RV[RV.size() - i]);
			if (suff == PerfGerund2[0] || suff == PerfGerund2[1])
			{
				RV.resize(RV.size() - 6);
				return(0);
			}
		}
		if (RV.size() >= 4)
		{
			suff.clear();
			for (int i = 4; i > 0; i--)
				suff.push_back(RV[RV.size() - i]);
			if (suff == PerfGerund2[2] || suff == PerfGerund2[3])
			{
				RV.resize(RV.size() - 4);
				return(0);
			}
		}
		if (RV.size() >= 2)
		{
			suff.clear();
			for (int i = 2; i > 0; i--)
				suff.push_back(RV[RV.size() - i]);
			if (suff == PerfGerund2[4] || suff == PerfGerund2[5])
			{
				RV.resize(RV.size() - 2);
				return(0);
			}
		}
	}
	vector<string>reflexive(2);
	reflexive[0] = "с€"; reflexive[1] = "сь";
	if (RV.size() >= 2)
	{
		suff.clear();
		for (int i = 2; i > 0; i--)
			suff.push_back(RV[RV.size() - i]);
		if (suff == reflexive[0] || suff == reflexive[1])
			RV.resize(RV.size() - 2);
	}
	vector<string>adjectival(26);
	adjectival[0] = "ее"; adjectival[1] = "ие"; adjectival[2] = "ые"; adjectival[3] = "ое"; adjectival[4] = "ими"; adjectival[5] = "ыми"; adjectival[6] = "ей";
	adjectival[7] = "ий"; adjectival[8] = "ый"; adjectival[9] = "ой"; adjectival[10] = "ем"; adjectival[11] = "им"; adjectival[12] = "ым"; adjectival[13] = "ом";
	adjectival[14] = "его"; adjectival[15] = "ого"; adjectival[16] = "ему"; adjectival[17] = "ому"; adjectival[18] = "их"; adjectival[19] = "ых"; adjectival[20] = "ую";
	adjectival[21] = "юю"; adjectival[22] = "а€"; adjectival[23] = "€€"; adjectival[24] = "ою"; adjectival[25] = "ею";
	int flag = -1;
	if (RV.size() >= 3)
	{
		suff.clear();
		for (int i = 3; i > 0; i--)
			suff.push_back(RV[RV.size() - i]);
		if (suff == adjectival[4] || suff == adjectival[5] || suff == adjectival[14] || suff == adjectival[15] || suff == adjectival[16] || suff == adjectival[17])
		{
			RV.resize(RV.size() - 3);
			flag = 1;
		}
	}
	if (RV.size() >= 2)
	{
		suff.clear();
		for (int i = 2; i > 0; i--)
			suff.push_back(RV[RV.size() - i]);
		if (suff == adjectival[0] || suff == adjectival[1] || suff == adjectival[2] || suff == adjectival[3] || suff == adjectival[6] || suff == adjectival[7] || suff == adjectival[8] ||
			suff == adjectival[9] || suff == adjectival[10] || suff == adjectival[11] || suff == adjectival[12] || suff == adjectival[13] || suff == adjectival[18] || suff == adjectival[19] ||
			suff == adjectival[20] || suff == adjectival[21] || suff == adjectival[22] || suff == adjectival[23] || suff == adjectival[24] || suff == adjectival[25])
		{
			word = RV[RV.size() - 2];
			RV.resize(RV.size() - 2);
			flag = 1;
		}
	}
	if (flag == 1)
	{
		vector<string>participle1(5);
		participle1[0] = "ем"; participle1[1] = "нн"; participle1[2] = "вш"; participle1[3] = "ющ"; participle1[4] = "щ";
		vector<string>participle2(3);
		participle2[0] = "ивш"; participle2[1] = "ывш"; participle2[2] = "ующ";
		if (word == 'а' || word == '€' || RV[RV.size() - 1] == 'a' || RV[RV.size() - 1] == '€')
		{
			if (RV.size() >= 2)
			{
				suff.clear();
				if ((word != 'а') && (word != '€'))
				{
					for (int i = 3; i > 1; i--)
						suff.push_back(RV[RV.size() - i]);
					if (suff == participle1[0] || suff == participle1[1] || suff == participle1[2] || suff == participle1[3])
					{
						RV.resize(RV.size() - 3);
						return(0);
					}
				}
				else
				{
					for (int i = 2; i > 0; i--)
						suff.push_back(RV[RV.size() - i]);
					if (suff == participle1[0] || suff == participle1[1] || suff == participle1[2] || suff == participle1[3])
					{
						RV.resize(RV.size() - 2);
						return(0);
					}
				}
			}
			if (RV.size() >= 1)
			{
				suff.clear();
				if (word != 'а' || word != '€')
				{
					suff.push_back(RV[RV.size() - 2]);
					if (suff == participle1[4])
					{
						RV.resize(RV.size() - 2);
						return(0);
					}
				}
				else
				{
					suff.push_back(RV[RV.size() - 1]);
					if (suff == participle1[4])
					{
						RV.resize(RV.size() - 1);
						return(0);
					}
				}
			}
		}
		else
		{
			if (RV.size() >= 3)
			{
				suff.clear();
				for (int i = 3; i > 0; i--)
					suff.push_back(RV[RV.size() - i]);
				if (suff == participle2[0] || suff == participle2[1] || suff == participle2[2])
				{
					RV.resize(RV.size() - 3);
					return(0);
				}
			}
		}
		return(0);
	}
	vector<string>verb1(17);
	verb1[0] = "ла";	verb1[1] = "на";	verb1[2] = "ете";	verb1[3] = "йте";	verb1[4] = "ли";	verb1[5] = "й";	verb1[6] = "л";	verb1[7] = "ем";
	verb1[8] = "н";	verb1[9] = "ло"; verb1[10] = "но";	verb1[11] = "ет";	verb1[12] = "ют";	verb1[13] = "ны";	verb1[14] = "ть";	verb1[15] = "ешь";	verb1[16] = "нно";
	vector<string>verb2(29);
	verb2[0] = "ила"; verb2[1] = "ыла"; verb2[2] = "ена"; verb2[3] = "ейте"; verb2[4] = "уйте"; verb2[5] = "ите"; verb2[6] = "или"; verb2[7] = "ыли";
	verb2[8] = "ей"; verb2[9] = "уй"; verb2[10] = "ил"; verb2[11] = "ыл"; verb2[12] = "им"; verb2[13] = "ым"; verb2[14] = "ен"; verb2[15] = "ило";
	verb2[16] = "ыло"; verb2[17] = "ено"; verb2[18] = "€т"; verb2[19] = "ует"; verb2[20] = "уют"; verb2[21] = "ит"; verb2[22] = "ыт"; verb2[23] = "ены";
	verb2[24] = "ить"; verb2[25] = "ыть"; verb2[26] = "ишь"; verb2[27] = "ую"; verb2[28] = "ю";
	if (RV[RV.size() - 1] == 'a' || RV[RV.size() - 1] == '€')
	{
		if (RV.size() >= 3)
		{
			suff.clear();
			for (int i = 4; i > 1; i--)
				suff.push_back(RV[RV.size() - i]);
			if (suff == verb1[2] || suff == verb1[3] || suff == verb1[15] || suff == verb1[16])
			{
				RV.resize(RV.size() - 4);
				return(0);
			}
		}
		if (RV.size() >= 2)
		{
			suff.clear();
			for (int i = 3; i > 1; i--)
				suff.push_back(RV[RV.size() - i]);
			if (suff == verb1[0] || suff == verb1[1] || suff == verb1[4] || suff == verb1[7] || suff == verb1[9] || suff == verb1[10] || suff == verb1[11] ||
				suff == verb1[12] || suff == verb1[13] || suff == verb1[14])
			{
				RV.resize(RV.size() - 3);
				return(0);
			}
		}
		if (RV.size() >= 1)
		{
			suff.clear();
			suff.push_back(RV[RV.size() - 2]);
			if (suff == verb1[5] || suff == verb1[6] || suff == verb1[8])
			{
				RV.resize(RV.size() - 2);
				return(0);
			}
		}
	}
	else
	{
		if (RV.size() >= 4)
		{
			suff.clear();
			for (int i = 4; i > 0; i--)
				suff.push_back(RV[RV.size() - i]);
			if (suff == verb2[3] || suff == verb2[4])
			{
				RV.resize(RV.size() - 4);
				return(0);
			}
		}
		if (RV.size() >= 3)
		{
			suff.clear();
			for (int i = 3; i > 0; i--)
				suff.push_back(RV[RV.size() - i]);
			if (suff == verb2[0] || suff == verb2[1] || suff == verb2[2] || suff == verb2[5] || suff == verb2[6] || suff == verb2[7] || suff == verb2[15] ||
				suff == verb2[16] || suff == verb2[17] || suff == verb2[19] || suff == verb2[20] || suff == verb2[23] || suff == verb2[24] || suff == verb2[25]
				|| suff == verb2[26])
			{
				RV.resize(RV.size() - 3);
				return(0);
			}
		}
		if (RV.size() >= 2)
		{
			suff.clear();
			for (int i = 2; i > 0; i--)
				suff.push_back(RV[RV.size() - i]);
			if (suff == verb2[8] || suff == verb2[9] || suff == verb2[10] || suff == verb2[11] || suff == verb2[12] || suff == verb2[13] || suff == verb2[14]
				|| suff == verb2[18] || suff == verb2[21] || suff == verb2[22] || suff == verb2[27])
			{
				RV.resize(RV.size() - 2);
				return(0);
			}
		}
		if (RV.size() >= 1)
		{
			suff.clear();
			suff.push_back(RV[RV.size() - 1]);
			if (suff == verb2[28])
			{
				RV.resize(RV.size() - 1);
				return(0);
			}
		}
	}
	vector<string>noun(36);
	noun[0] = "и€ми"; noun[1] = "€ми"; noun[2] = "ами"; noun[3] = "ией"; noun[4] = "и€м"; noun[5] = "ием"; noun[6] = "и€х"; noun[7] = "ев";
	noun[8] = "ов"; noun[9] = "ие"; noun[10] = "ье"; noun[11] = "еи"; noun[12] = "ии"; noun[13] = "ей"; noun[14] = "ой"; noun[15] = "ий";
	noun[16] = "€м"; noun[17] = "ем"; noun[18] = "ам"; noun[19] = "ом"; noun[20] = "ах"; noun[21] = "€х"; noun[22] = "ию"; noun[23] = "ью";
	noun[24] = "и€"; noun[25] = "ь€"; noun[26] = "а"; noun[27] = "е"; noun[28] = "и"; noun[29] = "й"; noun[30] = "о"; noun[31] = "у";
	noun[32] = "ы"; noun[33] = "ь"; noun[34] = "ю"; noun[35] = "€";
	if (RV.size() >= 4)
	{
		suff.clear();
		for (int i = 4; i > 0; i--)
			suff.push_back(RV[RV.size() - i]);
		if (suff == noun[0])
		{
			RV.resize(RV.size() - 4);
			return(0);
		}
	}
	if (RV.size() >= 3)
	{
		suff.clear();
		for (int i = 3; i > 0; i--)
			suff.push_back(RV[RV.size() - i]);
		if (suff == noun[1] || suff == noun[2] || suff == noun[3] || suff == noun[4] || suff == noun[5] || suff == noun[6])
		{
			RV.resize(RV.size() - 3);
			return(0);
		}
	}
	if (RV.size() >= 2)
	{
		suff.clear();
		for (int i = 2; i > 0; i--)
			suff.push_back(RV[RV.size() - i]);
		if (suff == noun[7] || suff == noun[8] || suff == noun[9] || suff == noun[10] || suff == noun[11] || suff == noun[12] || suff == noun[13] ||
			suff == noun[14] || suff == noun[15] || suff == noun[16] || suff == noun[17] || suff == noun[18] || suff == noun[19] || suff == noun[20] ||
			suff == noun[21] || suff == noun[22] || suff == noun[23] || suff == noun[24] || suff == noun[25])
		{
			RV.resize(RV.size() - 2);
			return(0);
		}
	}
	if (RV.size() >= 1)
	{
		suff.clear();
		suff.push_back(RV[RV.size() - 1]);
		if (suff == noun[26] || suff == noun[27] || suff == noun[28] || suff == noun[29] || suff == noun[30] || suff == noun[31] || suff == noun[32] ||
			suff == noun[33] || suff == noun[34] || suff == noun[35])
		{
			RV.resize(RV.size() - 1);
			return(0);
		}
	}
}
void Step2()
{
	if (RV.size() >= 1)
	{
		if (RV[RV.size() - 1] == 'и')
			RV.resize(RV.size() - 1);
	}
}
int FindR1(string StemmWordCpy)
{
	int i = 0, flag = 0;
	vector<unsigned char>vocal(10);
	vocal[0] = 'а'; vocal[1] = 'е'; vocal[2] = 'и'; vocal[3] = 'о'; vocal[4] = 'у'; vocal[5] = 'ы'; vocal[6] = 'э'; vocal[7] = 'ю';
	vocal[8] = '€'; vocal[9] = 'Є';
	while (i < StemmWordCpy.size())
	{
		for (int j = 0; j < 10; j++)
		{
			if ((char)StemmWordCpy[i] == (char)vocal[j])
			{
				for (int r = 0; r < 10; r++)
				{
					if ((char)StemmWordCpy[i + 1] == (char)vocal[r])
						flag = 1;
				}
				if (flag != 1)
				{
					for (int l = i + 2; l < StemmWordCpy.size(); l++)
						R1.push_back(StemmWordCpy[l]);
					return(0);
				}
			}
		}
		i++;
	}
}
int FindR2()
{
	int i = 0, flag = 0;
	vector<unsigned char>vocal(10);
	vocal[0] = 'а'; vocal[1] = 'е'; vocal[2] = 'и'; vocal[3] = 'о'; vocal[4] = 'у'; vocal[5] = 'ы'; vocal[6] = 'э'; vocal[7] = 'ю';
	vocal[8] = '€'; vocal[9] = 'Є';
	while (i < R1.size())
	{
		for (int j = 0; j < 10; j++)
		{
			if (R1[i] == vocal[j])
			{
				for (int r = 0; r < 10; r++)
				{
					if (R1[i + 1] == vocal[r])
						flag = 1;
				}
				if (flag != 1)
				{
					for (int l = i + 2; l < R1.size(); l++)
						R2.push_back(R1[l]);
					return(0);
				}
			}
		}
		i++;
	}
}

int Step3()
{
	vector<string>derivational(2);
	derivational[0] = "ост"; derivational[1] = "ость";
	string suff;
	if (R2.size() < 3)
	{
		globfl = 1;
		return(0);
	}
	if (R2.size() >= 4)
	{
		for (int i = 4; i > 0; i--)
			suff.push_back(R2[R2.size() - i]);
		if (suff == derivational[1])
		{
			R2.resize(R2.size() - 4);
			return(0);
		}
	}
	if (R2.size() >= 3)
	{
		suff.clear();
		for (int i = 3; i > 0; i--)
			suff.push_back(R2[R2.size() - i]);
		if (suff == derivational[0])
		{
			R2.resize(R2.size() - 3);
			return(0);
		}
	}
}
int Step4()
{
	if (RV.size() >= 1)
	{
		if (RV[RV.size() - 1] == 'ь')
		{
			RV.resize(RV.size() - 1);
			return(0);
		}
	}
	vector<string>superlativ(3);
	string suff;
	superlativ[0] = "нн"; superlativ[1] = "ейш"; superlativ[2] = "ейше";
	if (RV.size() >= 4)
	{
		for (int i = 4; i > 0; i--)
			suff.push_back(RV[RV.size() - i]);
		if (suff == superlativ[2])
			RV.resize(RV.size() - 4);
	}
	if (RV.size() >= 3)
	{
		suff.clear();
		for (int i = 3; i > 0; i--)
			suff.push_back(RV[RV.size() - i]);
		if (suff == superlativ[1])
			RV.resize(RV.size() - 3);
	}
	if (RV.size() >= 2)
	{
		suff.clear();
		for (int i = 2; i > 0; i--)
			suff.push_back(RV[RV.size() - i]);
		if (suff == superlativ[0])
		{
			RV.resize(RV.size() - 1);
			return(0);
		}
	}
	return(0);
}
void Rez(string StemmWord, int flag)
{
	StemmWordCpy.clear(); del.clear();
	if (globfl == 1)
		flag = 1;
	if (flag == 1)
	{
		for (int i = 0; i < RV.size(); i++)
			del.push_back(RV[i]);
		int a = StemmWord.find(del);
		for (int i = 0; i < a; i++)
			StemmWordCpy.push_back(StemmWord[i]);
		for (int i = 0; i < RV.size(); i++)
			StemmWordCpy.push_back(RV[i]);
		globfl = 0;
	}
	else if (flag == 2)
	{
		for (int i = 0; i < R2.size(); i++)
			del.push_back(R2[i]);
		int a = StemmWord.find(del);
		for (int i = 0; i < a; i++)
			StemmWordCpy.push_back(StemmWord[i]);
		for (int i = 0; i < R2.size(); i++)
			StemmWordCpy.push_back(R2[i]);
	}
}

void AllWord()
{
	string a, wordMS, StemW = ""; int r = 0; char znak;
	int j = 0, i = 0;
	cout << "--------------------------------------------------------------" << endl;
	cout << "C“≈ћћ»Ќ√" << endl;
	cout << "--------------------------------------------------------------" << endl;
	while (i < kolZagolovkov)
	{
		wordMS = Message[i];
		while (j < strlen(wordMS.c_str()))
		{
			while (wordMS[j] != ' ' && wordMS[j] != ','&& wordMS[j] != '.'&& r < strlen(wordMS.c_str()))
			{
				a.push_back(wordMS[j]);
				RV.push_back(wordMS[j]);
				if ((r = j + 1) != strlen(wordMS.c_str()));
				j++;
			}
			if (wordMS[j] == ' ')
			{
				znak = wordMS[j];
				j++;
			}
			else
			{
				znak = wordMS[j];
				j = j + 2;
			}
			if (RV.size() != 0)
			{
				Step1();
				Step2();
				Rez(a, 1);
				FindR1(StemmWordCpy);
				FindR2();
				Step3();
				Rez(a, 2);
				CutWord(StemmWordCpy);
				Step4();
				Rez(a, 1);
				StemW += StemmWordCpy + znak;
				RV.clear();
				R1.clear();
				R2.clear();
				a.clear();
			}
		}
		j = 0;
		string ex = "";
		for (int t = 0; t < strlen(StemW.c_str()); t++)
			ex += StemW[t];
		MessageStemming[i] = ex;
		cout << MessageStemming[i] << endl;
		i++;
		r = 0;
		StemW = "";
	}
}

void AloneWord(string *LSA, double **Arr)
{
	int i = 0, j = 0, k, flag = 0, r = 0, u;
	string wordMS, alone;
	cout << "--------------------------------------------------------------" << endl;
	cout << "«ј√ќЋќ¬ » Ѕ≈« ќƒ»Ќќ„Ќџ’ —Ћќ¬" << endl;
	cout << "--------------------------------------------------------------" << endl;
	while (i < kolZagolovkov)
	{
		wordMS = MessageStemming[i];
		while (j < strlen(wordMS.c_str()))
		{
			while (wordMS[j] != ' ' && wordMS[j] != ','&& wordMS[j] != '.'&& r < strlen(wordMS.c_str()))
			{
				alone += wordMS[j];
				if ((r = j + 1) != strlen(wordMS.c_str()));
				j++;
			}
			if (wordMS[j] == ' ')
				j++;
			else j = j + 2;
			for (k = 0; k < 10; k++)
			{
				if (k != i)
				{
					if (strstr(MessageStemming[k].c_str(), alone.c_str()) != NULL)
						flag = 1;
				}
			}
			if (flag == 0)
			{
				const char* t = strstr(wordMS.c_str(), alone.c_str());
				const char* t_ = t + strlen(alone.c_str());
				strcpy((char*)t, (char*)t_);
				j = j - strlen(alone.c_str());
				r = r - strlen(alone.c_str());
			}
			else
			{
				flag = -1;
				for (u = 0; u < 20; u++)
				{
					if (alone == LSA[u]) {
						flag = u;
						break;
					}
				}
				if (flag < 0)
				{
					LSA[globCount] = alone;
					int chislo = Arr[globCount][i];
					chislo++;
					Arr[globCount][i] = chislo;
					globCount++;
				}
				else
				{
					int chislo = Arr[u][i];
					chislo++;
					Arr[u][i] = chislo;
				}
				flag = 0;
			}
			alone = "";
		}
		string ex = "";
		for (int t = 0; t < strlen(wordMS.c_str()); t++)
			ex += wordMS[t];
		MessageStemming[i] = ex;
		cout << MessageStemming[i] << endl;
		i++;
		r = 0;
		j = 0;
	}
	cout << "--------------------------------------------------------------" << endl;
	cout << "„ј—“ќ“Ќјя ћј“–»÷ј »Ќƒ≈ —»–”≈ћџ’ —Ћќ¬" << endl;
	cout << "--------------------------------------------------------------" << endl;
	cout << endl;
	for (int u = 0; u < globCount; u++)
	{
		cout << setw(6) << LSA[u];
		for (int y = 0; y < kolZagolovkov; y++) {
			cout << setw(6) << Arr[u][y];
		}
		cout << endl;
	}
}

void Transp(double **Arr, double **TArr, float **CArr)
{
	for (int i = 0; i < globCount; i++)
	{
		for (int j = 0; j < kolZagolovkov; j++)
			TArr[j][i] = Arr[i][j];
	}
	cout << "--------------------------------------------------------------" << endl;
	cout << "“–јЌ—ѕќЌ»–ќ¬јЌЌјя ћј“–»÷ј" << endl;
	cout << "--------------------------------------------------------------" << endl;
	cout << endl;
	for (int i = 0; i < kolZagolovkov; i++)
	{
		for (int j = 0; j < globCount; j++)
			cout << setw(4) << TArr[i][j];
		cout << endl;
	}
	for (int i = 0; i < globCount; ++i)
		for (int j = 0; j < globCount; ++j)
		{
			CArr[i][j] = 0;
			for (int k = 0; k < kolZagolovkov; ++k)
				CArr[i][j] += (Arr[i][k] * TArr[k][j]);
		}
	cout << "--------------------------------------------------------------" << endl;
	cout << " ¬јƒ–ј“Ќјя ћј“–»÷ј" << endl;
	cout << "--------------------------------------------------------------" << endl;
	cout << endl;
	for (int i = 0; i < globCount; i++)
	{
		for (int j = 0; j < globCount; j++)
			cout << setw(4) << CArr[i][j];
		cout << endl;
	}

}

void tred2(float **CArr, int n, float *d, float *e) {
	int l, k, j, i;
	float scale, hh, h, g, f;
	for (i = n; i >= 2; i--)
	{
		l = i - 1; h = scale = 0.;
		if (l > 1) {
			for (k = 1; k <= l; k++)
				scale += fabs(CArr[i][k]);
			if (scale == 0.) e[i] = CArr[i][l];
			else {
				for (k = 1; k <= l; k++) {
					CArr[i][k] /= scale; h += CArr[i][k] * CArr[i][k];
				}
				f = CArr[i][l];
				g = (f >= 0. ? -sqrt(h) : sqrt(h));
				e[i] = scale * g; h -= f * g;
				CArr[i][l] = f - g;
				f = 0.;
				for (j = 1; j <= l; j++) {
					CArr[j][i] = CArr[i][j] / h;
					g = 0.;
					for (k = 1; k <= j; k++) g += CArr[j][k] * CArr[i][k];
					for (k = j + 1; k <= l; k++) g += CArr[k][j] * CArr[i][k];
					e[j] = g / h;
					f += e[j] * CArr[i][j];
				}
				hh = f / (h + h);
				for (j = 1; j <= l; j++) {
					f = CArr[i][j]; e[j] = g = e[j] - hh * f;
					for (k = 1; k <= j; k++) CArr[j][k] -= (f*e[k] + g * CArr[i][k]);
				}
			}
		}
		else e[i] = CArr[i][l];
		d[i] = h;
	}
	d[1] = 0.;
	e[1] = 0.;
	for (i = 1; i <= n; i++) {
		l = i - 1;
		if (d[i] != 0.) {
			for (j = 1; j <= l; j++) {
				g = 0.;
				for (k = 1; k <= l; k++) g += CArr[i][k] * CArr[k][j];
				for (k = 1; k <= l; k++) CArr[k][j] -= g * CArr[k][i];
			}
		}
		d[i] = CArr[i][i];
		CArr[i][i] = 1.;
		for (j = 1; j <= l; j++) CArr[j][i] = CArr[i][j] = 0.;
	}
}

void tqli(float *d, float *e, int n, float **z) {
	int m, l, iter, i, k;
	float s, r, p, g, f, dd, c, b;
	for (i = 2; i <= n; i++) e[i - 1] = e[i];
	e[n] = 0.;
	for (l = 1; l <= n; l++) {
		iter = 0;
		do {
			for (m = l; m <= n - 1; m++) {
				dd = fabs(d[m]) + fabs(d[m + 1]);
				if ((float)(fabs(e[m] + dd) == dd)) break;
			}
			if (m != l) {
				if (++iter >= MAXITER) { cout << "Error!!!"; return; };
				g = (d[l + 1] - d[l]) / (2.*e[l]); r = hypot(1., g);
				if (g >= 0.) g += fabs(r);
				else g -= fabs(r);
				g = d[m] - d[l] + e[l] / g;
				s = c = 1.; p = 0.;
				for (i = m - 1; i >= l; i--) {
					f = s * e[i]; b = c * e[i];
					e[i + 1] = r = hypot(f, g);
					if (r == 0.) { d[i + 1] -= p; e[m] = 0.; break; }
					s = f / r; c = g / r; g = d[i + 1] - p; r = (d[i] - g)*s + 2.*c*b; d[i + 1] = g + (p = s * r); g = c * r - b;
					for (k = 1; k <= n; k++) {
						f = z[k][i + 1]; z[k][i + 1] = s * z[k][i] + c * f; z[k][i] = c * z[k][i] - s * f;
					}
				}
				if (r == 0. && i >= l) continue;
				d[l] -= p; e[l] = g; e[m] = 0.;
			}
		} while (m != l);
	}
}


void output1(float *a, size_t n) {
	for (size_t x = 0; x < n; x++) printf("%4.1lf ", a[x]); puts("\n");
}

void output2(float **a, size_t n, size_t n1) {
	for (size_t y = 0; y < n; y++) {
		for (size_t x = 0; x < n1; x++) printf("%5.2lf ", a[y][x]); puts("");
	} puts("");
}

void sort(float **CArr, float *D0, float **W)
{
	float temp = 0;
	for (int i = 0; i < globCount; i++)
	{
		for (int j = 0; j < globCount - 1; j++)
		{
			if (D0[j] < D0[j + 1])
			{
				temp = D0[j];
				D0[j] = D0[j + 1];
				D0[j + 1] = temp;
				for (int k = 0; k < globCount; k++)
				{
					temp = CArr[k][j];
					CArr[k][j] = CArr[k][j + 1];
					CArr[k][j + 1] = temp;
				}

			}
		}
	}
	for (int i = 0; i < globCount; i++)
		for (int j = 0; j < globCount; j++)
		{
			if (i == j)
				W[i][j] = sqrt(fabs(D0[i]));
			else
				W[i][j] = 0;
		}
}

bool inverse(float **matrix, float **result, int size)
{
	for (int i = 0; i < size; ++i)
	{
		for (int j = 0; j < size; ++j)
			result[i][j] = 0.0;

		result[i][i] = 1.0;
	}

	double **copy = new double *[size]();

	for (int i = 0; i < size; ++i)
	{
		copy[i] = new double[size];

		for (int j = 0; j < size; ++j)
			copy[i][j] = matrix[i][j];
	}

	for (int k = 0; k < size; ++k)
	{
		if (fabs(copy[k][k]) < 1e-8)
		{
			bool changed = false;
			for (int i = k + 1; i < size; ++i)
			{
				if (fabs(copy[i][k]) > 1e-8)
				{
					std::swap(copy[k], copy[i]);
					std::swap(result[k], result[i]);
					changed = true;
					break;
				}
			}
			if (!changed)
			{
				for (int i = 0; i < size; ++i)
					delete[] copy[i];

				delete[] copy;
				return false;
			}
		}

		double div = copy[k][k];
		for (int j = 0; j < size; ++j)
		{
			copy[k][j] /= div;
			result[k][j] /= div;
		}
		for (int i = k + 1; i < size; ++i)
		{
			double multi = copy[i][k];
			for (int j = 0; j < size; ++j)
			{
				copy[i][j] -= multi * copy[k][j];
				result[i][j] -= multi * result[k][j];
			}
		}
	}
	for (int k = size - 1; k > 0; --k)
	{
		for (int i = k - 1; i + 1 > 0; --i)
		{
			double multi = copy[i][k];
			for (int j = 0; j < size; ++j)
			{
				copy[i][j] -= multi * copy[k][j];
				result[i][j] -= multi * result[k][j];
			}
		}
	}
	for (int i = 0; i < size; ++i)
		delete[] copy[i];

	delete[] copy;
	return true;
}

void V(float **CArr, double **Arr, float **TCArr2, float **W)
{
	cout << "--------------------------------------------------------------" << endl;
	cout << "ћј“–»÷ј V “–јЌ—ѕќЌ»–ќ¬јЌЌјя" << endl;
	cout << "--------------------------------------------------------------" << endl;
	float **TCArr = new float*[globCount];
	for (int i = 0; i < globCount; i++)
		TCArr[i] = new float[globCount];

	float **TCArr1 = new float*[globCount];
	for (int i = 0; i < globCount; i++)
		TCArr1[i] = new float[kolZagolovkov];


	bool a = inverse(CArr, TCArr, globCount);
	if (a != 0)
	{
		for (int i = 0; i < globCount; ++i)
			for (int j = 0; j < kolZagolovkov; ++j)
			{
				TCArr1[i][j] = 0;
				for (int k = 0; k < globCount; ++k)
					TCArr1[i][j] += (TCArr[i][k] * Arr[k][j]);
			}
	}
	for (int i = 0; i < globCount; i++)
		for (int j = 0; j < kolZagolovkov; j++)
			TCArr[i][j] = 0;
	bool b = inverse(W, TCArr, globCount - 3);


	for (int i = 0; i < globCount; ++i)
		for (int j = 0; j < kolZagolovkov; ++j)
		{
			TCArr2[i][j] = 0;
			for (int k = 0; k < globCount; ++k)
				TCArr2[i][j] += (TCArr[i][k] * TCArr1[k][j]);
		}
	output2(TCArr2, globCount, kolZagolovkov);
}

void KMeans(float **CArr, float **TCArr2)
{
	const int klaster = 3;
	float **N1 = new float*[klaster];
	for (int i = 0; i < kolZagolovkov; i++)
		N1[i] = new float[kolZagolovkov];

	float **N2 = new float*[klaster];
	for (int i = 0; i < globCount; i++)
		N2[i] = new float[globCount];

	float center[klaster][2] = { { 0, 0 }, { 1, 0 }, { 0, -1 } };
	float predcenter[klaster][2];
	float **rasstoyanie1 = new float*[3];
	for (int i = 0; i < 3; i++)
		rasstoyanie1[i] = new float[kolZagolovkov];

	float **rasstoyanie2 = new float*[3];
	for (int i = 0; i < 3; i++)
		rasstoyanie2[i] = new float[globCount];
	int i = 0, j = 0;
	while (center[0][0] != predcenter[0][0] && center[0][1] != predcenter[0][1] && center[1][0] != predcenter[1][0] &&
		center[1][1] != predcenter[1][1] && center[2][0] != predcenter[2][0] && center[2][1] != predcenter[2][1])
	{
		int c = 0;
		while (c < 3)
		{
			j = 0;
			while (i < globCount)
			{
				if (c == 0)
					rasstoyanie2[c][i] = sqrt(pow(CArr[i][j] - center[0][0], 2) + (pow(CArr[i][j + 1] - center[0][1], 2)));
				else if (c == 1)
					rasstoyanie2[c][i] = sqrt(pow(CArr[i][j] - center[1][0], 2) + (pow(CArr[i][j + 1] - center[1][1], 2)));
				else if (c == 2)
					rasstoyanie2[c][i] = sqrt(pow(CArr[i][j] - center[2][0], 2) + (pow(CArr[i][j + 1] - center[2][1], 2)));
				float ch1, ch2;
				ch1 = CArr[i][j]; ch2 = CArr[i][j + 1];
				i++;
			}
			i = 0;
			while (j < kolZagolovkov)
			{
				if (c == 0)
					rasstoyanie1[c][j] = sqrt(pow(TCArr2[i][j] - center[0][0], 2) + (pow(TCArr2[i + 1][j] - center[0][1], 2)));
				else if (c == 1)
					rasstoyanie1[c][j] = sqrt(pow(TCArr2[i][j] - center[1][0], 2) + (pow(TCArr2[i + 1][j] - center[1][1], 2)));
				else if (c == 2)
					rasstoyanie1[c][j] = sqrt(pow(TCArr2[i][j] - center[2][0], 2) + (pow(TCArr2[i + 1][j] - center[2][1], 2)));
				j++;
			}
			c++;
		}
		float min = 999;
		float x1 = 0, y1 = 0, x2 = 0, y2 = 0, x3 = 0, y3 = 0;
		int summ1 = 0, summ2 = 0, summ3 = 0, flag = -1;
		i = 0;
		while (i < globCount)
		{
			for (j = 0; j < klaster; j++)
			{
				if (rasstoyanie2[j][i] < min)
				{
					flag = j;
					min = rasstoyanie2[j][i];
				}
			}
			if (flag == 0)
			{
				summ1++;
				x1 += CArr[i][0];
				y1 += CArr[i][1];
			}
			if (flag == 1)
			{
				summ2++;
				x2 += CArr[i][0];
				y2 += CArr[i][1];
			}
			if (flag == 2)
			{
				x3 += CArr[i][0];
				y3 += CArr[i][1];
				summ3++;
			}
			i++;
			min = 999;
		}
		i = 0; min = 999;
		while (i < kolZagolovkov)
		{
			for (j = 0; j < klaster; j++)
			{
				if (rasstoyanie1[j][i] < min)
				{
					flag = j;
					min = rasstoyanie1[j][i];
				}
			}
			if (flag == 0)
			{
				summ1++;
				x1 += TCArr2[0][i];
				y1 += TCArr2[1][i];
			}
			if (flag == 1)
			{
				summ2++;
				x2 += TCArr2[0][i];
				y2 += TCArr2[1][i];
			}
			if (flag == 2)
			{
				x3 += TCArr2[0][i];
				y3 += TCArr2[1][i];
				summ3++;
			}
			i++;
			min = 999;
		}
		for (int i = 0; i < klaster; i++)
		{
			for (int j = 0; j < 2; j++)
				predcenter[i][j] = center[i][j];
		}
		for (int i = 0; i < klaster; i++)
		{
			if (i == 0) {
				center[i][0] = (x1 / summ1);
				center[i][1] = (y1 / summ1);
			}
			if (i == 1) {
				center[i][0] = (x2 / summ2);
				center[i][1] = (y2 / summ2);
			}
			if (i == 2) {
				center[i][0] = (x3 / summ3);
				center[i][1] = (y3 / summ3);
			}
		}
	}
	cout << "--------------------------------------------------------------" << endl;
	cout << "”—“ќ…„»¬џ≈ ÷≈Ќ“–џ  Ћј—“≈–ќ¬" << endl;
	cout << "--------------------------------------------------------------" << endl;
	for (int i = 0; i < klaster; i++)
	{
		for (int j = 0; j < 2; j++)
			printf("%4.2lf ", center[i][j]);
		cout << endl;
	}
	int q = 0; float min = 999; int flag = -1, k = 0;
	FILE *file;
	file = fopen("TOI3Klaster.txt", "w");
	fwrite(" ластер 1", sizeof(char), 9, file);
	fwrite("\n", sizeof(char), 1, file);
	while (k < klaster)
	{
		while (q < kolZagolovkov)
		{
			for (j = 0; j < klaster; j++)
			{
				if (rasstoyanie1[j][q] < min)
				{
					flag = j;
					min = rasstoyanie1[j][q];
				}
			}
			if ((k == 0) && (flag == 0))
			{
				fputs(Message[q].c_str(), file);
				fwrite("\n", sizeof(char), 1, file);
			}
			if ((k == 1) && (flag == 1))
			{
				fputs(Message[q].c_str(), file);
				fwrite("\n", sizeof(char), 1, file);
			}
			if ((k == 2) && (flag == 2))
			{
				fputs(Message[q].c_str(), file);
				fwrite("\n", sizeof(char), 1, file);
			}
			q++; min = 999;
		}
		q = 0; min = 999;
		k++;
		if (k == 1)
			fwrite(" ластер 2", sizeof(char), 9, file);
		if (k == 2)
			fwrite(" ластер 3", sizeof(char), 9, file);
		fwrite("\n", sizeof(char), 1, file);
	}
}

int main()
{
	SetConsoleCP(1251);
	SetConsoleOutputCP(1251);
	string fName, StemmWord;
	int flag = 0;
	cout << "¬ведите им€ файла без расширени€" << endl;
	cin >> fName;
	fName = fName + ".txt";
	file_inp(fName, Message, flag);
	fName = "STOP.txt";
	flag++;
	file_inp(fName, STOP, flag);
	//DelSTOP();
	AllWord();
	string LSA[20];
	double **Arr = new double*[20];
	for (int i = 0; i < 20; i++)
		Arr[i] = new double[kolZagolovkov];
	for (int i = 0; i < 20; i++)
	{
		for (int j = 0; j < kolZagolovkov; j++)
			Arr[i][j] = 0;
	}
	AloneWord(LSA, Arr);
	double **TArr = new double*[kolZagolovkov];
	for (int i = 0; i < kolZagolovkov; i++)
		TArr[i] = new double[globCount];
	float **CArr = new float*[globCount];
	for (int i = 0; i < globCount; i++)
		CArr[i] = new float[globCount];
	Transp(Arr, TArr, CArr);
	float *D0 = new float[globCount];
	float *D1 = new float[globCount];
	for (int i = 0; i < globCount; i++)
	{
		D0[i] = 0; D1[i] = 0;
	}
	tred2(CArr, globCount - 1, D0, D1);
	tqli(D0, D1, globCount - 1, CArr);
	cout << "--------------------------------------------------------------" << endl;
	cout << "—ќЅ—“¬≈ЌЌџ≈ «Ќј„≈Ќ»я" << endl;
	cout << "--------------------------------------------------------------" << endl;
	cout << endl;
	output1(D0, globCount);
	cout << "--------------------------------------------------------------" << endl;
	cout << "—ќЅ—“¬≈ЌЌџ≈ ¬≈ “ќ–ј" << endl;
	cout << "--------------------------------------------------------------" << endl;
	cout << endl;
	output2(CArr, globCount, globCount);
	float **W = new float*[globCount];
	for (int i = 0; i < globCount; i++)
		W[i] = new float[globCount];
	cout << "--------------------------------------------------------------" << endl;
	sort(CArr, D0, W);
	output2(W, globCount, globCount);
	output2(CArr, globCount, globCount);
	float **TCArr2 = new float*[globCount];
	for (int i = 0; i < kolZagolovkov; i++)
		TCArr2[i] = new float[kolZagolovkov];
	V(CArr, Arr, TCArr2, W);
	KMeans(CArr, TCArr2);
	system("pause");
}
