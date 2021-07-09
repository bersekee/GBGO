#include "Memory.h"

#include "Cartridge.h"
#include "Joypad.h"
#include "CPU.h"
#include "memory/MemoryChip.h"
#include "memory/MBC1.h"
#include "memory/MBC2.h"
#include "memory/MBC3.h"
#include "memory/RomOnly.h"
#include "Video.h"

#include <iostream>

MemoryChip* Memory::createMemoryChipForCartridge(Memory* memory, Cartridge* cartridge) {
	MemoryChip* chip;

	switch (cartridge->getCartridgeType()) {
		case Cartridge::CartridgeType::MBC3:					chip = new  MBC3(memory, cartridge);		break;
		case Cartridge::CartridgeType::MBC3_RAM:				chip = new  MBC3(memory, cartridge);		break;
		case Cartridge::CartridgeType::MBC3_RAM_BATTERY:		chip = new  MBC3(memory, cartridge);		break;
		case Cartridge::CartridgeType::MBC3_TIMER_BATTERY:		chip = new  MBC3(memory, cartridge);		break;
		case Cartridge::CartridgeType::MBC3_TIMER_RAM_BATTERY:	chip = new  MBC3(memory, cartridge);		break;
		case Cartridge::CartridgeType::MBC2_BATTERY:			chip = new  MBC2(memory, cartridge);		break;
		case Cartridge::CartridgeType::MBC2:					chip = new  MBC2(memory, cartridge);		break;
		case Cartridge::CartridgeType::MBC1:                    chip = new  MBC1(memory, cartridge);		break;
		case Cartridge::CartridgeType::MBC1_RAM:                chip = new  MBC1(memory, cartridge);		break;
		case Cartridge::CartridgeType::MBC1_RAM_BATTERY:        chip = new  MBC1(memory, cartridge);		break;
		case Cartridge::CartridgeType::ROM_RAM:                 chip = new  RomOnly(memory, cartridge);		break;
		case Cartridge::CartridgeType::ROM_RAM_BATTERY:         chip = new  RomOnly(memory, cartridge);		break;
		case Cartridge::CartridgeType::ROM_ONLY:				chip = new  RomOnly(memory, cartridge);		break;
		default:												std::cout << "Memory chip not supported yet" << std::endl; exit(-1); break;
	}

	return chip;
}

Memory::Memory(Cartridge *cartridge, Joypad *joypad) {
    this->map = new uint8_t[0x10000];
	memset(map, 0, 0x10000);
    this->joypad = joypad;

    this->cartridge = cartridge;
    hardReset();
}

void Memory::init(CPU * cpu, Video *video) {
    this->cpu = cpu;
	this->video = video;
}

uint8_t Memory::read(const uint16_t address) const {
	if (address <= 0xBFFF) {
		return chip->read(address);
	} else if (address == 0xFF00) {
		return joypad->getState();
	} else {
		return readDirectly(address);
	}
}

