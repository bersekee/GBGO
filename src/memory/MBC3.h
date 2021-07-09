#pragma once

#include "MemoryChip.h"

class Memory;

class MBC3 : public MemoryChip {
	public:
		MBC3(Memory* memory, Cartridge* cartridge);
		~MBC3();

		uint8_t read(uint16_t address) final override;
		void write(uint16_t address, uint8_t data) final override;
		void save(const std::string &name) override;
		void load(const std::string &name) override;

	private:
		const uint8_t SPECIAL_ROM_BANKS[4] = {0x00};
		const uint16_t RAM_BANK_SIZE		= 0x2000;
		const uint16_t ROM_BANK_SIZE		= 0x4000;

		uint8_t currentRAMBank;
		uint8_t currentROMBank;

		uint8_t* ram;
		bool ramEnabled;

		uint8_t readFromRamBank(uint16_t address);
		uint8_t readFromRomBank(uint16_t address);

		void writeToRamBank(uint16_t address, uint8_t data);
		void setRomBank(uint8_t data);
		void setRamBank(uint8_t data);
};
