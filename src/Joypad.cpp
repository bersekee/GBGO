#include "Joypad.h"

#include "Memory.h"
#include "CPU.h"

#include <iostream>

Joypad::Joypad(){
    joypadState = 0xFF;
}

void Joypad::init(CPU *cpu, Memory *memory){
    this->cpu = cpu;
    this->memory = memory;
}

void Joypad::keyPressed(int32_t key) {
    bool previouslyUnset = false ;
    
	if ( !isBitSet(joypadState, key) ) {
		previouslyUnset = true ;
    }
    
    clearBit(&joypadState, key);
    
	bool button;
    
	if ( key > 3 ) {
		button = true ;
    } else {
		button = false ;
    }
    
	uint8_t keyReq = memory->readDirectly(0xFF00);
	bool requestInterupt = false;
    
	if ( button && !isBitSet(keyReq,5) ) {
		requestInterupt = true ;
	} else if ( !button && !isBitSet(keyReq,4) ) {
		requestInterupt = true ;
	}
    
	if ( requestInterupt && !previouslyUnset ) {
        cpu->requestInterrupt(Interrupts::JOYPAD);
	}
}

void Joypad::keyReleased(int32_t key) {
    setBit(&joypadState, key);
}

uint8_t Joypad::getState() {
	uint8_t res = memory->readDirectly(0xFF00);
	res ^= 0xFF ;
    
	if ( !isBitSet(res, 4) ) {
		uint8_t topJoypad = joypadState >> 4 ;
		topJoypad |= 0xF0 ;
		res &= topJoypad ;
	} else if ( !isBitSet(res,5) ) {
		uint8_t bottomJoypad = joypadState & 0xF ;
		bottomJoypad |= 0xF0 ;
		res &= bottomJoypad ;
	}
	return res ;
}

void Joypad::handleInput(int32_t keyCode, bool keydown)
{
	int32_t key;
	switch (keyCode)
	{
		case 90:        key = 4;    break;
		case 88:        key = 5;    break;
		case 8:			key = 7;    break;
		case 32:		key = 6;    break;
		case 39:		key = 0;    break;
		case 37:		key = 1;    break;
		case 38:		key = 2;    break;
		case 40:		key = 3;    break;
		default: 		return;
	}
	if (keydown)
	{
		keyPressed(key);
	}
	else
	{
		keyReleased(key);
	}
}