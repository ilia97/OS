// ProducerConsumer.cpp: ���������� ����� ����� ��� ����������� ����������.
//

#include "stdafx.h"
#include <Windows.h>
#include <iostream>
#include <queue>
#include <vector>

using namespace std;

int filesCount = 20;
int consumedProductsCount = 0;// ���������� ����������� �������.
int producedProductsCount = 0;// ���������� ������������ �������.
queue<wchar_t*> producerConsumerQueue = queue<wchar_t*>(); // ������� ��� �������������� � ������������.
CRITICAL_SECTION consumerCriticalSection;
CRITICAL_SECTION producerCriticalSection;
vector<wchar_t*> fileNames = vector<wchar_t*>();

TCHAR* GetFileText(wchar_t* fileName)
{
	wchar_t path[100] = L"Files\\";
	wcscat_s(path, 100, (const wchar_t*)fileName);

	FILE *file;

	// ���������, ����� ��������� � ����.
	_wfopen_s(&file, path, L"rt");
	// ������ ���� �����.
	char BOM1 = fgetc(file);
	// ������ ���� �����.
	char BOM2 = fgetc(file);
	// ������ ���� �����.
	char BOM3 = fgetc(file);
	fclose(file);

	// ������ ��� UTF-16LE
	if (BOM1 == '\xff' && BOM2 == '\xfe')
	{
		_wfopen_s(&file, path, L"rt+, ccs=UTF-16LE");
		wchar_t text[30000] = L"";
		for (int i = 0; !feof(file); i++)
		{
			wchar_t w = fgetwc(file);
			text[i] = w;
		}
		fclose(file);
		return text;
	}
	// UTF-8
	else if (BOM1 == '\xef' && BOM2 == '\xbb' && BOM3 == '\xbf')
	{
		_wfopen_s(&file, path, L"rt+, ccs=UTF-8");
		wchar_t text[30000] = L"";
		for (int i = 0; !feof(file); i++)
		{
			wchar_t w = fgetwc(file);
			text[i] = w;
		}
		fclose(file);
		return text;
	}
	// ������ ��� ASCII
	else
	{
		_wfopen_s(&file, path, L"rt");
		wchar_t text[30000] = L"";
		while (!feof(file))
		{
			char str1[256] = "";
			fgets(str1, 256, file);

			int size = strlen(str1);
			wchar_t* wstr = new wchar_t[size + 1];
			size_t convertedChars = 0;
			mbstowcs_s(&convertedChars, wstr, size + 1, str1, _TRUNCATE);
			wcscat_s(text, wstr);
		}
		fclose(file);
		return text;
	}

	return L"";
}

DWORD WINAPI ProducerFunction(LPVOID arg)
{
	while (true)
	{
		EnterCriticalSection(&producerCriticalSection);

		// �������� ���������� ����� ����� � ����������� ������.
		// ��� ������ ���� ������������ ���� ������. ���� ��� ������������, 
		//�� ������� ��� �� �����������, �� ������� exception.
		if (producedProductsCount < filesCount)
		{
			producerConsumerQueue.push(fileNames[producedProductsCount]);
			wprintf(L"Producer %d has added file \"%s\".\n", (int)arg, fileNames[producedProductsCount]);
			producedProductsCount++;
		}
		else
		{
			// ���������� ��� ����, ����� ����� �� ����������� ������ ���������� � ����� ������.
			LeaveCriticalSection(&producerCriticalSection);
			break;
		}

		LeaveCriticalSection(&producerCriticalSection);
		
		Sleep(100 + rand() % (400));
	}
	return 0;
}

DWORD WINAPI ConsumerFunction(LPVOID arg)
{
	// ������� ���� ��� ����, ����� ����������� �� ���������� ��������.
	while (consumedProductsCount < filesCount)
	{
		while (consumedProductsCount < producedProductsCount)
		{
			EnterCriticalSection(&consumerCriticalSection);
			if (!producerConsumerQueue.empty())
			{
				wchar_t* fileName = producerConsumerQueue.front();
				producerConsumerQueue.pop();
				TCHAR* text = GetFileText(fileName);
				wprintf(L"Consumer %d... %s: %s\n", (int)arg, fileName, text);
				consumedProductsCount++;
			}
			LeaveCriticalSection(&consumerCriticalSection);

			Sleep(100 + rand() % (500 - 100));
		}
	}
	return 0;
}

void GetFiles()
{
	fileNames.push_back(L"1.txt");
	fileNames.push_back(L"2.txt");
	fileNames.push_back(L"3.txt");
	fileNames.push_back(L"4.txt");
	fileNames.push_back(L"5.txt");
	fileNames.push_back(L"6.txt");
	fileNames.push_back(L"7.txt");
	fileNames.push_back(L"8.txt");
	fileNames.push_back(L"9.txt");
	fileNames.push_back(L"10.txt");
	fileNames.push_back(L"11.txt");
	fileNames.push_back(L"12.txt");
	fileNames.push_back(L"13.txt");
	fileNames.push_back(L"14.txt");
	fileNames.push_back(L"15.txt");
	fileNames.push_back(L"16.txt");
	fileNames.push_back(L"17.txt");
	fileNames.push_back(L"18.txt");
	fileNames.push_back(L"19.txt");
	fileNames.push_back(L"20.txt");
}

// ��������� - ���������� ������� ����������� � �������������.
void ProducerConsumerTask(int producerThreadsCount, int consumerThredsCount)
{
	// �������� ����� � ���������� ������.
	GetFiles();

	HANDLE* producerHandles = new HANDLE[producerThreadsCount];
	HANDLE* consumerHandles = new HANDLE[consumerThredsCount];
	InitializeCriticalSection(&consumerCriticalSection);
	InitializeCriticalSection(&producerCriticalSection);

	// ������������� ��������������.
	for (int i = 0; i < producerThreadsCount; i++)
	{
		producerHandles[i] = CreateThread(0, 0, ProducerFunction, (LPVOID)(i + 1), 0, NULL);
	}

	// ������������� ������������.
	for (int i = 0; i < consumerThredsCount; i++)
	{
		consumerHandles[i] = CreateThread(0, 0, ConsumerFunction, (LPVOID)(i + 1), 0, NULL);
	}

	WaitForMultipleObjects(consumerThredsCount, consumerHandles, TRUE, INFINITE);

	for (int i = 0; i < producerThreadsCount; i++)
	{
		CloseHandle(producerHandles[i]);
	}
	for (int i = 0; i < consumerThredsCount; i++)
	{
		CloseHandle(consumerHandles[i]);
	}

	delete[] producerHandles;
	delete[] consumerHandles;
}


int main()
{
	// �������� ������������� � �����������.
	// ������������� �� ����������, ���� ����� �����.
	// ����������� �� ����������, ���� ���������� ������.
	ProducerConsumerTask(5, 5);
    return 0;
}

