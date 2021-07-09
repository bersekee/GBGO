#include "MBC2.h"

#include "../Memory.h"
#include "../Cartridge.h"
#include "../Utils.h"

MBC2::MBC2(Memory* memory, Cartridge *cartridge) : MemoryChip{ memory, cartridge }  {
	ram = new uint8_t[RAM_SIZE];
	memset(ram, 0xFF, sizeof(ram));

    ramEnabled = false;
    currentROMBank = 1;
}

MBC2::~MBC2() {
	delete[] ram;
}

uint8_t MBC2::read(uint16_t address) {
	if (address <= 0x3FFF) {
		return memory->readDirectly(address);
	} else if (address <= 0x7FFF) {
		return readFromRomBank(address);
	} else if (address <= 0x97FF) {
		return memory->readDirectly(address);
	} else if (address <= 0xA1FF) {
		readFromRam(address);
	} else {
		return memory->readDirectly(address);
	}
}

uint8_t MBC2::readFromRomBank(uint16_t address) {
	int32_t romAddress = currentROMBank * ROM_BANK_SIZE;
	uint8_t* rom = cartridge->getRom();
	return rom[(address - 0x4000) + romAddress];
}

uint8_t MBC2::readFromRam(uint16_t address) {
	if (ramEnabled) {
		return ram[(address - 0xA000)];
	} else {
		return 0xFF;
	}
}

void MBC2::write(const uint16_t address, const uint8_t data) {
	if (address <= 0x1FFF) {
		setRamState(address);
	} else if (address >= 0x2000 && address <= 0x3FFF) {
		setRomBank(address, data);
	} else if (address >= 0xA000 && address <= 0xA1FF) {
		if (ramEnabled) {
			ram[(address - 0xA000)] = data;
		}
	} else if (address >= 0x8000 && address <= 0xBFFF) {
		memory->writeDirectly(address, data);
	}
}
void MBC2::setRomBank(uint16_t address, uint8_t data) {
	uint8_t upperuint8_t = (address >> 8);
	if (isBitSet(upperuint8_t, 0)) {
		currentROMBank = data & 0x0F;
	}
}

void MBC2::setRamState(uint16_t address) {
	uint8_t upperuint8_t = (address >> 8);
	if (!isBitSet(upperuint8_t, 0)) {
		ramEnabled = !ramEnabled;
	}
}

void MBC2::save(const std::string &name) {
	std::ofstream savefile(name + SAVE_FILE_EXTENSION, std::ofstream::binary);
	if (savefile.is_open()) {
		savefile.write(reinterpret_cast<char*>(ram), RAM_SIZE);
	}
	savefile.close();
}

void MBC2::load(const std::string &name) {
	std::ifstream savefile(name + SAVE_FILE_EXTENSION, std::ios::in | std::ios::binary | std::ios::ate);
	if (savefile.is_open()) {
		std::streampos size = savefile.tellg();
		char* binary = new char[size];
		savefile.seekg(0, std::ios::beg);
		savefile.read(binary, size);
		savefile.close();

		for (int32_t address = 0; address <= RAM_SIZE; address++) {
			ram[address] = static_cast<uint8_t>(binary[address]);
		}

		delete[] binary;
	}
	savefile.close();
}