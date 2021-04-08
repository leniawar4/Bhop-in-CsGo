#include <iostream>
#include <Windows.h>
#include <TlHelp32.h>
#include <string>
#include "About.h"

uintptr_t dwLocalPlayer = 0xD8C2CC;
uintptr_t m_fFlags = 0x104;
uintptr_t dwForceJump = 0x524DEBC;
DWORD procId;
HANDLE hProcess;

uintptr_t GetModuleBaseAddress(const char* modName) {
	HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, procId);
	if (hSnap != INVALID_HANDLE_VALUE) {
		MODULEENTRY32 modEntry;
		modEntry.dwSize = sizeof(modEntry);
		if (Module32First(hSnap, &modEntry)) {
			do {
				if (!strcmp(modEntry.szModule, modName)) {
					CloseHandle(hSnap);
					return (uintptr_t)modEntry.modBaseAddr;
				}
			} while (Module32Next(hSnap, &modEntry));
		}
	}
}

template<typename T> T RPM(SIZE_T address) {
	T buffer;
	ReadProcessMemory(hProcess, (LPCVOID)address, &buffer, sizeof(T), NULL);
	return buffer;
}

template<typename T> void WPM(SIZE_T address, T buffer) {
	WriteProcessMemory(hProcess, (LPVOID)address, &buffer, sizeof(buffer), NULL);
}

int main()
{
	TCHAR szExeFileName[MAX_PATH];
	GetModuleFileName(NULL, szExeFileName, MAX_PATH);
	std::string path = std::string(szExeFileName);
	std::string exe = path.substr(path.find_last_of("\\") + 1, path.size());

	srand(time(0));
	char letters[] = "01V8YutSgDmzEX8pK3gimydac1Sn2eWa9g3z";
	char newname[17];

	int z = rand() % 5 + 5;
	for (int i = 0; i < z; i++)
	{
		char x = letters[rand() % 36];
		newname[i] = x;
	}
	newname[z] = 0x0;
	strcat_s(newname, ".exe\0");
	rename(exe.c_str(), newname);

	SetConsoleTitle("Huka Bhop");

	HWND GameWnd = FindWindowA(NULL, "Counter-Strike: Global Offensive");
	GetWindowThreadProcessId(GameWnd, &procId);
	uintptr_t moduleBase = GetModuleBaseAddress("client.dll");
	hProcess = OpenProcess(PROCESS_ALL_ACCESS, NULL, procId);

	uintptr_t LocalPlayer = NULL;
	int Flag = NULL;

	if (!procId) 
	{
		std::cout << "Process not Find\n";
		Sleep(1500);
		exit(0);
	}

	ABOUT::Banner();

	std::cout << "\tBhop Work\n";

	while (true)
	{
		if (GetAsyncKeyState(VK_SPACE)) 
		{
			LocalPlayer = RPM<uintptr_t>(moduleBase+ dwLocalPlayer);
			Flag = RPM<int>(LocalPlayer + m_fFlags);
			
			if (Flag & (1 << 0))
			{
				int jump = 0x5;
				WPM<int>(moduleBase + dwForceJump, jump);
				Sleep(20);
				jump = 0x4;
				WPM<int>(moduleBase + dwForceJump, jump);
			}
			Sleep(2);
		}

	}

	return 0;
}
