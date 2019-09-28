#include "GameBoy.h"

//GameBoy::GameBoy(std::function<void(std::vector<sf::Color>&)> draw) : cpu(mmu), ppu(cpu, mmu), mmu(cpu, ppu)
GameBoy::GameBoy(std::vector<unsigned char> cartridgeData) : cartridge(cartridgeData), cpu(mmu), ppu(cpu, mmu), mmu(cpu, ppu, cartridge)
{

}

GameBoy::~GameBoy()
{

}

bool GameBoy::loadGame(std::string fileName)
{
	std::ifstream ROM_File;
	int memoryOffset = 0x0000;
	char currentVal;
	bool status;

	ROM_File.open(fileName, std::ios::binary);

	if (!ROM_File.is_open())
	{
		//std::cout << "Could not open ROM: " << fileName << std::endl;
		status = false;
	}

	else
	{
		while (memoryOffset < 0x0100)		//The first 16K of ROM is always loaded into memory starting at 0x0000
		{
			ROM_File.get(currentVal);
			//memory[memoryOffset] = (unsigned char)currentVal;
			mmu.writeMemory(memoryOffset, (unsigned char)currentVal);
			memoryOffset++;
		}

		switch(mmu.readMemory(0x0147))
		//switch (mmu.memory[0x0147])		//Check which Memory Bank Controller (MBC) we need to use
		{
			case 0x00:		//No MBC (all ROM data is mapped directly into memory)
				while (ROM_File.get(currentVal))
				{
					//memory[memoryOffset] = (unsigned char)currentVal;
					mmu.writeMemory(memoryOffset, (unsigned char)currentVal);
					memoryOffset++;
				}
				break;

			case 0x01:
				break;

			default:
				break;
		}

		status = true;
	}

	ROM_File.close();

	return status;
}

void GameBoy::pressButton(int buttonIndex)
{
	mmu.writeMemory(0xFF0F, mmu.readMemory(0xFF0F) | 0x10);

	switch (buttonIndex)
	{
		case 0:		//Up
			mmu.writeMemory(0xFF00, 0xEB);
			break;

		case 1:		//Down
			mmu.writeMemory(0xFF00, 0xE7);
			break;

		case 2:		//Left
			mmu.writeMemory(0xFF00, 0xED);
			break;

		case 3:		//Right
			mmu.writeMemory(0xFF00, 0xEE);
			break;

		case 4:		//A
			mmu.writeMemory(0xFF00, 0xDE);
			break;

		case 5:		//B
			mmu.writeMemory(0xFF00, 0xDD);
			break;

		case 6:		//Start
			mmu.writeMemory(0xFF00, 0xD7);
			break;

		case 7:		//Select
			mmu.writeMemory(0xFF00, 0xDB);
			break;

		case 8:		//No button pressed
			//mmu.writeMemory(0xFF00, 0xFF);
			break;

		default:
			break;
	}
}

//void GameBoy::powerOn(std::function<void(std::vector<sf::Color>&)> draw)
void GameBoy::powerOn(std::function <bool()> checkWindow, std::function<void(const Buffer &buffer)> draw)
{
	ppu.setupDrawFunction(draw);

	/*
		int cyclesElapsed = cpu.tick();
		ppu.tick(cyclesElapsed);
		cpu.updateTimer(cyclesElapsed);
	*/

	while (checkWindow())
	{
		int cyclesElapsed = cpu.tick();

		ppu.tick(cyclesElapsed);

		cpu.updateTimer(cyclesElapsed);
	}
}