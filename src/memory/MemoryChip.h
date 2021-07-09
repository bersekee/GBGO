#pragma once

#include "../Types.h"

#include <string>

class Memory;
class Cartridge;

class MemoryChip {

public:
	virtual uint8_t read(uint16_t address) = 0;
	virtual void write(uint16_t address, uint8_t data) = 0;
	virtual void save(const std::string &name) {};
	virtual void load(const std::string &name) {};

protected:

	const std::string SAVE_FILE_EXTENSION = ".sav";

	MemoryChip(Memory* memory, Cartridge* cartridge) {
		this->memory = memory;
		this->cartridge = cartridge;
	}

	Memory* memory;
	Cartridge* cartridge;
};