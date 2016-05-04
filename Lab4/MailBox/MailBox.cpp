// Lab3.cpp: определяет точку входа для консольного приложения.
//

#include "stdafx.h"
#include <Windows.h>
#include <iostream>

using namespace std;

HANDLE h;
HANDLE hMapFile;
LPVOID lpMapAddress;
// Простое число, которое используется для CRC-кодирования.
const long prime = 274876858367;

// Функция, которая проверяет, совпадает ли заново сгенерированный CRC-код и CRC-код, записанный в файле.
bool CheckCRC(wchar_t* path)
{
	// Открываем файл по заданному пути.
	h = CreateFile(
		path,
		GENERIC_READ | GENERIC_WRITE,
		0,
		NULL,
		OPEN_ALWAYS,
		FILE_ATTRIBUTE_NORMAL,
		NULL);

	if (h == NULL)
	{
		return false;
	}

	int messagesCount = 0;
	int messagesSize = 0;
	int maximumSize = 0;

	ReadFile(h, &messagesCount, 4, 0, NULL); // Количество сообщений
	ReadFile(h, &messagesSize, 4, 0, NULL); // Размер сообщений
	ReadFile(h, &maximumSize, 4, 0, NULL); // Максимальный размер почты

	int crc = 0;
	// Считываем по 4 байта, прибаляем их и на каждом шаге находим остаток от деления на простое число.
	for (int i = 0; i < messagesSize; i += 4)
	{
		int buffer = 0;
		// Если осталось до конца файла меньше 4 байтов, надо взять только остаток, чтобы не задеть CRC код.
		if (i + 4 > messagesSize)
		{
			ReadFile(h, &buffer, messagesSize - i, 0, NULL);
		}
		else
		{
			ReadFile(h, &buffer, 4, 0, NULL);
		}
		crc = (crc + buffer) % prime;
	}

	int oldCrc = 0;
	// Считываем уже записанный CRC-код.
	ReadFile(h, &oldCrc, sizeof(crc), 0, NULL);

	CloseHandle(h);

	if (crc == oldCrc)
		return true;
	return false;
}

// Функция, которая генерирует новый код для файла по указанному пути.
void GenerateCRC(wchar_t* path)
{
	h = CreateFile(
		path,
		GENERIC_READ | GENERIC_WRITE,
		0,
		NULL,
		OPEN_ALWAYS,
		FILE_ATTRIBUTE_NORMAL,
		NULL);

	int messagesCount = 0;
	int messagesSize = 0;
	int maximumSize = 0;

	ReadFile(h, &messagesCount, 4, 0, NULL); // Количество сообщений
	ReadFile(h, &messagesSize, 4, 0, NULL); // Размер сообщений
	ReadFile(h, &maximumSize, 4, 0, NULL); // Максимальный размер почты

	int crc = 0;
	for (int i = 0; i < messagesSize; i += 4)
	{
		int buffer = 0;
		// Если осталось до конца файла меньше 4 байтов, надо взять только остаток, чтобы не задеть CRC код.
		if (i + 4 > messagesSize)
		{
			ReadFile(h, &buffer, messagesSize - i, 0, NULL);
		}
		else
		{
			ReadFile(h, &buffer, 4, 0, NULL);
		}
		crc = (crc + buffer) % prime;
	}
	// Записываем CRC-код в конец файла.
	WriteFile(h, &crc, sizeof(crc), 0, NULL);
	// Устанавливаем после кода конец файла.
	SetEndOfFile(h);

	CloseHandle(h);
}

// Функция, которая задаёт новый почтовый ящик с указанным именем.
void CreateNewFile(wchar_t* mailName)
{
	wchar_t path[100] = L"Mail\\";
	wcscat_s(mailName, 100, L".txt");
	wcscat_s(path, 100, mailName);

	h = CreateFile(
		path,
		GENERIC_READ | GENERIC_WRITE,
		0,
		NULL,
		OPEN_ALWAYS,
		FILE_ATTRIBUTE_NORMAL,
		NULL);

	int messagesCount = 0;
	int messagesSize = 0;
	int maximumSize = 0x100000;

	hMapFile = CreateFileMapping(h, NULL, PAGE_READWRITE, 0, 12, NULL);
	lpMapAddress = MapViewOfFile(hMapFile, FILE_MAP_ALL_ACCESS, 0, 0, 12);
	int* intPointer = (int*)lpMapAddress;

	*intPointer = messagesCount; // Количество сообщений
	*intPointer++;
	*intPointer = messagesSize; // Размер сообщений
	*intPointer++;
	*intPointer = maximumSize; // Максимальный размер почты

	SetFilePointer(h, 12, 0, FILE_BEGIN); // Устанавливаем конец файла в данной точке.
	SetEndOfFile(h);

	UnmapViewOfFile(lpMapAddress);
	CloseHandle(hMapFile);
	CloseHandle(h);

	GenerateCRC(path);
}

