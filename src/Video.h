#pragma once

#include "Types.h"
#include "Utils.h"

class CPU;
class Memory;

class Video {
    
public:
    static const short GAMEBOY_WIDTH = 160;
    static const short GAMEBOY_HEIGHT = 144;
    
    static const uint16_t LCD_CONTROL = 0xFF40;
    static const uint16_t LCDC_STATUS = 0xFF41;
    static const uint16_t LY_REGISTER = 0xFF44;
    static const uint16_t LY_COMPARE = 0xFF45;
    
    enum Colour : int32_t {
        WHITE = 0,
        LIGHT_GREY,
        DARK_GREY,
        BLACK
    };

	enum class Mode : uint8_t {
		H_BLANK = 0,
		V_BLANK = 1,
		OAM_RAM = 2,
		LCD_TRANSFER = 3
	};

    
    Video(Memory*, CPU*);
    ~Video();

	void reset();

    void updateGraphics(short);
	void switchPalette();
	void DisableLCD();
	uint8_t* GetCanvasImageData();

	void* getFrameBuffer() { return frameBuffer; }
	RGB* const getCurrentPallete() const { return currentPallete; }
	Colour getColourFromPallete(uint8_t pallete, Colour originalColour);
	Video::Mode getCurrentMode() const { return mode; }

	bool isLCDEnabled() const { return lcdEnabled; }
	void EnableLCD() { lcdEnabled = true; }
private:
	const uint16_t SCROLL_Y = 0xFF42;
	const uint16_t SCROLL_X = 0xFF43;
	const uint16_t WINDOWS_Y = 0xFF4A;
	const uint16_t WINDOWS_X = 0xFF4B;

	const uint8_t VERTICAL_BLANK_SCAN_LINE = 0x90;
	const uint8_t VERTICAL_BLANK_SCAN_LINE_MAX = 0x99;

	const int32_t MAX_VIDEO_CYCLES = 456;
	const int32_t MIN_HBLANK_MODE_CYCLES = 204;
	const int32_t MIN_OAM_MODE_CYCLES = 80;
	const int32_t MIN_LCD_TRANSFER_CYCLES = 172;
	const int32_t VBLANK_CYCLES = 4560;

	static RGB CLASSIC_PALLETE[4];
	static RGB GREY_PALLETE[4];

	struct WindowResolution
	{
		int32_t width;
		int32_t height;

		bool operator==(const WindowResolution& rhs) const {
			return width == rhs.width && height == rhs.height;
		}
	};
	static const int32_t MAX_AMOUNT_WINDOW_RESOLUTIONS = 3;
	static const WindowResolution FULLSCREEN_WINDOW;
	static const WindowResolution WINDOW_RESOLUTIONS[MAX_AMOUNT_WINDOW_RESOLUTIONS];
	
    RGB frameBuffer[GAMEBOY_HEIGHT * GAMEBOY_WIDTH];

	uint8_t* imageData;

	int32_t currentResolutionIndex;
    CPU* cpu;
    Memory* memory;
	void* icon;

	bool lcdEnabled;
    Mode mode;
	RGB* currentPallete;
    int32_t videoCycles;
	int32_t vblankCycles;

    void handleHBlankMode();
    void handleVBlankMode(short cycles);
    void handleOAMMode();
    void handleLCDTransferMode();

	void compareLYWithLYC();

    void drawScanline();
    Mode getLCDMode() const;
	void renderBackground(uint8_t);
	void renderWindow(uint8_t, uint8_t, uint8_t);
	void renderTile(bool unsig, uint16_t tileMap, uint16_t tileData, uint8_t xPos, uint8_t yPos, uint8_t pixel, uint8_t backgroundPallete, uint8_t scanline);
	void renderSprites(uint8_t);

	inline RGB getColour(const uint8_t colourNumber) const { return currentPallete[colourNumber]; }

    void resetFrameBuffer();
};