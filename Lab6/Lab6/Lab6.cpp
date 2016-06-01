// Lab6.cpp: ���������� ����� ����� ��� ����������� ����������.
//

#include "stdafx.h"
#include <Windows.h>
#include <queue>

using namespace std;

HANDLE firstTaskMutex;
CRITICAL_SECTION criticalSection;
CRITICAL_SECTION consumerCriticalSection;
CRITICAL_SECTION producerCriticalSection;

int consumedProductsCount = 0;// ���������� ����������� �������.
int producedProductsCount = 0;// ���������� ������������ �������.
int functionCallCount = 0; // ���������� ������� ������� � ������.
queue<LPVOID> producerConsumerQueue = queue<LPVOID>(); // ������� ��� �������������� � ������������.


DWORD WINAPI FirstFunction(LPVOID arg)
{
	// ���� ������� � ����� ������ ��� ������, �� ����� �� ���������, � ����������� ��� ������������.
	firstTaskMutex = CreateMutex(NULL, true, L"mutex");
	functionCallCount++;
	ReleaseMutex(firstTaskMutex);

	_tprintf(_T("Thread %d has started.\n"), (int)arg);
	for (int j = 0; j < 600000000; j++) {}
	_tprintf(_T("Thread %d has stopped.\n"), (int)arg);
	return 0;
}

DWORD WINAPI SecondFunction(LPVOID arg)
{
	EnterCriticalSection(&criticalSection);
	functionCallCount++;
	LeaveCriticalSection(&criticalSection);
	return 0;
}

void FirstTask()
{
	HANDLE* handles = new HANDLE[10];
	InitializeCriticalSection(&criticalSection);

	// ������������� �������.
	for (int i = 0; i < 10; i++)
	{
		handles[i] = CreateThread(0, 0, FirstFunction, (LPVOID)(i + 1), 0, NULL);
	}

	// ������� ���������� ���� ������� (FALSE ��� ������).
	WaitForMultipleObjects(10, handles, TRUE, INFINITE);

	for (int i = 0; i < 10; i++)
	{
		CloseHandle(handles[i]);
	}

	_tprintf(_T("First function has been called %d times.\n"), functionCallCount);
}

// threadsCount - ���������� �������.
void SecondTask()
{
	int threadsCount = 2;
	while(true)
	{
		functionCallCount = 0;

		HANDLE* handles = new HANDLE[threadsCount];
		InitializeCriticalSection(&criticalSection);

		// ������������� �������.
		for (int i = 0; i < threadsCount; i++)
		{
			handles[i] = CreateThread(0, 0, SecondFunction, (LPVOID)(i + 1), 0, NULL);
		}

		// ������� ���������� ���� ������� (FALSE ��� ������).
		WaitForMultipleObjects(threadsCount, handles, TRUE, INFINITE);

		for (int i = 0; i < threadsCount; i++)
		{
			CloseHandle(handles[i]);
		}

		if (functionCallCount != threadsCount)
		{
			break;
		}

		threadsCount *= 2;
	}
	_tprintf(_T("Maximum count of threads: %d.\n"), threadsCount);
}

int main()
{
	FirstTask();
	//SecondTask();
    return 0;
}