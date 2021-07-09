#include <emscripten.h>
#include <emscripten/bind.h>

using namespace emscripten;

#include "CPU.h"
#include "Video.h"
#include "Cartridge.h"
#include "Joypad.h"
#include "Memory.h"

extern "C" {
    void init();
    // void release();
    void setCPUSpeed(float_t speed);
    void updateState();
    void switchPalette();
    // uint8_t* dumpMem();
    void loadRom(uint8_t* uint8_tCode, uint32_t size);
    // void resetCPU();
    // void getNextCPUInstructionString(char* buffer);
    // void writeMemory(uint16_t address, uint8_t value);
    uint8_t* getCanvasImageData();
    void handleInput(uint32_t keycode, bool keydown);
}

bool initialized = false;
int maxCycles = 70224;

Cartridge* cartridge;
Memory* memory;
CPU* cpu;
Video* video;
Joypad* joypad;

// void init()
// {

// }

// void release()
// {
//     delete cpu;
//     delete mmu;
// }

void updateState() {
    int cycles = 0;
    while (cycles < maxCycles) {
        short cyclesPerThisOpcode = cpu->update();
        cpu->updateTimers(cyclesPerThisOpcode);
        video->updateGraphics(cyclesPerThisOpcode);
        cpu->updateInterrupts(cyclesPerThisOpcode);
        cycles += cyclesPerThisOpcode;
    }
}

void setCPUSpeed(float_t speed) {
    maxCycles = static_cast<int>(speed * 70224);
}

// uint8_t* dumpMem()
// {
//     return mmu->dumpMemory();
// }

void init() {
    cartridge = new Cartridge();
    cartridge->loadEmptyRom();
    joypad = new Joypad();
    memory = new Memory(cartridge, joypad);
    cpu = new CPU(memory);
    video = new Video(memory, cpu);
    memory->init(cpu, video);
    joypad->init(cpu, memory);
    initialized = true;
}

void loadRom(uint8_t* code, uint32_t size) {
    if (initialized) {
        cartridge->loadArrayByteCode(code, size);
        memory->hardReset();
        cpu->reset();
        video->reset();
    }
}

void handleInput(uint32_t keycode, bool keydown) {
    if (initialized) {
        joypad->handleInput(keycode, keydown);
    }
}

// void resetCPU()
// {
//     cpu->reset();
// }

// void writeMemory(uint16_t address, uint8_t value)
// {
//     mmu->write(address, value);
// }

// void getNextCPUInstructionString(char* buffer)
// {
//     return cpu->getNextInstructionString(buffer);
// }

// CPUInfo getCPUInfo()
// {
//     return cpu->getInfo();
// }


// EMSCRIPTEN_BINDINGS(cpu_info) {
//   class_<CPUInfo>("CPUInfo")
//     .constructor<>()
//     .property("a", &CPUInfo::a)
//     .property("b", &CPUInfo::b)
//     .property("c", &CPUInfo::c)
//     .property("d", &CPUInfo::d)
//     .property("e", &CPUInfo::e)
//     .property("f", &CPUInfo::f)
//     .property("h", &CPUInfo::h)
//     .property("l", &CPUInfo::l)
//     .property("sp", &CPUInfo::sp)
//     .property("pc", &CPUInfo::pc)
//     ;

//     function("_getCPUInfo", &getCPUInfo);
// }

// int main ()
// {
// }

void switchPalette() {
    video->switchPalette();
}

uint8_t* getCanvasImageData() {
    return video->GetCanvasImageData();
}