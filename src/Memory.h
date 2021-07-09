#pragma once

#include "Types.h"

class CPU;
class Joypad;
class Video;
class MemoryChip;
class Cartridge;

class Memory {
public:
    Memory(Cartridge *, Joypad *);
	~Memory();

	void init(CPU *, Video*);
	void reset();
    void hardReset();
    
    uint8_t read(const uint16_t) const;
	inline uint8_t readDirectly(const uint16_t address) const {
		return map[address];
	}

	inline uint16_t readWordDirectly(const uint16_t address) const {
		Register aux;
		aux.low = map[address];
		aux.hi = map[address + 1];
		return aux.value;
	}

    void write(const uint16_t, const uint8_t);

	inline void writeDirectly(const uint16_t address, const uint8_t data) {
		this->map[address] = data;
	}

	inline uint8_t * getMap() const {
		return map;
	}

private:

	static MemoryChip* createMemoryChipForCartridge(Memory* memory, Cartridge* cartridge);

    Video *video;
    Joypad *joypad;
    CPU *cpu;
    uint8_t *map;
    Cartridge *cartridge;
	MemoryChip* chip;


    void loadCartridge() const;
    void DMA(uint8_t);
};