// DynamicConnection.cpp: определяет точку входа для консольного приложения.
//

#include "stdafx.h"
#include <Windows.h>
#include <iostream>
#include <vector>

using namespace std;

TCHAR libName[] = _T("Dll.dll");
typedef void __stdcall NumberRSAencryption(unsigned long number, unsigned long* result, unsigned long* key, unsigned long* outN);
typedef void __stdcall NumberRSAdecryption(unsigned long number, unsigned long* key, unsigned long* n, unsigned long* result);
typedef void __stdcall LongNumberRSAencryption(vector<int> number, vector<int>* result, vector<int>* key, vector<int>* outN);
typedef void __stdcall LongNumberRSAdecryption(vector<int> number, vector<int>* key, vector<int>* n, vector<int>* result);
typedef vector<vector<int>> __stdcall StringRSAencryption(TCHAR str[], unsigned long strLen, vector<int>* key, vector<int>* outN);
typedef void __stdcall StringRSAdecryption(vector<vector<int>> str, unsigned long strLen, vector<int>* key, vector<int>* outN, TCHAR result[]);

NumberRSAencryption* numberRSAencryptionAddress;
NumberRSAdecryption* numberRSAdecryptionAddress;
LongNumberRSAencryption* longNumberRSAencryptionAddress;
LongNumberRSAdecryption* longNumberRSAdecryptionAddress;
StringRSAencryption* stringRSAencryptionAddress;
StringRSAdecryption* stringRSAdecryptionAddress;

TCHAR greetingMessage[256];
TCHAR encryptionSuccessMessage[256];
TCHAR resultMessage[256];
TCHAR mainKeyMessage[256];
TCHAR additionalKeyMessage[256];
TCHAR decryptionSuccessMessage[256];

int main()
{
	// Устанавливаем локальный режим для того, чтобы выводить символы кирилицы.
	_tsetlocale(LC_ALL, _T("Russian"));

	HMODULE h = LoadLibrary(libName); 

	if (h == NULL)
	{
		_tprintf(_T("No main library.\n"));
		return -1;
	}

	printf("If yot would like to get an english version of the program, enter \"1\".\n");
	printf("If yot would like to get a russian version of the program, enter \"2\".\n");

	char language = getchar();

	if (language == '2')
	{
		HMODULE russianLibrary = LoadLibrary(_T("RussianLibrary.dll"));
		if (russianLibrary == NULL)
		{
			_tprintf(_T("No russian library.\n"));
			return -1;
		}
		else
		{
			LoadString(russianLibrary, 101, greetingMessage, 256);
			LoadString(russianLibrary, 102, encryptionSuccessMessage, 256);
			LoadString(russianLibrary, 103, resultMessage, 256);
			LoadString(russianLibrary, 104, mainKeyMessage, 256);
			LoadString(russianLibrary, 105, additionalKeyMessage, 256);
			LoadString(russianLibrary, 106, decryptionSuccessMessage, 256);
		}
		FreeLibrary(russianLibrary);
	}
	else if (language == '1')
	{
		HMODULE englishLibrary = LoadLibrary(_T("EnglishLibrary.dll"));
		if (englishLibrary == NULL)
		{
			_tprintf(_T("No english library.\n"));
			return -1;
		}
		else
		{
			LoadString(englishLibrary, 101, greetingMessage, 256);
			LoadString(englishLibrary, 102, encryptionSuccessMessage, 256);
			LoadString(englishLibrary, 103, resultMessage, 256);
			LoadString(englishLibrary, 104, mainKeyMessage, 256);
			LoadString(englishLibrary, 105, additionalKeyMessage, 256);
			LoadString(englishLibrary, 106, decryptionSuccessMessage, 256);
		}
		FreeLibrary(englishLibrary);
	}

	// Сложная шифровка числа.
	vector<int> longNumber = { 1, 0, 7 };
	vector<int> longResult;
	vector<int> longKey;
	vector<int> longOutN;
	longNumberRSAencryptionAddress = (LongNumberRSAencryption*)GetProcAddress(h, "LongNumberRSAencryption");
	if (longNumberRSAencryptionAddress != 0)
	{
		_tprintf(greetingMessage);
		for (int i = 0; i < longNumber.size(); i++)
		{
			cout << longNumber[i];
		}
		cout << "" << endl;

		longNumberRSAencryptionAddress(longNumber, &longResult, &longKey, &longOutN);

		_tprintf(encryptionSuccessMessage);

		_tprintf(resultMessage);
		for (int i = 0; i < longResult.size(); i++)
		{
			cout << longResult[i];
		}
		cout << "" << endl;
		_tprintf(mainKeyMessage);
		for (int i = 0; i < longKey.size(); i++)
		{
			cout << longKey[i];
		}
		cout << "" << endl;
		_tprintf(additionalKeyMessage);
		for (int i = 0; i < longOutN.size(); i++)
		{
			cout << longOutN[i];
		}
		cout << "" << endl;

		longNumberRSAdecryptionAddress = (LongNumberRSAdecryption*)GetProcAddress(h, "LongNumberRSAdecryption");
		if (longNumberRSAdecryptionAddress != 0)
		{
			longNumberRSAdecryptionAddress(longResult, &longKey, &longOutN, &longNumber);

			_tprintf(decryptionSuccessMessage);
			_tprintf(resultMessage);
			for (int i = 0; i < longNumber.size(); i++)
			{
				cout << longNumber[i];
			}
			cout << "\n" << endl;
		}
		else
		{
			cout << "\"LongNumberRSAdecryption\" function wasn't found." << endl;
		}
	}
	else
	{
		cout << "\"LongNumberRSAencryption\" function wasn't found." << endl;
	}

	//Сложная шифровка строки.
	TCHAR str[] = _T("IliaВаня");
	unsigned long strLen = 8;
	vector<vector<int>> stringResult;
	vector<int> stringKey;
	vector<int> stringOutN;

	stringRSAencryptionAddress = (StringRSAencryption*)GetProcAddress(h, "StringRSAencryption");
	if (stringRSAencryptionAddress != 0)
	{
		_tprintf(greetingMessage);
		_tprintf(_TEXT("%ws\n"), str);

		stringResult = stringRSAencryptionAddress(str, strLen, &stringKey, &stringOutN);

		_tprintf(encryptionSuccessMessage);
		_tprintf(resultMessage);
		for (int i = 0; i < stringResult.size(); i++)
		{
			for (int j = 0; j < stringResult[i].size(); j++)
			{
				cout << stringResult[i][j];
			}
			cout << endl;
		}
		_tprintf(mainKeyMessage);
		for (int i = 0; i < stringKey.size(); i++)
		{
			cout << stringKey[i];
		}
		cout << "" << endl;
		_tprintf(additionalKeyMessage);
		for (int i = 0; i < stringOutN.size(); i++)
		{
			cout << stringOutN[i];
		}
		cout << "" << endl;

		stringRSAdecryptionAddress = (StringRSAdecryption*)GetProcAddress(h, "StringRSAdecryption");
		if (stringRSAdecryptionAddress != 0)
		{
			TCHAR str2[256] = L"";
			stringRSAdecryptionAddress(stringResult, strLen, &stringKey, &stringOutN, str2);

			_tprintf(decryptionSuccessMessage);
			_tprintf(resultMessage);
			_tprintf(_TEXT("%ws\n"), str2);
		}
		else
		{
			cout << "\"StringRSAdecryption\" function wasn't found." << endl;
		}
	}
	else
	{
		cout << "\"StringRSAencryption\" function wasn't found." << endl;
	}
	FreeLibrary(h);
    return 0;
}