// Функция, которая добавляет сообщение в конец файла в заданный ящик.
void AddMessage(TCHAR* text, TCHAR* mailName)
{
	wchar_t path[100] = L"Mail\\";
	wcscat_s(mailName, 100, L".txt");
	wcscat_s(path, 100, mailName);


	bool isChecked = CheckCRC(path);

	// В случае, если почтового ящика с указанным именем не существует, функция также вернёт false.
	if (!isChecked)
	{
		cout << "Файл был изменён или почтового ящика с таким названием не существует." << endl;
	}
	else
	{
		h = CreateFile(
			path,
			GENERIC_READ | GENERIC_WRITE,
			0,
			NULL,
			OPEN_ALWAYS,
			FILE_ATTRIBUTE_NORMAL,
			NULL);

		DWORD lpFileSizeHigh;
		// Вычисляем размер нового файла как размер старого файла + размер нового сообщения + 4 байта для записи размера.
		DWORD fileSize = GetFileSize(h, &lpFileSizeHigh) + (lstrlenW(text) * sizeof(TCHAR)) + 4;
		hMapFile = CreateFileMapping(h, NULL, PAGE_READWRITE, 0, fileSize, NULL);
		lpMapAddress = MapViewOfFile(hMapFile, FILE_MAP_ALL_ACCESS, 0, 0, fileSize);
		int* intPointer = (int*)lpMapAddress;
		byte* bytePointer = (byte*)lpMapAddress;

		int messagesCount = *intPointer;
		*intPointer++;
		int messagesSize = *intPointer;
		*intPointer++;
		int maximumSize = *intPointer;

		// Размер текста сообщения, который равен длине строки, умноженной на размер одного символа в байтах.
		int textSize = lstrlenW(text) * sizeof(TCHAR);
		// Размер сообщения с учётом поля, отвечающего за размер его текста.
		int newMessageSize = textSize + 4;

		// Увеличиваем общий размер ящика на размер сообщения.
		messagesSize += newMessageSize;

		// Проверяем, не вышли ли мы за допустимый размер ящика.
		if (messagesSize < maximumSize)
		{
			// Увеличиваем количество писем на 1.
			messagesCount += 1;
			// Устанавливаем указатель в начало файла.
			*intPointer--;
			*intPointer--;

			// Перезаписываем количество сообщений и общий размер сообщений.
			*intPointer = messagesCount;
			*intPointer++;
			*intPointer = messagesSize;

			// Перемещаем bytePointer на 12 + messagesSize байт.
			for (int i = 0; i < messagesSize - newMessageSize + 12; i++)
			{
				*bytePointer++;
			}

			// Записываем размер нового сообщения.
			*bytePointer = textSize;
			// Перемещаем указатель на 4 байта (Так как раньше там был написан CRC, то надо обнулить все участки памятиe).
			*bytePointer++;
			*bytePointer = 0;
			*bytePointer++;
			*bytePointer = 0;
			*bytePointer++;
			*bytePointer = 0;
			*bytePointer++;

			for (int i = 0; i < textSize / sizeof(TCHAR); i++)
			{
				*bytePointer = text[i];

				// Перемещаем указатель на размер TCHAR.
				*bytePointer++;
				if (sizeof(TCHAR) == 2)
				{
					*bytePointer++;
				}
			}
		}
		else
		{
			cout << "Ящик переполнен." << endl;
		}

		UnmapViewOfFile(lpMapAddress);
		CloseHandle(hMapFile);
		CloseHandle(h);

		GenerateCRC(path);

		cout << "Письмо было успешно добавлено." << endl;
	}
}

