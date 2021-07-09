#include "MBC1.h"

#include "../Memory.h"
#include "../Cartridge.h"

#include <fstream>

MBC1::MBC1(Memory* memory, Cartridge *cartridge) : MemoryChip{ memory, cartridge }  {
	mode = Mode::ROM;

	ram = new uint8_t[RAM_BANK_SIZE * 4];
	memset(ram, 0xFF, sizeof(ram));
    ramEnabled = false;
	rom = cartridge->getRom();

    currentRAMBank = 0;
    currentROMBank = 1;
    higherRomBankBits = 0;
}

MBC1::~MBC1() {
	delete[] ram;
}

uint8_t MBC1::read(uint16_t address) {
	if (address <= 0x3FFF) {
		return memory->readDirectly(address);
	} else if (address <= 0x7FFF) {
		return readFromRomBank(address);
	} else if (address <= 0x9FFF) {
		return memory->readDirectly(address);
	} else if (address <= 0xBFFF) {
		return readFromRamBank(address);
	}
}

uint8_t MBC1::readFromRomBank(uint16_t address) {
	int32_t romAddress = currentROMBank * ROM_BANK_SIZE;
	return rom[(address - 0x4000) + romAddress];
}

uint8_t MBC1::readFromRamBank(uint16_t address) {
	if (ramEnabled) {
		if (mode == Mode::ROM) {
			return ram[address - 0xA000];
		} else {
			int32_t ramAddress = 0;
			if (currentRAMBank != 0) {
				ramAddress = currentRAMBank * RAM_BANK_SIZE;
			}
			return ram[(address - 0xA000) + ramAddress];
		}
	} else {
		return 0xFF;
	}
}

void MBC1::write(const uint16_t address, const uint8_t data) {
	if (address <= 0x1FFF) {
		if (cartridge->getRamSize() != Cartridge::RamSize::NONE_RAM) {
			ramEnabled = ((data & 0x0F) == 0x0A);
		}
	} else if (address >= 0x2000 && address <= 0x3FFF) {
		setRomBank(data);
	} else if (address >= 0x4000 && address <= 0x5FFF) {
		setRamOrUpperBitsOfRomBank(data);
	} else if (address >= 0x6000 && address <= 0x7FFF) {
		setMode(data);
	} else if (address >= 0xA000 && address <= 0xBFFF) {
		writeToRamBank(address, data);
	} else {
		memory->writeDirectly(address, data);
	}
}

void MBC1::writeToRamBank(uint16_t address, uint8_t data) {
	if (ramEnabled) {
		if (mode == Mode::ROM) {
			ram[address - 0xA000] = data;
		} else {
			uint16_t ramAddress = currentRAMBank * RAM_BANK_SIZE;
			ram[(address - 0xA000) + ramAddress] = data;
		}
	}
}

void MBC1::setRomBank(uint8_t data) {
	if (mode == Mode::ROM) {
		currentROMBank = (data & 0x1F) | (higherRomBankBits << 5);
	} else {
		currentROMBank = data & 0x1F;
	}

	for (uint8_t specialBank : SPECIAL_ROM_BANKS) {
		if (currentROMBank == specialBank) {
			currentROMBank++;
		}
	}

	currentROMBank &= (cartridge->getROMBanks() - 1);
}

void MBC1::setRamOrUpperBitsOfRomBank(uint8_t data) {
	if (mode == Mode::RAM) {
		currentRAMBank = data & 0x03;
		currentRAMBank &= (cartridge->getRamBanks() - 1);
	} else {
		higherRomBankBits = data & 0x03;
		currentROMBank = (currentROMBank & 0x1F) | (higherRomBankBits << 5);

		for (uint8_t specialBank : SPECIAL_ROM_BANKS) {
			if (currentROMBank == specialBank) {
				currentROMBank++;
			}
		}

		currentROMBank &= (cartridge->getROMBanks() - 1);
	}
}

void MBC1::setMode(uint8_t data) {
	mode = Mode(data & 0x01);
}

void MBC1::save(const std::string &name) {
	std::ofstream savefile(name + SAVE_FILE_EXTENSION, std::ofstream::binary);
	if (savefile.is_open()) {
		savefile.write(reinterpret_cast<char*>(ram), RAM_BANK_SIZE);
	}
	savefile.close();
}

void MBC1::load(const std::string &name) {
	std::ifstream savefile(name + SAVE_FILE_EXTENSION, std::ios::in | std::ios::binary | std::ios::ate);
	if (savefile.is_open()) {
		std::streampos size = savefile.tellg();
		char* binary = new char[size];
		savefile.seekg(0, std::ios::beg);
		savefile.read(binary, size);
		savefile.close();

		for (int32_t address = 0; address <= RAM_BANK_SIZE; address++) {
			ram[address] = static_cast<uint8_t>(binary[address]);
		}

		delete[] binary;
	}
	savefile.close();
}