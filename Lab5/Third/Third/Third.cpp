// Third.cpp: определяет точку входа для консольного приложения.
//

#include "stdafx.h"
#include <Windows.h>

#define CreateMineProcess(pr, flags, si, pi) CreateProcess(0, pr, 0, 0, FALSE, flags, 0, 0, si, pi)

int main()
{
	TCHAR path1[256] = _T("D:\\Учёба\\2 курс\\ОС\\Лабы\\Lab5\\First\\Debug\\Lab5.exe");
	STARTUPINFO si;
	PROCESS_INFORMATION pi;
	memset(&si, 0, sizeof(si));
	si.cb = sizeof(si);
	memset(&pi, 0, sizeof(pi));
	FILETIME startTime;
	GetSystemTimeAsFileTime(&startTime);
	BOOL b = CreateMineProcess(path1, 0, &si, &pi);
	if (!b)
	{
		_tprintf(_T("Create process1 error (%d).\n"), GetLastError());
		return NULL;
	}
	WaitForSingleObject(pi.hProcess, INFINITE);
	FILETIME endTime;
	GetSystemTimeAsFileTime(&endTime);
	CloseHandle(pi.hThread);
	CloseHandle(pi.hProcess);

	TCHAR *l1 = (TCHAR*)&startTime.dwLowDateTime;
	TCHAR *l2 = (TCHAR*)&endTime.dwLowDateTime;
	TCHAR *h1 = (TCHAR*)&startTime.dwHighDateTime;
	TCHAR *h2 = (TCHAR*)&endTime.dwHighDateTime;
	TCHAR path2[256] = _T("D:\\Учёба\\2 курс\\ОС\\Лабы\\Lab5\\Second\\Debug\\Second.exe \"");
	wcscat_s(path2, 256, l1);
	wcscat_s(path2, 256, L"\" \"");
	wcscat_s(path2, 256, h1);
	wcscat_s(path2, 256, L"\" \"");
	wcscat_s(path2, 256, l2);
	wcscat_s(path2, 256, L"\" \"");
	wcscat_s(path2, 256, h2);
	wcscat_s(path2, 256, L"\"");
	b = CreateMineProcess(path2, BELOW_NORMAL_PRIORITY_CLASS, &si, &pi);
	if (!b)
	{
		_tprintf(_T("Create process1 error (%d).\n"), GetLastError());
		return NULL;
	}
	WaitForSingleObject(pi.hProcess, INFINITE);
	CloseHandle(pi.hThread);
	CloseHandle(pi.hProcess);
    return 0;
}

