#include "Video.h"

#include "Memory.h"
#include "CPU.h"
#include <map>

RGB Video::CLASSIC_PALLETE[4] = { { 155,188,15 }, { 139,172,15 }, { 48,98,48 }, { 15,56,15 } };
RGB Video::GREY_PALLETE[4] = { { 255,255,255 },{ 0xCC,0xCC,0xCC },{ 0x77,0x77,0x77 }, { 0x0,0x0,0x0 } };
const Video::WindowResolution Video::FULLSCREEN_WINDOW = { 1920, 1080 };
const Video::WindowResolution Video::WINDOW_RESOLUTIONS[MAX_AMOUNT_WINDOW_RESOLUTIONS] = 
{	
	{GAMEBOY_WIDTH, GAMEBOY_HEIGHT}, 
	{GAMEBOY_WIDTH * 5, GAMEBOY_HEIGHT * 5}, 
	FULLSCREEN_WINDOW 
};

Video::Video(Memory *memory, CPU *cpu) : lcdEnabled(true) {
    this->memory = memory;
    this->cpu = cpu;
    this->resetFrameBuffer();
	imageData = new uint8_t[GAMEBOY_WIDTH * GAMEBOY_HEIGHT * 4];

	this->videoCycles = 0;
	this->vblankCycles = 0;
	this->mode = Video::Mode::OAM_RAM;
	this->currentPallete = GREY_PALLETE;
	this->currentResolutionIndex = 0;
}

void Video::reset()
{
	this->resetFrameBuffer();
	this->videoCycles = 0;
	this->vblankCycles = 0;
	this->mode = Video::Mode::OAM_RAM;
}


void Video::updateGraphics(short cycles) {
    if (isLCDEnabled()) {
		videoCycles += cycles;
		switch (mode) {
			case Video::Mode::H_BLANK:
				handleHBlankMode();
				break;
			case Video::Mode::V_BLANK:
				handleVBlankMode(cycles);
				break;
			case Video::Mode::OAM_RAM:
				handleOAMMode();
				break;
			case Video::Mode::LCD_TRANSFER:
				handleLCDTransferMode();
				break;
		}
	} else {
		DisableLCD();
	}
}

void Video::DisableLCD()
{
	lcdEnabled = false;
	videoCycles = 0;
	vblankCycles = 0;
	memory->writeDirectly(LY_REGISTER, 0);

	uint8_t lcdc = memory->readDirectly(LCDC_STATUS);
	memory->writeDirectly(LCDC_STATUS, lcdc & 0xFC);
	mode = Video::Mode::H_BLANK;
}

void Video::handleHBlankMode() {
    if (videoCycles >= MIN_HBLANK_MODE_CYCLES) {
		videoCycles -= MIN_HBLANK_MODE_CYCLES;

		uint8_t scanline = memory->readDirectly(LY_REGISTER);
		memory->writeDirectly(LY_REGISTER, ++scanline);
		compareLYWithLYC();
		if (scanline == VERTICAL_BLANK_SCAN_LINE) {
			mode = Mode::V_BLANK;

			/* Move to mode 1: VBLANK*/
			uint8_t lcdStatus = memory->readDirectly(LCDC_STATUS);
			setBit(&lcdStatus, 0);
			memory->writeDirectly(LCDC_STATUS, lcdStatus);

			cpu->requestInterrupt(Interrupts::VBLANK);
			if (isBitSet(lcdStatus, 4)) {
				cpu->requestInterrupt(Interrupts::LCD);
			}
		} else {
			mode = Mode::OAM_RAM;
			uint8_t lcdStatus = memory->readDirectly(LCDC_STATUS);
			setBit(&lcdStatus, 1);
			clearBit(&lcdStatus, 0);
			memory->writeDirectly(LCDC_STATUS, lcdStatus);

			if (isBitSet(lcdStatus, 5)) {
				cpu->requestInterrupt(Interrupts::LCD);
			}
		}
    }
}