// Функция, распечатывающая сообщение с заданным номером из заданного ящика.
void GetMessage(int index, TCHAR* mailName)
{
	wchar_t path[100] = L"Mail\\";
	wcscat_s(mailName, 100, L".txt");
	wcscat_s(path, 100, mailName);


	bool isChecked = CheckCRC(path);

	if (!isChecked)
	{
		cout << "Файл был изменён или почтового ящика с таким названием не существует." << endl;
	}
	else
	{
		h = CreateFile(
			path,
			GENERIC_READ | GENERIC_WRITE,
			0,
			NULL,
			OPEN_ALWAYS,
			FILE_ATTRIBUTE_NORMAL,
			NULL);

		hMapFile = CreateFileMapping(h, NULL, PAGE_READWRITE, 0, 0, NULL);
		lpMapAddress = MapViewOfFile(hMapFile, FILE_MAP_ALL_ACCESS, 0, 0, 0);
		int* intPointer = (int*)lpMapAddress;
		byte* bytePointer = (byte*)lpMapAddress;

		int messagesCount = *intPointer;
		*intPointer++;

		if (index < 1 || messagesCount < index)
		{
			cout << "Письма с таким номером не существует." << endl;
			CloseHandle(h);
		}
		else
		{
			int messagesSize = *intPointer;
			*intPointer++;
			int maximumSize = *intPointer;

			int messageSize; // Размер следующего сообщения.
			TCHAR* text; // Текст сообщения.

			// Переносим указатель на 12 байт от начала файла.
			for (int i = 0; i < 12; i++)
			{
				*bytePointer++;
			}

			// Переходим к следующему сообщению, пока не достигнем необходимого нам номера.
			for (int i = 0; i < index; i++)
			{
				// Получаем размер сообщения.
				messageSize = *bytePointer;

				// Перемещаем указатель на 4 байта.
				*bytePointer++;
				*bytePointer++;
				*bytePointer++;
				*bytePointer++;

				// Создаём новый массив символов.
				text = new TCHAR[messageSize / sizeof(TCHAR)];
				//Побайтово считываем из памяти в массив.
				for (int j = 0; j < messageSize / sizeof(TCHAR); j++)
				{
					text[j] = *bytePointer;
					*bytePointer++;
					if (sizeof(TCHAR) == 2)
					{
						*bytePointer++;
					}
				}
			}

			UnmapViewOfFile(lpMapAddress);
			CloseHandle(hMapFile);
			CloseHandle(h);

			cout << "Ваше письмо:" << endl;
			_tprintf(text);
		}
	}
}

// Функция, удаляющая сообщение по заданному индексу из заданного ящика.
void RemoveMessage(int index, TCHAR* mailName, bool isPrinted)
{
	wchar_t path[100] = L"Mail\\";
	wcscat_s(mailName, 100, L".txt");
	wcscat_s(path, 100, mailName);


	bool isChecked = CheckCRC(path);

	if (!isChecked)
	{
		cout << "Файл был изменён или почтового ящика с таким названием не существует." << endl;
	}
	else
	{
		h = CreateFile(
			path,
			GENERIC_READ | GENERIC_WRITE,
			0,
			NULL,
			OPEN_ALWAYS,
			FILE_ATTRIBUTE_NORMAL,
			NULL);

		hMapFile = CreateFileMapping(h, NULL, PAGE_READWRITE, 0, 0, NULL);
		lpMapAddress = MapViewOfFile(hMapFile, FILE_MAP_ALL_ACCESS, 0, 0, 0);
		int* intPointer = (int*)lpMapAddress;
		byte* bytePointer = (byte*)lpMapAddress;

		int messagesCount = *intPointer;
		*intPointer++;

		if (index < 1 || messagesCount < index)
		{
			cout << "Письма с таким номером не существует." << endl;
			CloseHandle(h);
		}
		else
		{
			int messagesSize = *intPointer;
			*intPointer++;
			int maximumSize = *intPointer;

			// Устанавливаем указатель в начало файла.
			*intPointer--;
			*intPointer--;

			for (int i = 0; i < 12; i++)
			{
				*bytePointer++;
			}

			int currentPostion = 0;
			int messageSize; // Размер следующего сообщения.
			TCHAR* text; // Текст сообщения.
			int i = 0;
			for (; i < index; i++)
			{
				messageSize = *bytePointer;
				*bytePointer++;
				*bytePointer++;
				*bytePointer++;
				*bytePointer++;

				text = new TCHAR[messageSize / sizeof(TCHAR)];
				//Побайтово считываем из памяти в массив.
				for (int j = 0; j < messageSize / sizeof(TCHAR); j++)
				{
					text[j] = *bytePointer;
					*bytePointer++;
					if (sizeof(TCHAR) == 2)
					{
						*bytePointer++;
					}
				}

				currentPostion += messageSize + 4;
			}

			if (isPrinted)
			{
				cout << "Ваше письмо:" << endl;
				_tprintf(text);
				printf("\n");
			}

			// Смещаем все байты на (messageSize + 4) влево, пока не переберём все байты.
			for (; currentPostion < messagesSize; currentPostion += sizeof(TCHAR))
			{
				text = new TCHAR[1];
				// Запоминаем байт в буфер.
				text[0] = *bytePointer;
				// Переносим указатель на sizeof(TCHAR) байт.
				*bytePointer++;
				if (sizeof(TCHAR) == 2)
				{
					*bytePointer++;
				}

				// Возвращаемся к текущей позиции.
				for (int j = 0; j < messageSize + 4 + (int)sizeof(TCHAR); j++)
				{
					*bytePointer--;
				}

				// Записываем байт на текущую позицию.
				*bytePointer = text[0];

				// Переходим к следующему байту.
				for (int j = 0; j < messageSize + 4 + (int)sizeof(TCHAR); j++)
				{
					*bytePointer++;
				}
			}

			// Уменьшаем количество сообщений на 1.
			messagesCount -= 1;
			// Уменьшаем размер ящика на размер удалённого сообщения.
			messagesSize -= messageSize;
			// Размер поля, хранящего размер текста сообщения.
			messagesSize -= 4;

			// Обрезаем файл.
			SetFilePointer(h, messagesSize + 12, 0, FILE_BEGIN);
			SetEndOfFile(h);
			
			*intPointer = messagesCount;
			*intPointer++;
			*intPointer = messagesSize;

			UnmapViewOfFile(lpMapAddress);
			CloseHandle(hMapFile);
			CloseHandle(h);

			GenerateCRC(path);

			cout << "Ваше письмо было успешно удалено." << endl;
		}
	}
}

