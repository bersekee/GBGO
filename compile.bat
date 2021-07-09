if "%1"=="-d" (
	em++ src/GBGO.cpp src/Cartridge.cpp src/CPU.cpp src/Joypad.cpp src/main.cpp src/Memory.cpp src/Video.cpp src/cpu/CPU_ALU.cpp src/cpu/CPU_Control.cpp src/cpu/CPU_Jumps.cpp src/cpu/CPU_Loads.cpp src/cpu/CPU_Rotate.cpp src/cpu/CPU_SingleBit.cpp src/memory/MBC1.cpp src/memory/MBC2.cpp src/memory/MBC3.cpp src/memory/RomOnly.cpp -o public/js/GBGO.js -O0 -s --profiling --bind EXPORTED_FUNCTIONS="[_init,_updateState,_loadRom,_getCanvasImageData,_handleInput,_setCPUSpeed,_switchPalette]"
) else (
	em++ src/GBGO.cpp src/Cartridge.cpp src/CPU.cpp src/Joypad.cpp src/main.cpp src/Memory.cpp src/Video.cpp src/cpu/CPU_ALU.cpp src/cpu/CPU_Control.cpp src/cpu/CPU_Jumps.cpp src/cpu/CPU_Loads.cpp src/cpu/CPU_Rotate.cpp src/cpu/CPU_SingleBit.cpp src/memory/MBC1.cpp src/memory/MBC2.cpp src/memory/MBC3.cpp src/memory/RomOnly.cpp -o public/js/GBGO.js -O2 -s --bind EXPORTED_FUNCTIONS="[_init,_updateState,_loadRom,_getCanvasImageData,_handleInput,_setCPUSpeed,_switchPalette]"
)
