// Lab5.cpp: определяет точку входа для консольного приложения.
//

#include "stdafx.h"
#include <Windows.h>
#include <iostream>
#include <TlHelp32.h>

#define CreateMineProcess(pr,flags,si,pi) CreateProcess(0, pr, 0, 0, FALSE, flags, 0, 0, si, pi)

using namespace std;

void PrintProcesses()
{
	HANDLE h = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (h != INVALID_HANDLE_VALUE)
	{
		PROCESSENTRY32 pe;
		pe.dwSize = sizeof(pe);
		BOOL haveProcess = Process32First(h, &pe);
		MODULEENTRY32 me;
		me.dwSize = sizeof(me);
		BOOL mod;
		HANDLE hProc;

		while (haveProcess)
		{
			hProc = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, pe.th32ProcessID);
			wcout << L"Name: " << pe.szExeFile << endl;
			wcout << L"Used dll: " << endl;
			mod = Module32First(hProc, &me);
			while (mod)
			{
				wprintf_s(me.szExePath);
				wprintf_s(L"\n");
				mod = Module32Next(hProc, &me);
			}
			wcout << L"Threads count: " << pe.cntThreads << endl;
			haveProcess = Process32Next(h, &pe);
			CloseHandle(hProc);

			// Чтобы поочерёдно выводить все процессы.
			getchar();
		}
	}
	CloseHandle(h);
}

int main()
{
	PrintProcesses();

	STARTUPINFO si;
	PROCESS_INFORMATION pi;
	memset(&si, 0, sizeof(si));
	si.cb = sizeof(si);
	memset(&pi, 0, sizeof(pi));

	TCHAR *pr = new TCHAR[MAX_PATH];
	_tcscpy(pr, _T("Notepad"));
	BOOL b = CreateMineProcess(pr, 0, &si, &pi);
	if (!b)
	{
		cout << "Notepad programm cann't be run." << endl;
	}
	WaitForSingleObject(pi.hProcess, INFINITE);
	CloseHandle(pi.hThread);
	CloseHandle(pi.hProcess);
    return 0;
}

