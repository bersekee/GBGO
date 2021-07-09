set(project "GBGO")

project(${project})
cmake_minimum_required(VERSION 3.12)

set(src 
  Cartridge.cpp
  CPU.cpp
  GameBoy.cpp
  Joypad.cpp
  main.cpp
  Memory.cpp
  Video.cpp
  cpu/CPU_ALU.cpp
  cpu/CPU_Control.cpp
  cpu/CPU_Jumps.cpp
  cpu/CPU_Loads.cpp
  cpu/CPU_Rotate.cpp
  cpu/CPU_SingleBit.cpp
  memory/MBC1.cpp
  memory/MBC2.cpp
  memory/MBC3.cpp
  memory/RomOnly.cpp
  Cartridge.h
  CPU.h
  GameBoy.h
  Joypad.h
  Memory.h
  Video.h
  Types.h
  Utils.h
  memory/MBC1.h
  memory/MBC2.h
  memory/MBC3.h
  memory/RomOnly.h
  memory/MemoryChip.h
 )

add_executable(goodboy ${src})