void Video::handleVBlankMode(short cycles) {
	vblankCycles += cycles;

	uint8_t scanline = memory->readDirectly(LY_REGISTER);
    if (vblankCycles >= MAX_VIDEO_CYCLES) {
		vblankCycles = 0;

		if (scanline < VERTICAL_BLANK_SCAN_LINE_MAX) {
			memory->writeDirectly(LY_REGISTER, ++scanline);
		}
		compareLYWithLYC();
	}

	if (videoCycles >= VBLANK_CYCLES) {
		vblankCycles = 0;
		videoCycles = 0;

		/* Reset scanlines to zero */
		memory->writeDirectly(LY_REGISTER, 0);
		compareLYWithLYC();

		/* Move to mode 2: OAM Mode*/
		mode = Mode::OAM_RAM;
		uint8_t lcdStatus = memory->readDirectly(LCDC_STATUS);
        setBit(&lcdStatus, 1);
        clearBit(&lcdStatus, 0);
        memory->writeDirectly(LCDC_STATUS, lcdStatus);

        if (isBitSet(lcdStatus, 5)) {
            cpu->requestInterrupt(Interrupts::LCD);
        }
    }
}

void Video::handleOAMMode() {
	if (videoCycles >= MIN_OAM_MODE_CYCLES) {
		videoCycles -= MIN_OAM_MODE_CYCLES;

		/* Move to mode 3 */
		mode = Mode::LCD_TRANSFER;
		uint8_t lcdStatus = memory->readDirectly(LCDC_STATUS);
		setBit(&lcdStatus, 1);
		setBit(&lcdStatus, 0);
		memory->writeDirectly(LCDC_STATUS, lcdStatus);
	}
}

void Video::handleLCDTransferMode() {
	if (videoCycles >= MIN_LCD_TRANSFER_CYCLES) {
		videoCycles -= MIN_LCD_TRANSFER_CYCLES;

		/* This can be optimize so it is done before we change the mode */
		drawScanline();

		/* Move to mode 0 */
		mode = Mode::H_BLANK;
		uint8_t lcdStatus = memory->readDirectly(LCDC_STATUS);
		clearBit(&lcdStatus,1);
		clearBit(&lcdStatus,0);
		memory->writeDirectly(LCDC_STATUS, lcdStatus);

		if (isBitSet(lcdStatus, 3)) {
			cpu->requestInterrupt(Interrupts::LCD);
		}
	}
}

void Video::compareLYWithLYC() {
	uint8_t currentScanline = memory->readDirectly(LY_REGISTER);
	uint8_t lyCompare = memory->readDirectly(LY_COMPARE);
	uint8_t lcdStatus = memory->readDirectly(LCDC_STATUS);

	if (lyCompare == currentScanline) {
		setBit(&lcdStatus, 2);
		if (isBitSet(lcdStatus, 6)) {
			cpu->requestInterrupt(Interrupts::LCD);
		}
	} else {
		clearBit(&lcdStatus, 2);
	}
	memory->writeDirectly(LCDC_STATUS, lcdStatus);
}

void Video::drawScanline() {
    const uint8_t lcdControl = memory->readDirectly(LCD_CONTROL);
	if (isBitSet(lcdControl, 0)) {
		renderBackground(lcdControl);
		if (isBitSet(lcdControl, 5)) {
			const int32_t scanline = memory->readDirectly(LY_REGISTER);
			const uint8_t windowY = memory->readDirectly(WINDOWS_Y);
			if (scanline >= windowY)
			{
				renderWindow(lcdControl, scanline, windowY);
			}
		}
	}
	if ( isBitSet(lcdControl, 1) ) {
        renderSprites(lcdControl);
    }
}

void Video::renderBackground(uint8_t lcdControl) {
    uint8_t scrollY = memory->readDirectly(SCROLL_Y);
    uint8_t scrollX = memory->readDirectly(SCROLL_X);

	const int32_t scanline = memory->readDirectly(LY_REGISTER);
	const bool unsig = isBitSet(lcdControl, 4);
	const uint8_t backgroundPallete = memory->readDirectly(0xFF47);
	const uint16_t tileMap = isBitSet(lcdControl, 3) ? 0x9C00 : 0x9800;
	const uint16_t tileData = unsig ? 0x8000 : 0x8800;

	uint8_t xPos = 0;
	uint8_t yPos = scrollY + scanline;
	uint16_t tileRow, tileLocation;
    for ( int32_t pixel = 0 ; pixel < 160; pixel++ ) {
		xPos = pixel + scrollX;
		yPos = scrollY + scanline;
		renderTile(unsig, tileMap, tileData, xPos, yPos, pixel, backgroundPallete, scanline);
    }
}