void Memory::write(const uint16_t address, const uint8_t data) {
    if (address <= 0xBFFF) {
		chip->write(address, data);
    } else if ( (address >= 0xC000) && (address <= 0xDFFF) ){
        map[address] = data;
		if (address <= 0xDDFF) {
			map[address + 0x2000] = data;
		}
    } else if ( (address > 0xE000) && (address <= 0xFDFF) ) {
        map[address] = data;
        map[address - 0x2000] = data;
    } else if ((address >= 0xFEA0) && (address <= 0xFEFF)) {
        // Not allowed
    } else if (address == 0xFF04) {
        map[address] = 0x0;
        cpu->resetDivRegister();
    }else if (address == 0xFF07) {
        map[address] = data;

        switch(data & 0x03) {
            case 0:  cpu->setCurrentClockSpeed(1024);  break;
            case 1:  cpu->setCurrentClockSpeed(16);    break;
            case 2:  cpu->setCurrentClockSpeed(64);    break;
			case 3:  cpu->setCurrentClockSpeed(256);   break;
            default: /*assert(false);*/  break;
        }

	} else if (address == Video::LCD_CONTROL) {
		uint8_t currentLCDStatus = map[address];
		if (!isBitSet(data, 7) && video->isLCDEnabled()) {
			video->DisableLCD();
		} else if (isBitSet(data, 7) && !video->isLCDEnabled()) {
			video->EnableLCD();
		}
		map[address] = data;
	} else if (address == Video::LCDC_STATUS) {
		// Don't allow the override of values in bits [0,2]
		uint8_t lcdcStatus = map[address];
		uint8_t relevantData = data & 0x78;
		uint8_t currentLCDCStatus = relevantData | (lcdcStatus & 0x07);
		map[address] = currentLCDCStatus;

		// Check the current state of the PPU and request interrupt if needed
		if (video->isLCDEnabled()) {
			Video::Mode currentMode = video->getCurrentMode();
			bool shouldRequestInterrupt = false;
			switch (currentMode) {
				case Video::Mode::H_BLANK:
					shouldRequestInterrupt = isBitSet(currentLCDCStatus, 3);
					break;
				case Video::Mode::V_BLANK:
					shouldRequestInterrupt = isBitSet(currentLCDCStatus, 4);
					break;
				case Video::Mode::OAM_RAM:
					shouldRequestInterrupt = isBitSet(currentLCDCStatus, 5);
					break;
			}

			if (shouldRequestInterrupt) {
				cpu->requestInterrupt(Interrupts::LCD);
			}
		}
    } else if (address == 0xFF46){
		map[0xFF46] = data;
		DMA(data);
    } else if ((address >= 0xFF4C) && (address <= 0xFF7F)) {
        // Not allowed
    } else if ( address >= 0xFF10 && address <= 0xFF3F ) {
		map[address] = data;
    } else {
		writeDirectly(address, data);
    }
}

void Memory::DMA(uint8_t data) {
    for ( int32_t i = 0; i < 0xA0; i++ ) {
        writeDirectly(0xFE00+i, read((cpu->getAF().hi << 8) + i));
    }
}

void Memory::loadCartridge() const {
    uint8_t *rom = cartridge->getRom();
    for ( int32_t i = 0; i < 0x8000; i++ ) {
        map[i] = rom[i];
    }
}

void Memory::reset() {
	map[0xFF05] = 0x00;
	map[0xFF06] = 0x00;
	map[0xFF07] = 0x00;
	map[0xFF0F] = 0xE1;
	map[0xFF10] = 0x80;
	map[0xFF11] = 0xBF;
	map[0xFF12] = 0xF3;
	map[0xFF14] = 0xBF;
	map[0xFF16] = 0x3F;
	map[0xFF17] = 0x00;
	map[0xFF19] = 0xBF;
	map[0xFF1A] = 0x7F;
	map[0xFF1B] = 0xFF;
	map[0xFF1C] = 0x9F;
	map[0xFF1E] = 0xBF;
	map[0xFF20] = 0xFF;
	map[0xFF21] = 0x00;
	map[0xFF22] = 0x00;
	map[0xFF23] = 0xBF;
	map[0xFF24] = 0x77;
	map[0xFF25] = 0xF3;
	map[0xFF26] = 0xF1;
	map[0xFF40] = 0x91;
	map[0xFF42] = 0x00;
	map[0xFF43] = 0x00;
	map[0xFF45] = 0x00;
	map[0xFF47] = 0xFC;
	map[0xFF48] = 0xFF;
	map[0xFF49] = 0xFF;
	map[0xFF4A] = 0x00;
	map[0xFF4B] = 0x00;
	map[0xFFFF] = 0x00;
}

void Memory::hardReset() {
    loadCartridge();
	MemoryChip* oldChip = this->chip;
	this->chip = Memory::createMemoryChipForCartridge(this, cartridge);
	if (cartridge->getRamSize() != Cartridge::RamSize::NONE_RAM) {
		chip->load(cartridge->getTitle());
	}
	delete oldChip;
}

Memory::~Memory() {
	if (cartridge->getRamSize() != Cartridge::RamSize::NONE_RAM) {
		chip->save(cartridge->getTitle());
	}

    delete []map;
}
