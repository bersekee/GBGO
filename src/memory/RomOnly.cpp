#include "RomOnly.h"

#include "../Memory.h"
#include "../Cartridge.h"

RomOnly::RomOnly(Memory* memory, Cartridge* cartridge) : MemoryChip(memory, cartridge) {}

uint8_t RomOnly::read(uint16_t address) {
	if ( address >= 0xA000 && address <= 0xBFFF ) {
        if ( cartridge->getRamSize() != Cartridge::RamSize::NONE_RAM ) {
            return memory->readDirectly(address);
        } else {
            return 0xFF;
        }
    } else {
        return memory->readDirectly(address);
    }
}

void RomOnly::write(uint16_t address, uint8_t data) {
    if ( address < 0x8000 ) {
        // trapped address
    } else if ( address >= 0xA000 && address < 0xBFFF) {
        if ( cartridge->getRamSize() != Cartridge::RamSize::NONE_RAM ) {
            memory->writeDirectly(address, data);
        }
    } else {
		memory->writeDirectly(address, data);
    }
}