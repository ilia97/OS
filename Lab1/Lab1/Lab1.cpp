// Lab1.cpp: определяет точку входа для консольного приложения.
//

#include <iostream>
#include "stdafx.h"
#include <locale.h>
#include <windows.h>
#include <fstream>

using namespace std;

// 1. Программа для определения кодировки символов по умолчанию.
char* GetProgramEncoding()
{
	if (typeid(char).name() == typeid(TCHAR).name())
	{
		return "ANCII";
	}
	else
	{
		return "Unicode";
	}

	//-----OR------
	/*if (sizeof(TCHAR) == 1)
	{
	return "ANCII";
	}
	else
	{
	return "Unicode";
	}*/
}


int wcompare(void const * arg1, void const * arg2)
{
	int n = lstrcmpW(*(const wchar_t**)arg1, *(const wchar_t**)arg2);
	return n;
}

// Объединение двух строк в третью (wchar_t).
void concatW(wchar_t* buffer, wchar_t* text1, wchar_t* text2)
{
	int i = 0;
	int len1 = wcslen(text1);
	int len2 = wcslen(text2);
	for ( ; i < len1; i++)
	{
		buffer[i] = text1[i];
	}

	for (; i < len1 + len2; i++)
	{
		buffer[i] = text2[i - len1];
	}
}

// Объединение двух строк в третью (char).
void concatC(char* buffer, char* text1, char* text2)
{
	int i = 0;
	int len1 = strlen(text1);
	int len2 = strlen(text2);
	for (; i < len1; i++)
	{
		buffer[i] = text1[i];
	}

	for (; i < len1 + len2; i++)
	{
		buffer[i] = text2[i - len1];
	}
}

// Переворот мтроки (wchar_t).
void reverseW(wchar_t* buffer, wchar_t* text1)
{
	int len = wcslen(text1);

	for (int i = 0; i < len - 1; i++)
	{
		// В данном случае вычитаем 2, потому что мы символ перехода 
		// на новую строку и символ конца файла оставляем на своём месте.
		buffer[i] = text1[len - i - 2];
	}

	// Добавляем символ перехода на новую строку в конец строки.
	if (text1[len - 1] == '\n')
		buffer[len - 1] = text1[len - 1];
}

// Переворот мтроки (char).
void reverseC(char* buffer, char* text1)
{
	int len = strlen(text1);

	// В данном случае вычитаем 2, потому что мы переход на новую строку оставляем на своём месте.
	if (text1[len - 1] == '\n')
	{
		for (int i = 0; i < len - 1; i++)
		{
			buffer[i] = text1[len - i - 2];
		}
		// Добавляем переход на новую строку в конце файла.
		buffer[len - 1] = '\n';
	}
	else
	{
		for (int i = 0; i < len; i++)
		{
			buffer[i] = text1[len - i - 1];
		}
	}
}

void ReverceText(char* path)
{
	FILE *file;

	// Проверяем, какая кодировка в файл.
	fopen_s(&file, path, "rt");
	// Первый байт метки.
	char BOM1 = fgetc(file);
	// Второй байт метки.
	char BOM2 = fgetc(file);
	// Третий байт метки.
	char BOM3 = fgetc(file);
	fclose(file);

	// Случай для UTF-16LE
	if (BOM1 == '\xff' && BOM2 == '\xfe')
	{
		fopen_s(&file, path, "rt+, ccs=UTF-16LE");
		
		wchar_t text[30000] = L"";

		while (!feof(file))
		{
			wchar_t str1[256] = L"";
			wchar_t str2[256] = L"";

			wchar_t w = ' ';
			for (int i = 0; w != '\n' && !feof(file); i++)
			{
				w = fgetwc(file);
				str1[i] = w;
			}

			//fgetws(str1, 256, file);
			reverseW(str2, str1);
			concatW(text, text, str2);
		}
		fclose(file);

		// Открываем файл для записи.
		fopen_s(&file, path, "wt, ccs=UTF-16LE");
		for (int i = 0; i < wcslen(text); i++)
		{
			fputwc(text[i], file);
		}
		fclose(file);
	}
	// UTF-16 BigEndian 
	//else if (BOM1 == '\xfe' && BOM2 == '\xff')
	//{
	//	fopen_s(&file, path, "rt+");

	//	// Пропускаем первые четыре символа (BOM + '\0' + '\0')
	//	for (int i = 0; i < 2; i++)
	//		fgetwc(file);

	//	wchar_t text[30000] = L"";

	//	while (!feof(file))
	//	{
	//		wchar_t str1[256] = L"";
	//		wchar_t str2[256] = L"";

	//		wchar_t w = ' ';
	//		for (int i = 0; w != '\n' && !feof(file); i++)
	//		{
	//			w = fgetwc(file);
	//			// (костыльно)
	//			if (w != '\0')
	//				str1[i] = w;
	//			else
	//				i--;
	//		}

	//		//fgetws(str1, 256, file);
	//		reverseW(str2, str1);
	//		concatW(text, text, str2);
	//	}
	//	fclose(file);

	//	// Открываем файл для записи.
	//	fopen_s(&file, path, "wt");
	//	for (int i = 0; i < wcslen(text); i++)
	//	{
	//		fputwc(text[i], file);
	//	}
	//	fclose(file);
	//}
	// UTF-8
	else if (BOM1 == '\xef' && BOM2 == '\xbb' && BOM3 == '\xbf')
	{
		fopen_s(&file, path, "rt+, ccs=UTF-8");

		wchar_t text[30000] = L"";

		while (!feof(file))
		{
			wchar_t str1[256] = L"";
			wchar_t str2[256] = L"";

			wchar_t w = ' ';
			for (int i = 0; w != '\n' && !feof(file); i++)
			{
				w = fgetwc(file);
				str1[i] = w;
			}

			//fgetws(str1, 256, file);
			reverseW(str2, str1);
			concatW(text, text, str2);
		}
		fclose(file);

		// Открываем файл для записи.
		fopen_s(&file, path, "wt, ccs=UTF-8");
		for (int i = 0; i < wcslen(text); i++)
		{
			fputwc(text[i], file);
		}
		fclose(file);
	}
	// Случай для ASCII
	else
	{
		fopen_s(&file, path, "rt");
		char text[30000] = "";

		while (!feof(file))
		{
			char str1[256] = "";
			char str2[256] = "";
			fgets(str1, 256, file);
			reverseC(str2, str1);
			concatC(text, text, str2);
		}
		fclose(file);

		// Открываем файл для записи.
		fopen_s(&file, path, "wt");
		fprintf(file, text);
		fclose(file);
	}
}

