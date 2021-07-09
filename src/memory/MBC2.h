#pragma once

#include "MemoryChip.h"

class Memory;

class MBC2 : public MemoryChip {
	public:
		MBC2(Memory* memory, Cartridge* cartridge);
		~MBC2();

		uint8_t read(uint16_t address) final override;
		void write(uint16_t address, uint8_t data) final override;
		void save(const std::string &name) override;
		void load(const std::string &name) override;

	private:
		const uint16_t RAM_SIZE				= 0x200;
		const uint16_t ROM_BANK_SIZE		= 0x4000;

		uint8_t* ram;
		uint8_t currentROMBank;
		bool ramEnabled;

		uint8_t readFromRam(uint16_t address);
		uint8_t readFromRomBank(uint16_t address);
		void setRomBank(uint16_t address, uint8_t data);
		void setRamState(uint16_t address);
};
