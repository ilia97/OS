// Lab6.cpp: определяет точку входа для консольного приложения.
//

#include "stdafx.h"
#include <Windows.h>
#include <queue>

using namespace std;

HANDLE firstTaskMutex;
CRITICAL_SECTION criticalSection;
CRITICAL_SECTION consumerCriticalSection;
CRITICAL_SECTION producerCriticalSection;

int consumedProductsCount = 0;// Количество потреблённых товаров.
int producedProductsCount = 0;// Количество произведённых товаров.
int functionCallCount = 0; // Количество вызовов функции в потоке.
queue<LPVOID> producerConsumerQueue = queue<LPVOID>(); // Очередь для производителей и потребителей.


DWORD WINAPI FirstFunction(LPVOID arg)
{
	// Если мьютекс с таким именем уже создан, то новый не создайтся, а возврщается уже существующий.
	firstTaskMutex = CreateMutex(NULL, true, L"mutex");

#ifdef _DEBUG
	HANDLE hFile = CreateFile(_T("First.txt"), GENERIC_WRITE, FILE_SHARE_WRITE, 0, OPEN_ALWAYS, 0, 0);
	SetFilePointer(hFile, 0, 0, FILE_END);
	DWORD dwCount = 0;
	TCHAR* temp = TEXT(" thread has started.\r\n");
	TCHAR* buf = new TCHAR[10];
	_stprintf_s(buf, 10, TEXT("%d"), (int)arg);
	WriteFile(hFile, buf, (DWORD)4, &dwCount, 0);
	WriteFile(hFile, temp, sizeof(TCHAR)*_tcslen(temp), &dwCount, 0);
	CloseHandle(hFile);

	for (int j = 0; j < 600000000; j++) {}

	hFile = CreateFile(_T("First.txt"), GENERIC_WRITE, FILE_SHARE_WRITE, 0, OPEN_ALWAYS, 0, 0);
	SetFilePointer(hFile, 0, 0, FILE_END);
	dwCount = 0;
	temp = TEXT(" thread has stopped.\r\n");
	WriteFile(hFile, buf, (DWORD)4, &dwCount, 0);
	WriteFile(hFile, temp, sizeof(TCHAR)*_tcslen(temp), &dwCount, 0);
	CloseHandle(hFile);
#else
	_tprintf(_T("Thread %d has started.\n"), (int)arg);
	for (int j = 0; j < 60000000; j++) {}
	_tprintf(_T("Thread %d has stopped.\n"), (int)arg);
#endif // DEBUG

	functionCallCount++;
	ReleaseMutex(firstTaskMutex);
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

	// Инициализация потоков.
	for (int i = 0; i < 10; i++)
	{
		handles[i] = CreateThread(0, 0, FirstFunction, (LPVOID)(i + 1), 0, NULL);
	}

	// Ожидаем завершения всех потоков (FALSE для одного).
	WaitForMultipleObjects(10, handles, FALSE, INFINITE);

	for (int i = 0; i < 10; i++)
	{
		CloseHandle(handles[i]);
	}

	_tprintf(_T("First function has been called %d times.\n"), functionCallCount);
}

// threadsCount - Количество потоков.
void SecondTask()
{
	int threadsCount = 2;
	while(true)
	{
		functionCallCount = 0;

		HANDLE* handles = new HANDLE[threadsCount];
		InitializeCriticalSection(&criticalSection);

		// Инициализация потоков.
		for (int i = 0; i < threadsCount; i++)
		{
			handles[i] = CreateThread(0, 0, SecondFunction, (LPVOID)(i + 1), 0, NULL);
		}

		// Ожидаем завершения всех потоков (FALSE для одного).
		WaitForMultipleObjects(threadsCount, handles, FALSE, INFINITE);

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