// StaticConnection.cpp: определяет точку входа для консольного приложения.
//

#include "stdafx.h"
#include <iostream>
#include <vector>
#include <math.h>

using namespace std;

_declspec(dllimport) void __stdcall LongNumberRSAencryption(vector<int> number, vector<int>* result, vector<int>* key, vector<int>* outN);
_declspec(dllimport) void __stdcall LongNumberRSAdecryption(vector<int> number, vector<int>* key, vector<int>* n, vector<int>* result);
_declspec(dllimport) void __stdcall NumberRSAdecryption(unsigned long number, unsigned long* key, unsigned long* n, unsigned long* result);
_declspec(dllimport) void __stdcall NumberRSAencryption(unsigned long number, unsigned long* result, unsigned long* key, unsigned long* outN);
_declspec(dllimport) vector<vector<int>> __stdcall StringRSAencryption(TCHAR str[], unsigned long strLen, vector<int>* key, vector<int>* outN);
_declspec(dllimport) void __stdcall StringRSAdecryption(vector<vector<int>> str, unsigned long strLen, vector<int>* key, vector<int>* outN, TCHAR result[]);

int main()
{	
	// Устанавливаем локальный режим для того, чтобы выводить символы кирилицы.
	_tsetlocale(LC_ALL, _T("Russian"));

	// BAD ENCRYPTOR
	// Максимальное значение - 3149.
	/*unsigned long number = 3149;
	unsigned long result;
	unsigned long key;
	unsigned long outN;

	NumberRSAencryption(number, &result, &key, &outN);

	cout << "Your number was encrypted successfully." << endl;
	cout << "Result:" << result << endl;
	cout << "Key:" << key << endl;
	cout << "N:" << outN << endl;

	NumberRSAdecryption(result, &key, &outN, &number);

	cout << "Your number was decrypted successfully." << endl;
	cout << "Result:" << number << endl;*/

	// NUMBER ENCRIPTION
	vector<int> number = { 9,7,3,4,5,5,6 };
	vector<int> result;
	vector<int> result2;
	vector<int> key;
	vector<int> outN;

	cout << "Encryption of number ";
	for (int i = 0; i < number.size(); i++)
	{
		cout << number[i];
	}
	cout << "" << endl;

	LongNumberRSAencryption(number, &result, &key, &outN);

	cout << "Your number was encrypted successfully." << endl;
	cout << "Result:";
	for (int i = 0; i < result.size(); i++)
	{
		cout << result[i];
	}
	cout << "" << endl;
	cout << "Key:";
	for (int i = 0; i < key.size(); i++)
	{
		cout << key[i];
	}
	cout << "" << endl;
	cout << "N:";
	for (int i = 0; i < outN.size(); i++)
	{
		cout << outN[i];
	}
	cout << "" << endl;

	LongNumberRSAdecryption(result, &key, &outN, &result2);

	cout << "Your number was decrypted successfully." << endl;
	cout << "Result:";
	for (int i = 0; i < result2.size(); i++)
	{
		cout << result2[i];
	}
	cout << "\n" << endl;


	// STRING ENCRIPTION.
	//TCHAR str[] = _T("APяяSя");
	TCHAR str[] = _T("387457349875390287402980837209874377466573645798364987623498762374");
	unsigned long strLen = 67;
	vector<vector<int>> stringResult;
	vector<int> stringKey;
	vector<int> stringOutN;

	_tprintf(_TEXT("Encription of string: %ws\n"), str);

	stringResult = StringRSAencryption(str, strLen, &stringKey, &stringOutN);

	cout << "Your string was encrypted successfully." << endl;
	cout << "Result:" << endl;
	for (int i = 0; i < stringResult.size(); i++)
	{
		for (int j = 0; j < stringResult[i].size(); j++)
		{
			cout << stringResult[i][j];
		}
		cout << endl;
	}
	cout << "Key:";
	for (int i = 0; i < stringKey.size(); i++)
	{
		cout << stringKey[i];
	}
	cout << "" << endl;
	cout << "N:";
	for (int i = 0; i < stringOutN.size(); i++)
	{
		cout << stringOutN[i];
	}
	cout << "" << endl;

	TCHAR str2[256] = L"";
	StringRSAdecryption(stringResult, strLen, &stringKey, &stringOutN, str2);

	cout << "Your string was decrypted successfully." << endl;
	_tprintf(_TEXT("Result: %ws\n"), str2);

    return 0;
}

