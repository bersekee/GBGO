#pragma once

#include "MemoryChip.h"

class Memory;

class MBC1 : public MemoryChip {
	public:
		MBC1(Memory* memory, Cartridge* cartridge);
		~MBC1();

		uint8_t read(uint16_t address) final override;
		void write(uint16_t address, uint8_t data) final override;
		void save(const std::string &name) override;
		void load(const std::string &name) override;

	private:
		const uint8_t SPECIAL_ROM_BANKS[4] = {0x00, 0x20, 0x40, 0x60};
		const uint16_t RAM_BANK_SIZE		= 0x2000;
		const uint16_t ROM_BANK_SIZE		= 0x4000;

		enum class Mode : int32_t {
			ROM = 0,
			RAM = 1
		};

		Mode mode;
		uint8_t currentRAMBank;
		uint8_t currentROMBank;
		uint8_t higherRomBankBits;

		uint8_t* ram;
		uint8_t* rom;
		bool ramEnabled;

		uint8_t readFromRamBank(uint16_t address);
		uint8_t readFromRomBank(uint16_t address);

		void writeToRamBank(uint16_t address, uint8_t data);
		void setRomBank(uint8_t data);
		void setRamOrUpperBitsOfRomBank(uint8_t data);
		void setMode(uint8_t data);
};
