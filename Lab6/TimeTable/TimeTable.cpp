// TimeTable.cpp: определяет точку входа для консольного приложения.
//

#include "stdafx.h"
#include <iostream>
#include <Windows.h>

using namespace std;

HANDLE h;

DWORD WINAPI timer(LPVOID param)
{
	while (true)
	{
		DWORD timeout = WaitForSingleObject(h, 3000);
		if (WAIT_TIMEOUT == timeout)
			break;
		PROCESS_INFORMATION pi;
		STARTUPINFO si;
		memset(&si, 0, sizeof(si));
		si.cb = sizeof(si);
		memset(&pi, 0, sizeof(pi));
		wchar_t* pr1 = new wchar_t[MAX_PATH];
		wcscpy_s(pr1, 42, L"ProducerConsumer.exe");
		CreateProcess(NULL, pr1, 0, 0, 0, CREATE_NEW_CONSOLE, 0, 0, &si, &pi);
		CloseHandle(pi.hThread);
		CloseHandle(pi.hProcess);
	}
	return 0;
}

int _tmain(int argc, _TCHAR* argv[])
{
	h = CreateWaitableTimer(0, FALSE, 0);
	LARGE_INTEGER dueTime = { 0 };
	dueTime.QuadPart = -10000000;
	SetWaitableTimer(h, &dueTime, 1000, 0, 0, FALSE);
	HANDLE thread = CreateThread(0, 0, timer, 0, 0, 0);
	CloseHandle(thread);
	Sleep(10000);
	CancelWaitableTimer(h);
	WaitForSingleObject(thread, 10000);
	CloseHandle(h);
	return 0;
}

