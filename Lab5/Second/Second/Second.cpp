// Second.cpp: определяет точку входа для консольного приложения.
//

#include "stdafx.h"
#include <Windows.h>
#include <iostream>

using namespace std;

void PrintFileInformation(WIN32_FIND_DATA data, FILETIME start, FILETIME end)
{
	// Выводим информацию только о файлах, созданные в указанный промежуток времени.
	if (CompareFileTime(&start, &data.ftCreationTime) < 0 &&
		CompareFileTime(&data.ftCreationTime, &end) < 0)
	{
		wcout << L"File name: " << data.cFileName << endl;
		wcout << L"Size: " << data.nFileSizeLow << " bytes" << endl;
		
		FILE *file;
		wchar_t path[128] = L"..\\..\\Files\\";
		wcscat_s(path, 128, data.cFileName);
		//wcscat_s(path, 128, L".txt");


		// Проверяем, какая кодировка в файл.
		_wfopen_s(&file, path, L"rt");
		// Первый байт метки.
		char BOM1 = fgetc(file);
		// Второй байт метки.
		char BOM2 = fgetc(file);
		// Третий байт метки.
		char BOM3 = fgetc(file);
		fclose(file);

		int rowsCount = 0;

		// Случай для UTF-16LE
		if (BOM1 == '\xff' && BOM2 == '\xfe')
		{
			_wfopen_s(&file, path, L"rt+, ccs=UTF-16LE");
			while (!feof(file))
			{
				wchar_t w = ' ';
				int rowLength = 0;
				while (!feof(file))
				{
					w = fgetwc(file);
					// w = 65535 только в конце файла. Этот символ не надо учитывать.
					if (w == '\n' || w == 65535)
						break;
					rowLength++;
				}
				rowsCount++;
				cout << rowsCount << " row length: " << rowLength << endl;
			}
			cout << "Total rows count: " << rowsCount << endl;
			fclose(file);
		}
		// UTF-8
		else if (BOM1 == '\xef' && BOM2 == '\xbb' && BOM3 == '\xbf')
		{
			_wfopen_s(&file, path, L"rt+, ccs=UTF-8");
			while (!feof(file))
			{
				wchar_t w = ' ';
				int rowLength = 0;
				while (!feof(file))
				{
					w = fgetwc(file);
					// w = 65535 только в конце файла. Этот символ не надо учитывать.
					if (w == '\n' || w == 65535)
						break;
					rowLength++;
				}
				rowsCount++;
				cout << rowsCount << " row length: " << rowLength << endl;
			}
			cout << "Total rows count: " << rowsCount << endl;
			fclose(file);
		}
		// Случай для ASCII
		else
		{
			_wfopen_s(&file, path, L"rt");
			while (!feof(file))
			{
				char w = ' ';
				int rowLength = 0;
				while (!feof(file))
				{
					w = getc(file);
					// w = -1 только в конце файла. Этот символ не надо учитывать.
					if (w == '\n' || w == -1)
						break;
					rowLength++;
				}
				rowsCount++;
				cout << rowsCount << " row length: " << rowLength << endl;
			}
			cout << "Total rows count: " << rowsCount << endl;
			fclose(file);
		}
		cout << "-------------------------------------------------" << endl;
	}
}

int _tmain(int argc, _TCHAR* argv[])
{
	FILETIME start, end;
	start.dwLowDateTime = *(DWORD*)argv[1];
	start.dwHighDateTime = *(DWORD*)argv[2];
	end.dwLowDateTime = *(DWORD*)argv[3];
	end.dwHighDateTime = *(DWORD*)argv[4];
	TCHAR *path = _T("..\\..\\Files\\*.txt");
	HANDLE hf;
	WIN32_FIND_DATA w32;
	hf = FindFirstFile(path, &w32);
	if (hf != INVALID_HANDLE_VALUE)
	{
		printf("The information about files:\n");
		PrintFileInformation(w32, start, end);
		while (FindNextFile(hf, &w32))
		{
			PrintFileInformation(w32, start, end);
		}
	}
	else
	{
		printf("The directory is empty.");
	}
	CloseHandle(hf);
    return 0;
}

