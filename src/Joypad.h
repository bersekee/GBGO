#pragma once

#include <iostream>

class Memory;
class CPU;

class Joypad {
    
private:
    Memory* memory;
    CPU* cpu;
    uint8_t joypadState;
    void keyPressed(int32_t);
    void keyReleased(int32_t);
    
public:
    Joypad();
    void init(CPU*, Memory*);
    void handleInput(int32_t keyCode, bool keydown);
    uint8_t getState();
};