void Video::renderWindow(uint8_t lcdControl, uint8_t scanline, uint8_t windowY) {
	uint8_t windowX = memory->readDirectly(WINDOWS_X);
	if (windowX <= 0x07) {
		windowX -= windowX;
	} else {
		windowX -= 7;
	}

	const bool unsig = isBitSet(lcdControl, 4);
	const uint16_t windowTileMap = isBitSet(lcdControl, 6) ? 0x9C00 : 0x9800;
	const uint16_t tileData = unsig ? 0x8000 : 0x8800;
	const uint8_t backgroundPallete = memory->readDirectly(0xFF47);

	uint16_t tileMap;
	uint8_t xPos = 0, yPos = 0;
	uint16_t tileRow, tileLocation;
	for (int32_t pixel = windowX; pixel < 160; pixel++) {
		xPos = pixel - windowX;
		yPos = scanline - windowY;
		renderTile(unsig, windowTileMap, tileData, xPos, yPos, pixel, backgroundPallete, scanline);
	}
}

void Video::renderTile(bool unsig, uint16_t tileMap, uint16_t tileData, uint8_t xPos, uint8_t yPos, uint8_t pixel, uint8_t backgroundPallete, uint8_t scanline) {

	const uint16_t tileRow = ((yPos / 8) * 32);
	if (unsig) {
		uint8_t tileNum = memory->read(tileMap + tileRow + (xPos / 8));
		tileData += (tileNum * 16);
		//assert(tileData < 0x8FFF);
	}
	else {
		signed char tileNum = memory->read(tileMap + tileRow + (xPos / 8));
		tileData += ((tileNum + 0x80) * 16);
		//assert(tileData < 0x97FF);
	}

	uint8_t address = (yPos % 8) * 2;
	uint8_t upperuint8_t = memory->read(tileData + address);
	uint8_t loweruint8_t = memory->read(tileData + address + 1);

	int32_t colourBit = xPos % 8;
	colourBit -= 7;
	colourBit = -colourBit;

	int32_t colourNum = getBitValue(loweruint8_t, colourBit);
	colourNum <<= 1;
	colourNum |= getBitValue(upperuint8_t, colourBit);

	RGB colour = currentPallete[getColourFromPallete(backgroundPallete, Colour(colourNum))];
	int32_t index = pixel + (scanline * GAMEBOY_WIDTH);
	frameBuffer[index].red = colour.red;
	frameBuffer[index].green = colour.green;
	frameBuffer[index].blue = colour.blue;
}