int main()
{
	// Устанавливаем локальный режим для того, чтобы выводить символы кирилицы.
	_tsetlocale(LC_ALL, _T("Russian"));

	// 1. Тип кодировки по умолчанию (ASCII, потому что в настройках был задан этот тип кодировки).
	cout << "Тип кодировки по умолчанию: ";
	cout << GetProgramEncoding() << endl;

	// 2. Определение типа кодировки по заданному макросу в командной строке.
	// Properties → C/C++ → Command Line (Unicode)

	// 3. Переключение режима задания символа на ASCII.
	// Properties → General → Character Set → Use Multi-Byte Character Set

	// 4. Проверка типа кодировки по заданному макросу в командной строке (после компиляции).
	// Properties → C/C++ → Command Line (Unicode)

	// 5. 
	char asciiNames[3][256] = 
	{ 
		"Шутеев Илья Вячеславович",
		"Шутеева Татьяна Николаевна",
		"Шутеев Вячеслав Вадимович" 
	};

	cout << "Объявлен массив в ASCII кодировке:" << endl;
	for (int i = 0; i < 3; i++)
	{
		cout << asciiNames[i] << endl;
	}
	cout << "\n";

	// 6.
	wchar_t *unicodeNames[3] = { L"",L"",L"" };
	for (int i = 0; i < 3; i++)
	{
		int len = MultiByteToWideChar(
			CP_ACP,
			0,
			asciiNames[i],
			-1,
			NULL,
			0
			);
		unicodeNames[i] = new wchar_t[len];

		MultiByteToWideChar(
			CP_ACP,
			0,
			asciiNames[i],
			-1,
			unicodeNames[i],
			len
			);
	}

	// 7.
	cout << "Массив в UNICODE кодировке:" << endl;
	for (int i = 0; i < 3; i++)
	{
		//_tprintf работает только с типом char.
		_tprintf(_T("%s\n\r"), unicodeNames[i]);

		// MessageBox работает только с типом char.
		MessageBox(0, unicodeNames[i], L"Name", MB_OK);
	}
	cout << "\n";

	// 8.
	qsort(unicodeNames, 3, sizeof(wchar_t*), wcompare);

	// 9.
	for (int i = 0; i < 3; i++)
	{
		int len = WideCharToMultiByte(
			CP_ACP,
			0,
			unicodeNames[i],
			-1,
			NULL,
			0,
			NULL,
			NULL
			);

		WideCharToMultiByte(
			CP_ACP,
			0,
			unicodeNames[i],
			-1,
			asciiNames[i],
			len,
			NULL,
			NULL
			);
	}

	// 10.
	cout << "Отсортированный массив в ASCII кодировке:" << endl;
	for (int i = 0; i < 3; i++)
	{
		cout << asciiNames[i] << endl;
	}

	//11. Считывание из файла.
	ReverceText("G:\\ОС\\Лабы\\Lab1\\Debug\\UTF-8.txt");
	ReverceText("G:\\ОС\\Лабы\\Lab1\\Debug\\UTF-16LE.txt");
	ReverceText("G:\\ОС\\Лабы\\Lab1\\Debug\\ASCII.txt");
}


