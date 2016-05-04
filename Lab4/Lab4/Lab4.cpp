// Lab4.cpp: определяет точку входа для консольного приложения.
//

#include "stdafx.h"
#include <Windows.h>
#include <iostream>

using namespace std;

class PageList
{
private:
	int maxSize;
	int count;
	int* pages;
	
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

class CacheList
{
private:
	int* mask;
	int* list;

public:
	CacheList()
	{
		mask = new int[3];
		mask[0] = mask[1] = mask[2] = 0;
		list = new int[4];
	};

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
	cout << "Active processor mark: " << lpSystemInfo.dwActiveProcessorMask << endl;
	cout << "Allocation granularity: " << lpSystemInfo.dwAllocationGranularity << endl;
	cout << "Number of procesors: " << lpSystemInfo.dwNumberOfProcessors << endl;
	cout << "Oem Id: " << lpSystemInfo.dwOemId << endl;
	cout << "Page size: " << lpSystemInfo.dwPageSize << endl;
	cout << "Processor type: " << lpSystemInfo.dwProcessorType << endl;
	cout << "Minimum application address: " << lpSystemInfo.lpMinimumApplicationAddress << endl;
	cout << "Maximum application address: " << lpSystemInfo.lpMaximumApplicationAddress << endl;
	cout << "Processor architecture: " << lpSystemInfo.wProcessorArchitecture << endl;
	cout << "Processor level: " << lpSystemInfo.wProcessorLevel << endl;
	cout << "Processor revision: " << lpSystemInfo.wProcessorRevision << endl;
	cout << "Reserved: " << lpSystemInfo.wReserved << endl;
	cout << "-------------------------------------------------------" << endl;

	lpMinimumApplicationAddress = lpSystemInfo.lpMinimumApplicationAddress;
	lpMaximumApplicationAddress = lpSystemInfo.lpMaximumApplicationAddress;
}

// Распечатка всех свободных страниц.
void PrintFreeMemoryBlocks()
{
	LPVOID address = lpMinimumApplicationAddress;
	cout << "List of free blocks of memory:" << endl;
	while (address < lpMaximumApplicationAddress)
	{
		MEMORY_BASIC_INFORMATION buffer;
		VirtualQuery((LPCVOID)address, &buffer, sizeof(buffer));
		if (buffer.State == MEM_FREE)
		{
			cout << "Allocation base: " << buffer.AllocationBase << endl;
			cout << "Allocation protect: " << buffer.AllocationProtect << endl;
			cout << "Base address: " << buffer.BaseAddress << endl;
			cout << "Protect: " << buffer.Protect << endl;
			cout << "Region size: " << buffer.RegionSize << endl;
			cout << "State: " << buffer.State << endl;
			cout << "Type: " << buffer.Type << "\n" << endl;
		}
		address = &address + buffer.RegionSize;
	}
}

// Выделение памяти размером size.
void AllocMemoryBlock(int size)
{
	bool flag = true;
	LPVOID address = lpMinimumApplicationAddress;
	while (address < lpMaximumApplicationAddress)
	{
		MEMORY_BASIC_INFORMATION buffer;
		VirtualQuery((LPCVOID)address, &buffer, sizeof(buffer));
		if (buffer.State == MEM_FREE && buffer.RegionSize > size)
		{
			VirtualAlloc(address, size, MEM_COMMIT, PAGE_EXECUTE);
			cout << "Alloc function was successfully perfomed." << endl;
			flag = false;
			break;

		}
		address = &address + buffer.RegionSize;
	}
	if (flag)
	{
		cout << "There wasn't enough free memory to perform an alloc function." << endl;
	}
}

// Освобождение памяти по адресу address.
void FreeMemoryBlock(LPVOID address)
{
	LPVOID baseAddress = lpMinimumApplicationAddress;
	while (baseAddress <= address)
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
		baseAddress = &baseAddress + buffer.RegionSize;
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
	//PrintSystemInfo();

	//PrintFreeMemoryBlocks();

	ReplacementAlgorithmRun();

	//RunLRU();

	return 0;
}

