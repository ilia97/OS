// Lab4.cpp: определяет точку входа для консольного приложения.
//

#include "stdafx.h"
#include <Windows.h>
#include <iostream>
#include <conio.h>

using namespace std;

// Класс для хранения списка станиц.
class PageList
{
private:
	int maxSize;   // Максимально возможный размер списка страниц.
	int count;     // Текущее количество страниц.
	int* pages;    // Массив адрессов страниц, хранящихся в списке.
	
public:
	PageList()
	{
		maxSize = 3;
		count = 0;
		pages = new int[maxSize];
	}

	PageList(int maxPagesCount)
	{
		maxSize = maxPagesCount;
		count = 0;
		pages = new int[maxSize];
	}

	// Метод для добавления новой страницы к списку.
	void Add(int newPage)
	{
		if (count < maxSize)
		{
			pages[count] = newPage;
			count++;
		}
		int startIndex = 0;
		for (int i = 0; i < count; i++)
		{
			if (pages[i] == newPage)
			{
				startIndex = i;
				break;
			}
		}

		for (int i = startIndex; i < count - 1; i++)
		{
			pages[i] = pages[i + 1];
		}
		pages[count - 1] = newPage;
	}

	// Метод распечатки списка.
	void Print()
	{
		if (count == 0)
		{
			printf("Your list is empty.\n");
			return;
		}
		printf("%d", pages[0]);
		for (int i = 1; i < count; i++)
		{
			printf(" %d", pages[i]);
		}
		printf("\n");
	}

	~PageList()
	{
		delete[](pages);
	}

	int GetCount()
	{
		return count;
	}
};

// Класс для хранения четырёхнаправленного кэша.
class CacheList
{
private:
	int* mask;  // Битовая маска.
	int* list;  // Массив элементов.

public:
	CacheList()
	{
		mask = new int[3];
		mask[0] = mask[1] = mask[2] = 0;
		list = new int[4];
	};

	// Добавление нового элемента.
	void Add(int key)
	{
		if (mask[0] == 0)
		{
			mask[0] = 1;
			if (mask[1] == 0)
			{
				list[0] = key;
				mask[1] = 1;
			}
			else
			{
				list[1] = key;
				mask[1] = 0;
			}
		}
		else
		{
			mask[0] = 0;
			if (mask[2] == 0)
			{
				list[2] = key;
				mask[2] = 1;
			}
			else
			{
				list[3] = key;
				mask[2] = 0;
			}
		}
	};

	// Распечатка состояния.
	void Print()
	{
		printf("Mask: ");
		printf("%d", mask[0]);
		for (int i = 1; i < 3; i++)
		{
			printf(" %d", mask[i]);
		}
		printf("\n");

		printf("Cache: ");
		printf("%d", list[0]);
		for (int i = 1; i < 4; i++)
		{
			printf(" %d", list[i]);
		}
		printf("\n");
	};
};

SYSTEM_INFO lpSystemInfo;
MEMORY_BASIC_INFORMATION buffer;
LPVOID lpMinimumApplicationAddress;
LPVOID lpMaximumApplicationAddress;

// Информация о системе.
void PrintSystemInfo()
{
	GetSystemInfo(&lpSystemInfo);
	cout << "System info:" << endl;
	//cout << "Active processor mark: " << lpSystemInfo.dwActiveProcessorMask << endl;
	//cout << "Allocation granularity: " << lpSystemInfo.dwAllocationGranularity << endl;
	//cout << "Number of procesors: " << lpSystemInfo.dwNumberOfProcessors << endl;
	//cout << "Oem Id: " << lpSystemInfo.dwOemId << endl;
	cout << "Page size: " << lpSystemInfo.dwPageSize << endl;
	//cout << "Processor type: " << lpSystemInfo.dwProcessorType << endl;
	cout << "Minimum application address: " << lpSystemInfo.lpMinimumApplicationAddress << endl;
	cout << "Maximum application address: " << lpSystemInfo.lpMaximumApplicationAddress << endl;
	//cout << "Processor architecture: " << lpSystemInfo.wProcessorArchitecture << endl;
	//cout << "Processor level: " << lpSystemInfo.wProcessorLevel << endl;
	//cout << "Processor revision: " << lpSystemInfo.wProcessorRevision << endl;
	//cout << "Reserved: " << lpSystemInfo.wReserved << endl;
	cout << "-------------------------------------------------------" << endl;

	lpMinimumApplicationAddress = lpSystemInfo.lpMinimumApplicationAddress;
	lpMaximumApplicationAddress = lpSystemInfo.lpMaximumApplicationAddress;
}

