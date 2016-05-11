// Lab5.cpp: определяет точку входа для консольного приложения.
//

#include "stdafx.h"
#include <Windows.h>
#include <iostream>

#define CreateMineProcess(pr,flags,si,pi) CreateProcess(0, pr, 0, 0, FALSE, flags, 0, 0, si, pi)

using namespace std;

int main()
{
	STARTUPINFO si;
	PROCESS_INFORMATION pi;
	memset(&si, 0, sizeof(si));
	si.cb = sizeof(si);
	memset(&pi, 0, sizeof(pi));

	TCHAR *pr = new TCHAR[MAX_PATH];
	_tcscpy(pr, _T("Notepad"));
	BOOL b = CreateMineProcess(pr, 0, &si, &pi);
	cout << b << endl;
	WaitForSingleObject(pi.hProcess, INFINITE);
	CloseHandle(pi.hThread);
	CloseHandle(pi.hProcess);
    return 0;
}

