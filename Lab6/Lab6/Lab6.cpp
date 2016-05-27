// Lab6.cpp: ���������� ����� ����� ��� ����������� ����������.
//

#include "stdafx.h"
#include <Windows.h>
#include <iostream>
#include <queue>

using namespace std;

CRITICAL_SECTION criticalSection;
CRITICAL_SECTION consumerCriticalSection;
CRITICAL_SECTION producerCriticalSection;

CRITICAL_SECTION philosopherCriticalSection;

int consumedProductsCount = 0;// ���������� ����������� �������.
int producedProductsCount = 0;// ���������� ������������ �������.
int functionCallCount = 0; // ���������� ������� ������� � ������.
queue<LPVOID> producerConsumerQueue = queue<LPVOID>(); // ������� ��� �������������� � ������������.

HANDLE* forks = new HANDLE[5]; // ����� ��� ���������.

DWORD WINAPI FirstFunction(LPVOID arg)
{
	EnterCriticalSection(&criticalSection);
	functionCallCount++;
	LeaveCriticalSection(&criticalSection);

	cout << "Thread " << (int)arg << " has started." << endl;
	for (int j = 0; j < 300000000; j++) {}
	cout << "Thread " << (int)arg << " has stopped." << endl;
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

	cout << "First function has been called " << functionCallCount << " times" << endl;
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
	cout << "Maximum count of threads: " << threadsCount << endl;
}


// ������� ����� �������� ��������.
DWORD WINAPI PhilosopherFunction(LPVOID arg)
{
	int philosopherNumber = (int)arg;
	int leftForkNumber = philosopherNumber - 1;
	int rightForkNumber = (philosopherNumber - 2) % 5; // ��� ������ �������� ��� ������ �����, �� ������ ����� ������.

	int firstForkNumber = 0;
	int secondForkNumber = 0;
	char* firstFork = "";
	char* secondFork = "";
	if (leftForkNumber < rightForkNumber)
	{
		firstForkNumber = leftForkNumber;
		secondForkNumber = rightForkNumber;
		firstFork = "left";
		secondFork = "right";
	}
	else
	{
		firstForkNumber = rightForkNumber;
		secondForkNumber = leftForkNumber;
		firstFork = "right";
		secondFork = "left";
	}

	// ������������� ������ 
	while (true)
	{
		EnterCriticalSection(&philosopherCriticalSection);
		cout << "Philosopher " << philosopherNumber << " is thinking." << endl;
		LeaveCriticalSection(&philosopherCriticalSection);

		WaitForSingleObject(forks[firstForkNumber], INFINITY);
		EnterCriticalSection(&philosopherCriticalSection);
		cout << "Philosopher " << philosopherNumber << " is taking " << firstFork << " fork." << endl;
		//forks[firstForkNumber] = CreateEvent(NULL, TRUE, TRUE, TEXT(""));
		LeaveCriticalSection(&philosopherCriticalSection);

		WaitForSingleObject(forks[secondForkNumber], INFINITY);
		EnterCriticalSection(&philosopherCriticalSection);
		cout << "Philosopher " << philosopherNumber << " is taking " << secondFork << " fork." << endl;
		//forks[secondForkNumber] = CreateEvent(NULL, TRUE, TRUE, TEXT(""));
		LeaveCriticalSection(&philosopherCriticalSection);

		EnterCriticalSection(&philosopherCriticalSection);
		cout << "Philosopher " << philosopherNumber << " is eating." << endl;
		LeaveCriticalSection(&philosopherCriticalSection);

		EnterCriticalSection(&philosopherCriticalSection);
		cout << "Philosopher " << philosopherNumber << " is putting down " << secondFork << " fork." << endl;
		ReleaseSemaphore(forks[secondForkNumber], 1, 0);
		//SetEvent(forks[secondForkNumber]);
		LeaveCriticalSection(&philosopherCriticalSection);

		EnterCriticalSection(&philosopherCriticalSection);
		cout << "Philosopher " << philosopherNumber << " is putting down " << firstFork << " fork." << endl;
		ReleaseSemaphore(forks[firstForkNumber], 1, 0);
		//SetEvent(forks[firstForkNumber]);
		LeaveCriticalSection(&philosopherCriticalSection);
		Sleep(1000);
	}
	return 0;
}

void PhilosophersTask()
{
	HANDLE* philosophers = new HANDLE[5];
	InitializeCriticalSection(&philosopherCriticalSection);

	// ������������� �����.
	for (int i = 0; i < 5; i++)
	{
		forks[i] = CreateSemaphore(0, 1, 1, 0);
	}
	// ������������� ���������.
	for (int i = 0; i < 5; i++)
	{
		philosophers[i] = CreateThread(0, 0, PhilosopherFunction, (LPVOID)(i + 1), 0, NULL);
	}

	Sleep(3000);
	//WaitForMultipleObjects(5, philosophers, TRUE, INFINITE);

	for (int i = 0; i < 10; i++)
	{
		CloseHandle(philosophers[i]);
	}
}

int main()
{
	//FirstTask();
	//SecondTask();
	//ProducerConsumerTask(10, 100);
	PhilosophersTask();
    return 0;
}