// Функция, удаляющая все сообщения из заданного ящика.
void RemoveAllMessages(TCHAR* mailName)
{
	wchar_t path[100] = L"Mail\\";
	wcscat_s(mailName, 100, L".txt");
	wcscat_s(path, 100, mailName);

	bool isChecked = CheckCRC(path);

	if (!isChecked)
	{
		cout << "Файл был изменён или почтового ящика с таким названием не существует." << endl;
	}
	else
	{
		h = CreateFile(
			path,
			GENERIC_READ | GENERIC_WRITE,
			0,
			NULL,
			OPEN_ALWAYS,
			FILE_ATTRIBUTE_NORMAL,
			NULL);

		int messagesCount = 0;
		int messagesSize = 0;
		int maximumSize = 0x100000;

		hMapFile = CreateFileMapping(h, NULL, PAGE_READWRITE, 0, 0, NULL);
		lpMapAddress = MapViewOfFile(hMapFile, FILE_MAP_ALL_ACCESS, 0, 0, 0);
		int* intPointer = (int*)lpMapAddress;

		// Перезаписываем файл, как будто это новый файл, в котором нет сообщений.
		*intPointer = messagesCount; // Количество сообщений
		*intPointer++;
		*intPointer = messagesSize; // Размер сообщений
		*intPointer++;
		*intPointer = maximumSize; // Максимальный размер почты
							
		SetFilePointer(h, 12, 0, FILE_BEGIN); // Устанавливаем конец файла в данной точке.
		SetEndOfFile(h);
		UnmapViewOfFile(lpMapAddress);
		CloseHandle(hMapFile);
		CloseHandle(h);

		GenerateCRC(path);
	}
}

// Функция, которая считает количество почтовых ящиков в данной папке.
int MailsCount()
{
	WIN32_FIND_DATA FindFileData;
	HANDLE hFind;

	// Получаем первый файл, удовлетворяющий маске.
	hFind = FindFirstFile(L"Mail\\*.txt", &FindFileData);
	int count = 0;
	// Если ни одного файла не существует, возвращаем 0.
	if (hFind != INVALID_HANDLE_VALUE)
	{
		count += 1;
		// Пока существует ещё файл, увеличиваем количесвто файлов на 1.
		while (FindNextFile(hFind, &FindFileData))
		{
			count += 1;
		}
	}
	FindClose(hFind);
	return count;
}

