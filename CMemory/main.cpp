#include "memory.h"
#include <iostream>

constexpr auto localPlayer = 0x0018AC00;
constexpr auto health = 0xEC;
constexpr auto armor = 0xF0;

int main()
{
	Memory mem(L"ac_client.exe");

	const auto moduleBaseAddress = mem.GetModuleBaseAddress(L"ac_client.exe");
	std::cout << "Module Base Address: " << std::hex << moduleBaseAddress << std::endl;

	const auto localPlayerAddress = mem.Read<uintptr_t>(moduleBaseAddress + localPlayer);

	std::cout << "Health:" << std::dec << mem.Read<uintptr_t>(localPlayerAddress + health) << std::endl;
	std::cout << "Armor:" << std::dec << mem.Read<uintptr_t>(localPlayerAddress + armor) << std::endl;
	std::cout << "Ammo:" << std::dec << mem.FindDMAAddy(localPlayerAddress, { 0x364, 0x14, 0x0 }) << std::endl;

	while (true)
	{
		mem.Write<int>(localPlayerAddress + health, 1337);
	}

	return 0;
}