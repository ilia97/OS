// ProducerConsumer.cpp: определяет точку входа для консольного приложения.
//

#include "stdafx.h"
#include <Windows.h>
#include <iostream>
#include <queue>

using namespace std;

int consumedProductsCount = 0;// Количество потреблённых товаров.
int producedProductsCount = 0;// Количество произведённых товаров.
queue<LPVOID> producerConsumerQueue = queue<LPVOID>(); // Очередь для производителей и потребителей.
CRITICAL_SECTION consumerCriticalSection;
CRITICAL_SECTION producerCriticalSection;

DWORD WINAPI ProducerFunction(LPVOID arg)
{
	EnterCriticalSection(&producerCriticalSection);
	producerConsumerQueue.push(arg);
	producedProductsCount++;
	LeaveCriticalSection(&producerCriticalSection);
	return 0;
}

DWORD WINAPI ConsumerFunction(LPVOID arg)
{
	// Внешний цикл для того, чтобы потребитель не переставал работать.
	while (true)
	{
		while (consumedProductsCount < producedProductsCount)
		{
			EnterCriticalSection(&consumerCriticalSection);
			if (!producerConsumerQueue.empty())
			{
				LPVOID c = producerConsumerQueue.front();
				producerConsumerQueue.pop();
				cout << (int)c << endl;
				consumedProductsCount++;
			}
			LeaveCriticalSection(&consumerCriticalSection);
		}
	}
	return 0;
}

// Параметры - количество потоков потребителя и производителя.
void ProducerConsumerTask(int producerThreadsCount, int consumerThredsCount)
{
	HANDLE* producerHandles = new HANDLE[producerThreadsCount];
	HANDLE* consumerHandles = new HANDLE[consumerThredsCount];
	InitializeCriticalSection(&consumerCriticalSection);
	InitializeCriticalSection(&producerCriticalSection);

	// Инициализация производителей.
	for (int i = 0; i < producerThreadsCount; i++)
	{
		producerHandles[i] = CreateThread(0, 0, ProducerFunction, (LPVOID)(i + 1), 0, NULL);
	}

	// Инициализация потребителей.
	for (int i = 0; i < consumerThredsCount; i++)
	{
		consumerHandles[i] = CreateThread(0, 0, ConsumerFunction, (LPVOID)(i + 1), 0, NULL);
	}

	Sleep(3000);

	for (int i = 0; i < producerThreadsCount; i++)
	{
		CloseHandle(producerHandles[i]);
	}
	for (int i = 0; i < consumerThredsCount; i++)
	{
		CloseHandle(consumerHandles[i]);
	}
}


int main()
{
	ProducerConsumerTask(10, 20);
    return 0;
}

