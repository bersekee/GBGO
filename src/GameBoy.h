#pragma once

#include <string>

class Cartridge;
class Memory;
class CPU;
class Video;
class Joypad;

class GameBoy {

public:
	GameBoy(const std::string&);
	void startEmulation();
	~GameBoy();

private:
	const int32_t MAX_CYCLES = 70224;
	const float FPS = 59.73f;
	const float DELAY_TIME = 1000.0f / FPS;

    Cartridge *cartridge;
    Memory *memory;
    CPU *cpu;
    Video *video;
    Joypad *joypad;
};