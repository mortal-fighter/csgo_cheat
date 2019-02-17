#pragma once //With this pragma we can tell the compiler to compile only once this header

#include <Windows.h> //Windows.h contains the functions that we want to use to read/write memory

#include <TlHelp32.h> //TlHelp32.h contains some functions useful when we want to get modules (dlls) of CS:GO

class MemoryManager
{
	private:
		DWORD dwPID; //The ProcessID of CS:GO process
		HANDLE hProcess; //The handle of CS:GO process
	public:
		MODULEENTRY32 ClientDLL;
		MODULEENTRY32 EngineDLL;
		DWORD ClientDLL_Base, ClientDLL_Size;
		DWORD EngineDLL_Base, EngineDLL_Size;
	
	/*
		These 2 functions can seem a real monster to understand, but it's actually easy.
		The AttachProcess function converts the char array containing the process name (in our case "csgo.exe") into a wchar_t and then loop using do while until it finds a corresponding process to the process name. When it finds it, assign the process id and the handle to our private members, then returns true, meaning success.
		The literally same concept is applied in the GetModule function. In this case, if no module is found, it returns a module entry with the module base 0x0.
	*/
	bool AttachProcess(const char* ProcessName)
	{
		HANDLE hPID = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL); //Takes a snapshot of the specified processes, as well as the heaps, modules, and threads used by these processes.
		PROCESSENTRY32 procEntry;
		procEntry.dwSize = sizeof(procEntry);

		const WCHAR* procNameChar;
		int nChars = MultiByteToWideChar(CP_ACP, 0, ProcessName, -1, NULL, 0);
		procNameChar = new WCHAR[nChars];
		MultiByteToWideChar(CP_ACP, 0, ProcessName, -1, (LPWSTR)procNameChar, nChars);

		do
			if (!wcscmp(procEntry.szExeFile, procNameChar))
			{
				this->dwPID = procEntry.th32ProcessID;
				CloseHandle(hPID);
				this->hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, this->dwPID);
				return true;
			}
		while (Process32Next(hPID, &procEntry));

		CloseHandle(hPID);
		return false;
	}

	MODULEENTRY32 GetModule(const char* ModuleName)
	{
		HANDLE hModule = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, dwPID);
		MODULEENTRY32 mEntry;
		mEntry.dwSize = sizeof(mEntry);

		const WCHAR *modNameChar;
		int nChars = MultiByteToWideChar(CP_ACP, 0, ModuleName, -1, NULL, 0);
		modNameChar = new WCHAR[nChars];
		MultiByteToWideChar(CP_ACP, 0, ModuleName, -1, (LPWSTR)modNameChar, nChars);

		do
			if (!wcscmp(mEntry.szModule, modNameChar))
			{
				CloseHandle(hModule);
				return mEntry;
			}
		while (Module32Next(hModule, &mEntry));

		CloseHandle(hModule);
		mEntry.modBaseAddr = 0x0;
		return mEntry;
	}

	template<class c>
	c Read(DWORD dwAddress)
	{
		c val;
		ReadProcessMemory(hProcess, (LPVOID)dwAddress, &val, sizeof(c), NULL);
		return val;
	}

	template<class c>
	BOOL Write(DWORD dwAddress, c ValueToWrite)
	{
		return WriteProcessMemory(hProcess, (LPVOID)dwAddress, &ValueToWrite, sizeof(c), NULL);
	}

	DWORD GetProcID() { return this->dwPID; }
	HANDLE GetProcHandle() { return this->hProcess; }

	MemoryManager()
	{
		this->hProcess = NULL;
		this->dwPID = NULL;
		try {
			if (!AttachProcess("csgo.exe")) throw 1;
			this->ClientDLL = GetModule("client_panorama.dll");
			this->EngineDLL = GetModule("engine.dll");
			this->ClientDLL_Base = (DWORD)this->ClientDLL.modBaseAddr;
			this->EngineDLL_Base = (DWORD)this->EngineDLL.modBaseAddr;
			if (this->ClientDLL_Base == 0x0) throw 2;
			if (this->EngineDLL_Base == 0x0) throw 3;
			this->ClientDLL_Size = this->ClientDLL.dwSize;
			this->EngineDLL_Size = this->EngineDLL.dwSize;
		}
		catch (int iEx) {
			switch (iEx)
			{
			case 1: MessageBoxA(NULL, "CS:GO must be running", "ERROR", MB_ICONSTOP | MB_OK); exit(0); break;
			case 2: MessageBoxA(NULL, "Couldn't find Client.dll", "ERROR", MB_ICONSTOP | MB_OK); exit(0); break;
			case 3: MessageBoxA(NULL, "Couldn't find Engine.dll", "ERROR", MB_ICONSTOP | MB_OK); exit(0); break;
			default: throw;
			}
		}
		catch (...) {
			MessageBoxA(NULL, "Unhandled exception thrown", "ERROR", MB_ICONSTOP | MB_OK);
			exit(0);
		}
	}

	~MemoryManager()
	{
		CloseHandle(this->hProcess);
	}

};