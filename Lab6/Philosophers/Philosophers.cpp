// Philosophers.cpp: ���������� ����� ����� ��� ����������� ����������.
//

#include "stdafx.h"
#include <Windows.h>

CRITICAL_SECTION philosopherCriticalSection;
HANDLE* forks = new HANDLE[5]; // ����� ��� ���������.

void Wait()
{
	Sleep(100 + rand() % (500 - 100));
}

void Think(int philosopherNumber)
{
	_tprintf(_T("Philosopher %d is thinking.\n"), philosopherNumber);
	Wait();
}

void Take(int philosopherNumber, int forkNumber)
{
	_tprintf(_T("Philosopher %d is taking %d fork.\n"), philosopherNumber, forkNumber);
}

void Eat(int philosopherNumber)
{
	_tprintf(_T("Philosopher %d is eating.\n"), philosopherNumber);
	Wait();
}

void Put(int philosopherNumber, int forkNumber)
{
	_tprintf(_T("Philosopher %d is putting down %d fork.\n"), philosopherNumber, forkNumber);
}

// ������� ����� �������� ��������.
DWORD WINAPI PhilosopherFunction(LPVOID arg)
{
	int philosopherNumber = (int)arg;
	int leftForkNumber = philosopherNumber % 5;
	int rightForkNumber = (philosopherNumber - 1) % 5; // ��� ������ �������� ��� ������ �����, �� ������ ����� ������.



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

	while (true)
	{
		Think(philosopherNumber);
		WaitForSingleObject(forks[firstForkNumber], INFINITE);
		Take(philosopherNumber, firstForkNumber);
		WaitForSingleObject(forks[secondForkNumber], INFINITE);
		Take(philosopherNumber, secondForkNumber);
		Eat(philosopherNumber);
		Put(philosopherNumber, secondForkNumber);
		ReleaseMutex(forks[secondForkNumber]);
		Put(philosopherNumber, firstForkNumber);
		ReleaseMutex(forks[firstForkNumber]);
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
		forks[i] = CreateMutex(0, false, NULL);
	}
	// ������������� ���������.
	for (int i = 0; i < 5; i++)
	{
		philosophers[i] = CreateThread(0, 0, PhilosopherFunction, (LPVOID)(i + 1), 0, NULL);
	}

	Sleep(5000);
	//WaitForMultipleObjects(5, philosophers, TRUE, INFINITE);

	for (int i = 0; i < 5; i++)
	{
		CloseHandle(philosophers[i]);
	}
}

int main()
{
	PhilosophersTask();
    return 0;
}