// Распечатка всех свободных страниц.
void PrintFreeMemoryBlocks()
{
	DWORD address = (DWORD)lpMinimumApplicationAddress;
	cout << "List of free blocks of memory:" << endl;
	while (address < (DWORD)lpMaximumApplicationAddress)
	{
		MEMORY_BASIC_INFORMATION buffer;
		VirtualQuery((LPCVOID)address, &buffer, sizeof(buffer));
		if (buffer.State == MEM_FREE)
		{
			_getch();
			//cout << "Allocation base: " << buffer.AllocationBase << endl;
			//cout << "Allocation protect: " << buffer.AllocationProtect << endl;
			cout << "Base address: " << buffer.BaseAddress << endl;
			//cout << "Protect: " << buffer.Protect << endl;
			cout << "Region size: " << buffer.RegionSize << endl;
			//cout << "State: " << buffer.State << endl;
			//cout << "Type: " << buffer.Type << endl;
			cout << endl;
		}
		address += buffer.RegionSize;
	}
}

// Выделение памяти размером size.
LPVOID AllocMemoryBlock(int size)
{
	LPVOID alloc = 0;
	bool flag = true;
	DWORD address = (DWORD)lpMinimumApplicationAddress;
	while (address < (DWORD)lpMaximumApplicationAddress)
	{
		MEMORY_BASIC_INFORMATION buffer;
		VirtualQuery((LPCVOID)address, &buffer, sizeof(buffer));
		if (buffer.State == MEM_FREE && buffer.RegionSize > size)
		{
			alloc = VirtualAlloc((LPVOID)address, size, MEM_RESERVE | MEM_COMMIT, PAGE_READONLY);
			cout << "Alloc function was successfully perfomed at address " << alloc << "." << endl;
			flag = false;
			break;

		}
		address += buffer.RegionSize;
	}
	if (flag)
	{
		cout << "There wasn't enough free memory to perform an alloc function." << endl;
	}
	return alloc;
}

// Освобождение памяти по адресу address.
void FreeMemoryBlock(LPVOID address)
{
	DWORD baseAddress = (DWORD)lpMinimumApplicationAddress;
	while (baseAddress <= (DWORD)address)
	{
		MEMORY_BASIC_INFORMATION buffer;
		VirtualQuery((LPCVOID)baseAddress, &buffer, sizeof(buffer));
		if (address < buffer.BaseAddress &&
			buffer.RegionSize + &buffer.BaseAddress > address)
		{
			VirtualFree(buffer.BaseAddress, buffer.RegionSize, MEM_DECOMMIT);
			cout << "Free function was successfully perfomed." << endl;
			break;

		}
		baseAddress += buffer.RegionSize;
	}
}

// Алгоритм замещения страниц.
void ReplacementAlgorithmRun()
{
	PageList pageList = PageList();
	pageList.Add(10);
	pageList.Print();
	pageList.Add(20);
	pageList.Print();
	pageList.Add(30);
	pageList.Print();
	pageList.Add(40);
	pageList.Print();
	pageList.Add(50);
	pageList.Print();
	pageList.Add(40);
	pageList.Print();
}

// LRU.
void RunLRU()
{
	CacheList c;
	c.Print();
	c.Add(10);
	c.Print();
	c.Add(20);
	c.Print();
	c.Add(30);
	c.Print();
	c.Add(40);
	c.Print();
	c.Add(10);
	c.Print();
	c.Add(50);
	c.Print();
}

int main()
{
	PrintSystemInfo();

	/*LPVOID res = AllocMemoryBlock(150);
	FreeMemoryBlock(res);*/

	PrintFreeMemoryBlocks();

	//ReplacementAlgorithmRun();

	//RunLRU();

	return 0;
}

