// Lab3.cpp: ���������� ����� ����� ��� ����������� ����������.
//

#include "stdafx.h"
#include <Windows.h>
#include <iostream>

using namespace std;

HANDLE h;
// ������� �����, ������� ������������ ��� CRC-�����������.
const long prime = 274876858367;

// �������, ������� ���������, ��������� �� ������ ��������������� CRC-��� � CRC-���, ���������� � �����.
bool CheckCRC(wchar_t* path)
{
	// ��������� ���� �� ��������� ����.
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

	ReadFile(h, &messagesCount, 4, 0, NULL); // ���������� ���������
	ReadFile(h, &messagesSize, 4, 0, NULL); // ������ ���������
	ReadFile(h, &maximumSize, 4, 0, NULL); // ������������ ������ �����

	int crc = 0;
	// ��������� �� 4 �����, ��������� �� � �� ������ ���� ������� ������� �� ������� �� ������� �����.
	for (int i = 0; i < messagesSize; i += 4)
	{
		int buffer = 0;
		// ���� �������� �� ����� ����� ������ 4 ������, ���� ����� ������ �������, ����� �� ������ CRC ���.
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
	// ��������� ��� ���������� CRC-���.
	ReadFile(h, &oldCrc, sizeof(crc), 0, NULL);

	CloseHandle(h);

	if (crc == oldCrc)
		return true;
	return false;
}

// �������, ������� ���������� ����� ��� ��� ����� �� ���������� ����.
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

	ReadFile(h, &messagesCount, 4, 0, NULL); // ���������� ���������
	ReadFile(h, &messagesSize, 4, 0, NULL); // ������ ���������
	ReadFile(h, &maximumSize, 4, 0, NULL); // ������������ ������ �����

	int crc = 0;
	for (int i = 0; i < messagesSize; i += 4)
	{
		int buffer = 0;
		// ���� �������� �� ����� ����� ������ 4 ������, ���� ����� ������ �������, ����� �� ������ CRC ���.
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
	// ���������� CRC-��� � ����� �����.
	WriteFile(h, &crc, sizeof(crc), 0, NULL);
	// ������������� ����� ���� ����� �����.
	SetEndOfFile(h);

	CloseHandle(h);
}

// �������, ������� ����� ����� �������� ���� � ��������� ������.
void CreateNewFile(wchar_t* mailName)
{
	wchar_t path[100] = L"Mail\\";
	wcscat_s(mailName, 100, L".txt");
	wcscat_s(path, 100, mailName);

	h = CreateFile(
		path,
		GENERIC_WRITE,
		0,
		NULL,
		CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL,
		NULL);

	int messagesCount = 0;
	int messagesSize = 0;
	int maximumSize = 0x100000;

	WriteFile(h, &messagesCount, 4, 0, NULL); // ���������� ���������
	WriteFile(h, &messagesSize, 4, 0, NULL); // ������ ���������
	WriteFile(h, &maximumSize, 4, 0, NULL); // ������������ ������ �����

	CloseHandle(h);

	GenerateCRC(path);
}

// �������, ������� ��������� ��������� � ����� ����� � �������� ����.
void AddMessage(TCHAR* text, TCHAR* mailName)
{
	wchar_t path[100] = L"Mail\\";
	wcscat_s(mailName, 100, L".txt");
	wcscat_s(path, 100, mailName);


	bool isChecked = CheckCRC(path);

	// � ������, ���� ��������� ����� � ��������� ������ �� ����������, ������� ����� ����� false.
	if (!isChecked)
	{
		cout << "���� ��� ������ ��� ��������� ����� � ����� ��������� �� ����������." << endl;
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
		int maximumSize = 0;

		ReadFile(h, &messagesCount, 4, 0, NULL); // ���������� ���������
		ReadFile(h, &messagesSize, 4, 0, NULL); // ������ ���������
		ReadFile(h, &maximumSize, 4, 0, NULL); // ������������ ������ �����

		// ������ ������ ���������, ������� ����� ����� ������, ���������� �� ������ ������ ������� � ������.
		int textSize = lstrlenW(text) * sizeof(TCHAR);
		// ������ ��������� � ������ ����, ����������� �� ������ ��� ������.
		int newMessageSize = textSize + 4; 

		// ����������� ����� ������ ����� �� ������ ���������.
		messagesSize += newMessageSize;

		// ���������, �� ����� �� �� �� ���������� ������ �����.
		if (messagesSize < maximumSize)
		{
			// ����������� ���������� ����� �� 1.
			messagesCount += 1;
			// ������������� ��������� � ������ �����.
			SetFilePointer(h, 0, 0, FILE_BEGIN);
			// �������������� ���������� ��������� � ����� ������ ���������.
			WriteFile(h, &messagesCount, 4, 0, NULL);
			WriteFile(h, &messagesSize, 4, 0, NULL);

			// �� ��������� �� 8-�� ����, ����������� �� ���� ������ �����, 
			// ������� ���� ������������� �� 4 ����� � ����� �� ������ ���� ��������� ��� ����� ������.
			SetFilePointer(h, messagesSize - newMessageSize + 4, 0, FILE_CURRENT);

			// ���������� ������ ������ �����.
			WriteFile(h, &textSize, 4, 0, NULL);
			// ���������� ����� ������ ���������.
			WriteFile(h, text, textSize, 0, NULL);
		}
		else
		{
			cout << "���� ����������." << endl;
		}

		CloseHandle(h);

		GenerateCRC(path);

		cout << "������ ���� ������� ���������." << endl;
	}
}

// �������, ��������������� ��������� � �������� ������� �� ��������� �����.
void GetMessage(int index, TCHAR* mailName)
{
	wchar_t path[100] = L"Mail\\";
	wcscat_s(mailName, 100, L".txt");
	wcscat_s(path, 100, mailName);


	bool isChecked = CheckCRC(path);

	if (!isChecked)
	{
		cout << "���� ��� ������ ��� ��������� ����� � ����� ��������� �� ����������." << endl;
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
		int maximumSize = 0;

		ReadFile(h, &messagesCount, 4, 0, NULL); // ���������� ���������
		if (index < 1 || messagesCount < index)
		{
			cout << "������ � ����� ������� �� ����������." << endl; 
			CloseHandle(h);
		}
		else
		{
			ReadFile(h, &messagesSize, 4, 0, NULL); // ������ ���������
			ReadFile(h, &maximumSize, 4, 0, NULL); // ������������ ������ �����

			int messageSize; // ������ ���������� ���������.
			TCHAR* text; // ����� ���������.

			// ��������� � ���������� ���������, ���� �� ��������� ������������ ��� ������.
			for (int i = 0; i < index; i++)
			{
				ReadFile(h, &messageSize, 4, 0, NULL);
				text = new TCHAR[messageSize / sizeof(TCHAR)];
				ReadFile(h, text, messageSize, 0, NULL);
			}

			CloseHandle(h);

			cout << "���� ������:" << endl;
			_tprintf(text);
		}
	}
}

// �������, ��������� ��������� �� ��������� ������� �� ��������� �����.
void RemoveMessage(int index, TCHAR* mailName, bool isPrinted)
{
	wchar_t path[100] = L"Mail\\";
	wcscat_s(mailName, 100, L".txt");
	wcscat_s(path, 100, mailName);


	bool isChecked = CheckCRC(path);

	if (!isChecked)
	{
		cout << "���� ��� ������ ��� ��������� ����� � ����� ��������� �� ����������." << endl;
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
		int maximumSize = 0;

		ReadFile(h, &messagesCount, 4, 0, NULL); // ���������� ���������
		if (index < 1 || messagesCount < index)
		{
			cout << "������ � ����� ������� �� ����������." << endl;
			CloseHandle(h);
		}
		else
		{
			ReadFile(h, &messagesSize, 4, 0, NULL); // ������ ���������
			ReadFile(h, &maximumSize, 4, 0, NULL); // ������������ ������ �����

			int currentPostion = 0;
			int messageSize; // ������ ���������� ���������.
			TCHAR* text; // ����� ���������.
			int i = 0;
			for (; i < index; i++)
			{
				ReadFile(h, &messageSize, 4, 0, NULL);
				text = new TCHAR[messageSize / sizeof(TCHAR)];
				ReadFile(h, text, messageSize, 0, NULL);
				currentPostion += messageSize + 4;
			}

			if (isPrinted)
			{
				cout << "���� ������:" << endl;
				_tprintf(text);
				printf("\n");
			}

			// ������� ��� ����� �� (messageSize + 4) �����, ���� �� �������� ��� �����.
			for (; currentPostion < messagesSize; currentPostion += sizeof(TCHAR))
			{
				text = new TCHAR[1];
				// ���������� ���� � �����.
				ReadFile(h, text, sizeof(TCHAR), 0, NULL);
				// ������������ � ������� �������.
				SetFilePointer(h, -(messageSize + 4 + (int)sizeof(TCHAR)), 0, FILE_CURRENT);
				// ���������� ��� �� ������� �������.
				WriteFile(h, text, sizeof(TCHAR), 0, NULL);
				// ��������� � ���������� �����.
				SetFilePointer(h, messageSize + 4, 0, FILE_CURRENT);
			}
			SetEndOfFile(h);

			// ��������� ���������� ��������� �� 1.
			messagesCount -= 1;
			// ��������� ������ ����� �� ������ ��������� ���������.
			messagesSize -= messageSize;
			// ������ ����, ��������� ������ ������ ���������.
			messagesSize -= 4;

			// ������������� ��������� � ������ �����.
			SetFilePointer(h, 0, 0, FILE_BEGIN);
			WriteFile(h, &messagesCount, 4, 0, NULL); // �������������� ���������� ���������.
			WriteFile(h, &messagesSize, 4, 0, NULL); // �������������� ����� ������ ���������.

			CloseHandle(h);

			GenerateCRC(path);

			cout << "���� ������ ���� ������� �������." << endl;
		}
	}
}

// �������, ��������� ��� ��������� �� ��������� �����.
void RemoveAllMessages(TCHAR* mailName)
{
	wchar_t path[100] = L"Mail\\";
	wcscat_s(mailName, 100, L".txt");
	wcscat_s(path, 100, mailName);


	bool isChecked = CheckCRC(path);

	if (!isChecked)
	{
		cout << "���� ��� ������ ��� ��������� ����� � ����� ��������� �� ����������." << endl;
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

		// �������������� ����, ��� ����� ��� ����� ����, � ������� ��� ���������.
		WriteFile(h, &messagesCount, 4, 0, NULL); // ���������� ���������
		WriteFile(h, &messagesSize, 4, 0, NULL); // ������ ���������
		WriteFile(h, &maximumSize, 4, 0, NULL); // ������������ ������ �����
		// ������������� ����� ����� � ������ �����.
		SetEndOfFile(h);
		CloseHandle(h);

		GenerateCRC(path);
	}
}

// �������, ������� ������� ���������� �������� ������ � ������ �����.
int MailsCount()
{
	WIN32_FIND_DATA FindFileData;
	HANDLE hFind;

	// �������� ������ ����, ��������������� �����.
	hFind = FindFirstFile(L"Mail\\*.txt", &FindFileData);
	int count = 0;
	// ���� �� ������ ����� �� ����������, ���������� 0.
	if (hFind != INVALID_HANDLE_VALUE)
	{
		count += 1;
		// ���� ���������� ��� ����, ����������� ���������� ������ �� 1.
		while (FindNextFile(hFind, &FindFileData))
		{
			count += 1;
		}
	}
	FindClose(hFind);
	return count;
}

// �������, ������� ������� ���������� ��������� � ������ �������� �����.
void GetMessagesCount(TCHAR* mailName)
{
	wchar_t path[100] = L"Mail\\";
	wcscat_s(mailName, 100, L".txt");
	wcscat_s(path, 100, mailName);


	bool isChecked = CheckCRC(path);

	if (!isChecked)
	{
		cout << "���� ��� ������ ��� ��������� ����� � ����� ��������� �� ����������." << endl;
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
		ReadFile(h, &messagesCount, 4, 0, NULL); // ���������� ���������

		CloseHandle(h);
		cout << "���������� ��������� � ������ �������� �����: " << messagesCount << endl;
	}
}

int main()
{
	// ������������� ��������� ����� ��� ����, ����� �������� ������� ��������.
	_tsetlocale(LC_ALL, _T("Russian"));
	
	while (true)
	{
		printf("������� 1, ����� �������� ������.\n");
		printf("������� 2, ����� ��������� ������ ��� �������� (���� ���������� � 1).\n");
		printf("������� 3, ����� ��������� ������ � ��������� (���� ���������� � 1).\n");
		printf("������� 4, ����� ������� ������ � �������� ��������.\n");
		printf("������� 5, ����� ������� ��� ������.\n");
		printf("������� 6, ����� ��������� ���������� ��������� � �����.\n");
		printf("������� 7, ����� ��������� ���������� �������� ������.\n");
		printf("������� 8, ����� ������� ����� �������� ����.\n");
		printf("������� 0, ����� ��������� ������.\n");

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
				printf("������� �������� ��������� �����:\n");

				getchar();
				_fgetts(mailName, 100, stdin);
				mailName[wcslen(mailName) - 1] = '\0';

				printf("������� ����� ������ ������:\n");

				_fgetts(text, 100, stdin);
				text[wcslen(text) - 1] = '\0';

				AddMessage(text, mailName);
				break;
			case '2':
				cout << "������� �������� ��������� �����:" << endl;

				getchar();
				_fgetts(mailName, 100, stdin);
				mailName[wcslen(mailName) - 1] = '\0';

				cout << "������� ����� ������, ������� �� ������ ���������:" << endl;
				int index;
				cin >> index;
				GetMessage(index, mailName);
				cout << endl;
				break;
			case '3':
				cout << "������� �������� ��������� �����:" << endl;

				getchar();
				_fgetts(mailName, 100, stdin);
				mailName[wcslen(mailName) - 1] = '\0';

				cout << "������� ����� ������, ������� �� ������ ���������:" << endl;
				cin >> index;
				RemoveMessage(index, mailName, true);
				break;
			case '4':
				cout << "������� �������� ��������� �����:" << endl;

				getchar();
				_fgetts(mailName, 100, stdin);
				mailName[wcslen(mailName) - 1] = '\0';

				cout << "������� ����� ������, ������� �� ������ �������:" << endl;
				cin >> index;
				RemoveMessage(index, mailName, false);
				break;
			case '5':
				cout << "������� �������� ��������� �����:" << endl;

				getchar();
				_fgetts(mailName, 100, stdin);
				mailName[wcslen(mailName) - 1] = '\0';

				RemoveAllMessages(mailName);
				cout << "��� ������ ���� ������� �������." << endl;
				break;
			case '6':
				cout << "������� �������� ��������� �����:" << endl;

				getchar();
				_fgetts(mailName, 100, stdin);
				mailName[wcslen(mailName) - 1] = '\0';

				GetMessagesCount(mailName);
				break;
			case '7':
				c = MailsCount();
				cout << "���������� �������� ������: " << c << endl;
				break;
			case '8':
				printf("������� �������� ������ ��������� �����:\n");
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

