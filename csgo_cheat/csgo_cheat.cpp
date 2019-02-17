// csgo_cheat.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//

#include "pch.h"
#include <iostream>
#include "memory.h" //So we can use MemoryManager class

MemoryManager* Mem; //We declare globally the MemoryManager class, but its constructor must be executed in the main entry point!

//OFFSETS
#define m_dwLocalPlayer 0xCCA6A4
#define m_iTeamNum 0xF4
#define m_iCrossHairID 0xB394
#define m_dwEntityList 0x4CDB00C
#define m_bDormant 0xED


void Trigger()
{
	bool TriggerToggled = false;

	while (true)
	{
		//Check for toggle on/off
		if (GetAsyncKeyState(VK_F1)) //You can customize the toggle key
		{
			TriggerToggled = !TriggerToggled;
			if (TriggerToggled) std::cout << "Awesome triggerbot now ON" << std::endl;
			else std::cout << "Awesome triggerbot now OFF" << std::endl;
			Sleep(200);
		}
		//If the triggerbot is not toggled, skip the iteration
		if (!TriggerToggled) continue;

		//Retrieve player informations
		DWORD LocalPlayer_Base = Mem->Read<DWORD>(Mem->ClientDLL_Base + m_dwLocalPlayer);
		int LocalPlayer_inCross = Mem->Read<int>(LocalPlayer_Base + m_iCrossHairID);
		int LocalPlayer_Team = Mem->Read<int>(LocalPlayer_Base + m_iTeamNum);

		//Retrieve the EntityBase, using dwEntityList
		DWORD Trigger_EntityBase = Mem->Read<DWORD>(Mem->ClientDLL_Base + m_dwEntityList + ((LocalPlayer_inCross - 1) * 0x10));
		int Trigger_EntityTeam = Mem->Read<int>(Trigger_EntityBase + m_iTeamNum);

		//bDormant is a boolean value
		bool Trigger_EntityDormant = Mem->Read<bool>(Trigger_EntityBase + m_bDormant);

		if ((LocalPlayer_inCross > 0 && LocalPlayer_inCross <= 64) && (Trigger_EntityBase != NULL) && (Trigger_EntityTeam != LocalPlayer_Team) && (!Trigger_EntityDormant))
		{
			//External way, you can customize the delays
			Sleep(10); //Delay before shooting
			mouse_event(MOUSEEVENTF_LEFTDOWN, NULL, NULL, NULL, NULL);
			Sleep(10); //Delay between shots
			mouse_event(MOUSEEVENTF_LEFTUP, NULL, NULL, NULL, NULL);
			Sleep(10); //Delay after shooting
		}
	}
}


int main()
{
	Mem = new MemoryManager(); //Execute the constructor code
	std::cout << "My handsome first triggerbot!" << std::endl;
	Trigger();
	delete Mem; //Don't forget to delete MemoryManager pointer, so it executes destructor and closes correctly the handle
	return 0;
}



// Запуск программы: CTRL+F5 или меню "Отладка" > "Запуск без отладки"
// Отладка программы: F5 или меню "Отладка" > "Запустить отладку"

// Советы по началу работы 
//   1. В окне обозревателя решений можно добавлять файлы и управлять ими.
//   2. В окне Team Explorer можно подключиться к системе управления версиями.
//   3. В окне "Выходные данные" можно просматривать выходные данные сборки и другие сообщения.
//   4. В окне "Список ошибок" можно просматривать ошибки.
//   5. Последовательно выберите пункты меню "Проект" > "Добавить новый элемент", чтобы создать файлы кода, или "Проект" > "Добавить существующий элемент", чтобы добавить в проект существующие файлы кода.
//   6. Чтобы снова открыть этот проект позже, выберите пункты меню "Файл" > "Открыть" > "Проект" и выберите SLN-файл.
