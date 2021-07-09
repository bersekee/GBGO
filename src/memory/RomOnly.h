#pragma once

#include "MemoryChip.h"

class Cartridge;
class Memory;

class RomOnly : public MemoryChip {
	public:
		RomOnly(Memory*, Cartridge *);
		virtual uint8_t read(uint16_t address) override;
		virtual void write(uint16_t address, uint8_t data) override;
};
