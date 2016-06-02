// TimeTable.cpp: определяет точку входа для консольного приложения.
//

#define _CRT_SECURE_NO_WARNINGS

#include "stdafx.h"
#include <iostream>
#include <Windows.h>

using namespace std;

TCHAR** buf = new TCHAR*[5]();
HANDLE* processes = new HANDLE[5]();

int _tmain(int argc, _TCHAR* argv[])
{
	HANDLE h = CreateWaitableTimer(0, TRUE, L"timer");
	LARGE_INTEGER dueTime = { 0 };
	dueTime.QuadPart = -10000000;
	SetWaitableTimer(h, &dueTime, 1000, 0, 0, TRUE);
	//HANDLE thread = CreateThread(0, 0, timer, 0, 0, 0);
	for (int i = 0; i < 5; i++)
	{
		//DWORD timeout = WaitForSingleObject(h, 3000);
		//if (WAIT_TIMEOUT == timeout)
			//break;
		PROCESS_INFORMATION pi;
		STARTUPINFO si;
		memset(&si, 0, sizeof(si));
		si.cb = sizeof(si);
		memset(&pi, 0, sizeof(pi));
		wchar_t* pr1 = new wchar_t[MAX_PATH];
		wcscpy_s(pr1, 42, L"Philosophers.exe \"");
		buf[i] = new TCHAR[10];

		_stprintf_s(buf[i], 10, TEXT("%d"), i + 1);
		wcscat_s(pr1, 256, buf[i]);
		wcscat_s(pr1, 256, L"\"");
		CreateProcess(NULL, pr1, 0, 0, 0, 0, 0, 0, &si, &pi);
		processes[i] = pi.hProcess;
		//WaitForSingleObject(pi.hProcess, 1000);
		//CloseHandle(pi.hThread);
		//CloseHandle(pi.hProcess);
	}
	WaitForMultipleObjects(5, processes, TRUE, INFINITE);
	CancelWaitableTimer(h);
	CloseHandle(h);
	return 0;
}