void Video::renderSprites(uint8_t lcdControl) {
	bool use8x16 = isBitSet(lcdControl, 2);

    for (int32_t sprite = 0 ; sprite < 40; sprite++) {
        uint8_t index = sprite*4 ;

        // Read the sprite position and its tile index
        uint8_t yPos = memory->read(0xFE00 + index) - 16;
        uint8_t xPos = memory->read(0xFE00 + index + 1) - 8;
        uint8_t tileIndex = memory->read(0xFE00+index+2) ;

        // Read the sprite attributes
        const uint8_t attributes = memory->read(0xFE00+index+3) ;
        const uint8_t pallete = memory->read(isBitSet(attributes, 4) ? 0xFF49 : 0xFF48);
        const uint8_t backgroundPallete = memory->readDirectly(0xFF47);
        const RGB backgroundZeroColour = currentPallete[getColourFromPallete(backgroundPallete, Colour::WHITE)];
        const bool yFlip =  isBitSet(attributes, 6);
        const bool xFlip = isBitSet(attributes, 5);
        const bool spriteOnTop = !isBitSet(attributes, 7);

        const int32_t scanline = memory->read(LY_REGISTER);

        int32_t ysize = 8;
        if ( use8x16 ) {
            ysize = 16;
        }

        if ( (scanline >= yPos) && (scanline < (yPos+ysize)) ) {
            int32_t line = scanline - yPos;

            if ( yFlip ) {
                line -= ysize;
                line *= -1;
            }

            line *= 2;
            uint8_t upperuint8_t = memory->read((0x8000 + (tileIndex * 0x10)) + line);
            uint8_t loweruint8_t = memory->read((0x8000 + (tileIndex * 0x10)) + line+1);

            for ( int32_t tilePixel = 7; tilePixel >= 0; tilePixel-- ) {
                
				int32_t xPix = 0 - tilePixel;
				xPix += 7;

				int32_t pixel = xPos + xPix;
				if (pixel < 160 && scanline < 144) {

					int32_t position = tilePixel;
					if (xFlip) {
						position -= 7;
						position *= -1;
					}

					uint8_t colourNum = 0x00;
					if (getBitValue(loweruint8_t, position)) {
						setBit(&colourNum, 1);
					}
					if (getBitValue(upperuint8_t, position)) {
						setBit(&colourNum, 0);
					}

					if (colourNum != Colour::WHITE) {
						const int32_t index = pixel + (scanline * GAMEBOY_WIDTH);
						if (spriteOnTop || frameBuffer[index].isEqual(backgroundZeroColour)) {
					        const RGB colour = currentPallete[getColourFromPallete(pallete, Colour(colourNum))];
                            frameBuffer[index].red = colour.red;
							frameBuffer[index].green = colour.green;
							frameBuffer[index].blue = colour.blue;
						}
					}
				}
 			}
		}
	}
}

Video::Mode Video::getLCDMode() const {
    uint8_t lcdStatus = memory->readDirectly(LCDC_STATUS);
	return Video::Mode(lcdStatus & 0x3);
}

Video::Colour Video::getColourFromPallete(uint8_t pallete, Colour originalColour) {
	uint8_t colourNumber = 0;
	switch (originalColour) {
		case Colour::WHITE:
		if (getBitValue(pallete, 1)) {
			setBit(&colourNumber, 1);
		}
		if (getBitValue(pallete, 0)) {
			setBit(&colourNumber, 0);
		}
		break;
		case Colour::LIGHT_GREY:
		if (getBitValue(pallete, 3)) {
			setBit(&colourNumber, 1);
		}
		if (getBitValue(pallete, 2)) {
			setBit(&colourNumber, 0);
		}
		break;
		case Colour::DARK_GREY:
		if (getBitValue(pallete, 5)) {
			setBit(&colourNumber, 1);
		}
		if (getBitValue(pallete, 4)) {
			setBit(&colourNumber, 0);
		}
		break;
		case Colour::BLACK:
		if (getBitValue(pallete, 7)) {
			setBit(&colourNumber, 1);
		}
		if (getBitValue(pallete, 6)) {
			setBit(&colourNumber, 0);
		}
		break;
	}

	return Colour(colourNumber);
}

void Video::switchPalette() {
	if (currentPallete == CLASSIC_PALLETE) {
		currentPallete = GREY_PALLETE;
	} else if (currentPallete == GREY_PALLETE) {
		currentPallete = CLASSIC_PALLETE;
	}
}

uint8_t* Video::GetCanvasImageData()
{
	uint8_t* im = new uint8_t[GAMEBOY_WIDTH * GAMEBOY_HEIGHT * 4];
	for (int32_t i = 0; i < GAMEBOY_WIDTH * GAMEBOY_HEIGHT; i++)
	{
		int32_t imageDataIdex = i * 4;
		im[imageDataIdex] = this->frameBuffer[i].red;
		im[imageDataIdex + 1] = this->frameBuffer[i].green;
		im[imageDataIdex + 2] = this->frameBuffer[i].blue;
		im[imageDataIdex + 3] = 255;
	}
	return im;
}

void Video::resetFrameBuffer() {
	memset(frameBuffer, 0xFF, sizeof(frameBuffer));
}

Video::~Video() {
	delete[] imageData;
}
