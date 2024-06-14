#include <Windows.h>
#include <TlHelp32.h>
#include <vector>


class Memory
{
private:
	uintptr_t	processID	= 0;
	HANDLE		hProcess	= nullptr;

public:
	Memory(const wchar_t* processName)
	{
		HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

		if (hSnap != INVALID_HANDLE_VALUE)
		{
			PROCESSENTRY32 processEntry;
			processEntry.dwSize = sizeof(processEntry);

			if (Process32First(hSnap, &processEntry))
			{
				do {
					if (!_wcsicmp(processEntry.szExeFile, processName))
					{
						processID = processEntry.th32ProcessID;
						hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, processID);
						break;
					}
				} while (Process32Next(hSnap, &processEntry));
			}
		}

		if (hSnap)
		{
			CloseHandle(hSnap);
		}
	}
	~Memory()
	{
		if (hProcess)
		{
			CloseHandle(hProcess);
		}
	}

	uintptr_t GetModuleBaseAddress(const wchar_t* moduleName)
	{
		uintptr_t moduleBaseAddress = 0;
		HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, processID);

		if (hSnap != INVALID_HANDLE_VALUE)
		{
			MODULEENTRY32 moduleEntry;
			moduleEntry.dwSize = sizeof(moduleEntry);
			if (Module32First(hSnap, &moduleEntry))
			{
				do {
					if (!_wcsicmp(moduleEntry.szModule, moduleName))
					{
						moduleBaseAddress = (uintptr_t)moduleEntry.modBaseAddr;
						break;
					}
				} while (Module32Next(hSnap, &moduleEntry));
			}
		}

		if (hSnap)
		{
			CloseHandle(hSnap);
		}

		return moduleBaseAddress;
	}

	/*
	* return value
	*/
	uintptr_t FindDMAAddy(uintptr_t address, std::vector<unsigned int>offsets)
	{
		for (unsigned int i = 0; i < offsets.size(); i++)
		{
			address += offsets[i];
			address = Read<uintptr_t>(address);
		}
		return address;
	}

	template <typename T>
	constexpr const T Read(const uintptr_t& address) const noexcept
	{
		T value = {};
		ReadProcessMemory(hProcess, reinterpret_cast<void*>(address), &value, sizeof(T), nullptr);
		return value;
	}

	template <typename T>
	constexpr void Write(const uintptr_t& address, const T& value) const noexcept
	{
		WriteProcessMemory(hProcess, reinterpret_cast<void*>(address), &value, sizeof(T), nullptr);
	}
};
