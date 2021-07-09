#include "GameBoy.h"

int32_t main(int32_t argc, char **argv)
{
    GameBoy* gameboy = new GameBoy("C:\\Users\\Jeremy\\Documents\\GBGo\\test_roms\\tetris.gb");
	gameboy->startEmulation();
    delete gameboy;
	return 0;
}