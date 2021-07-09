#include "GameBoy.h"

#include "Cartridge.h"
#include "Memory.h"
#include "Video.h"
#include "CPU.h"
#include "Types.h"
#include "Joypad.h"
#include <iostream>
#include <thread>

GameBoy::GameBoy(const std::string &path) {
    this->cartridge = new Cartridge();
	this->cartridge->loadEmptyRom();
    this->joypad = new Joypad();
	this->memory = new Memory(cartridge, joypad);
    this->cpu = new CPU(memory);
    this->video = new Video(memory, cpu);
    this->memory->init(cpu, video);
    joypad->init(cpu, memory);
}

void GameBoy::startEmulation() {
  
	int32_t cycles;
	std::chrono::time_point<std::chrono::high_resolution_clock> current, previous;
	previous = std::chrono::high_resolution_clock::now();

    bool quit = false;
    while (!quit) {
		current = std::chrono::high_resolution_clock::now();
		auto elapsed = std::chrono::duration_cast<std::chrono::duration<float, std::milli>> (current - previous);
		previous = current;

		cycles = 0;
		while (cycles < MAX_CYCLES) {
			short cyclesPerThisOpcode = cpu->update();
			cpu->updateTimers(cyclesPerThisOpcode);
			video->updateGraphics(cyclesPerThisOpcode);
			cpu->updateInterrupts(cyclesPerThisOpcode);
			cycles += cyclesPerThisOpcode;
		}

		if (elapsed.count() < DELAY_TIME) {
			std::this_thread::sleep_for(std::chrono::duration<float, std::milli>(DELAY_TIME - elapsed.count()));
		} else {
			printf("overpassed by %f\n", elapsed.count() - DELAY_TIME);
		}
    }
}

GameBoy::~GameBoy() {
    delete joypad;
    delete memory;
    delete cartridge;
    delete video;
    delete cpu;
}