// Функция, которая считает количество сообщений в данном почтовом ящике.
void GetMessagesCount(TCHAR* mailName)
{
	wchar_t path[100] = L"Mail\\";
	wcscat_s(mailName, 100, L".txt");
	wcscat_s(path, 100, mailName);


	bool isChecked = CheckCRC(path);

	if (!isChecked)
	{
		cout << "Файл был изменён или почтового ящика с таким названием не существует." << endl;
	}
	else
	{
		h = CreateFile(
			path,
			GENERIC_READ | GENERIC_WRITE,
			0,
			NULL,
			OPEN_ALWAYS,
			FILE_ATTRIBUTE_NORMAL,
			NULL);

		hMapFile = CreateFileMapping(h, NULL, PAGE_READWRITE, 0, 0, NULL);
		lpMapAddress = MapViewOfFile(hMapFile, FILE_MAP_ALL_ACCESS, 0, 0, 0);
		int* intPointer = (int*)lpMapAddress;
		int messagesCount = *intPointer;

		UnmapViewOfFile(lpMapAddress);
		CloseHandle(hMapFile);
		CloseHandle(h);
		cout << "Количество сообщений в данном почтовом ящике: " << messagesCount << endl;
	}
}

int main()
{
	// Устанавливаем локальный режим для того, чтобы выводить символы кирилицы.
	_tsetlocale(LC_ALL, _T("Russian"));

	while (true)
	{
		printf("Введите 1, чтобы добавить письмо.\n");
		printf("Введите 2, чтобы прочитать письмо без удаления (счёт начинается с 1).\n");
		printf("Введите 3, чтобы прочитать письмо с удалением (счёт начинается с 1).\n");
		printf("Введите 4, чтобы удалить письмо с заданным индексом.\n");
		printf("Введите 5, чтобы удалить все письма.\n");
		printf("Введите 6, чтобы посчитать количество сообщений в ящике.\n");
		printf("Введите 7, чтобы посчитать количество почтовых ящиков.\n");
		printf("Введите 8, чтобы создать новый почтовый ящик.\n");
		printf("Введите 0, чтобы завершить работу.\n");

		char number;
		cin >> number;
		TCHAR text[1000];
		TCHAR mailName[100];
		int index = 0;
		int c = 0;

		bool flag = false;

		switch (number)
		{
		case '1':
			printf("Введите название почтового ящика:\n");

			getchar();
			_fgetts(mailName, 100, stdin);
			mailName[wcslen(mailName) - 1] = '\0';

			printf("Введите текст нового письма:\n");

			_fgetts(text, 100, stdin);
			text[wcslen(text) - 1] = '\0';

			AddMessage(text, mailName);
			break;
		case '2':
			cout << "Введите название почтового ящика:" << endl;

			getchar();
			_fgetts(mailName, 100, stdin);
			mailName[wcslen(mailName) - 1] = '\0';

			cout << "Введите номер письма, которое вы хотите прочитать:" << endl;
			int index;
			cin >> index;
			GetMessage(index, mailName);
			cout << endl;
			break;
		case '3':
			cout << "Введите название почтового ящика:" << endl;

			getchar();
			_fgetts(mailName, 100, stdin);
			mailName[wcslen(mailName) - 1] = '\0';

			cout << "Введите номер письма, которое вы хотите прочитать:" << endl;
			cin >> index;
			RemoveMessage(index, mailName, true);
			break;
		case '4':
			cout << "Введите название почтового ящика:" << endl;

			getchar();
			_fgetts(mailName, 100, stdin);
			mailName[wcslen(mailName) - 1] = '\0';

			cout << "Введите номер письма, которое вы хотите удалить:" << endl;
			cin >> index;
			RemoveMessage(index, mailName, false);
			break;
		case '5':
			cout << "Введите название почтового ящика:" << endl;

			getchar();
			_fgetts(mailName, 100, stdin);
			mailName[wcslen(mailName) - 1] = '\0';

			RemoveAllMessages(mailName);
			cout << "Все письма были успешно удалены." << endl;
			break;
		case '6':
			cout << "Введите название почтового ящика:" << endl;

			getchar();
			_fgetts(mailName, 100, stdin);
			mailName[wcslen(mailName) - 1] = '\0';

			GetMessagesCount(mailName);
			break;
		case '7':
			c = MailsCount();
			cout << "Количество почтовых ящиков: " << c << endl;
			break;
		case '8':
			printf("Введите название нового почтового ящика:\n");
			//_tscanf_s(TEXT("%ws"), mailName, 200);

			getchar();
			_fgetts(mailName, 100, stdin);
			mailName[wcslen(mailName) - 1] = '\0';

			CreateNewFile(mailName);
			break;
		case '0':
			flag = true;
			break;
		}

		if (flag)
			break;
	}

	return 0;
}

