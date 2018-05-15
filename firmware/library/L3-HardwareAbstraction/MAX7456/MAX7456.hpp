#include <stdint.h>

#ifndef MAX7456_H
#define MAX7456_H

// ==============================
// MAX7456 Driver
// ==============================

void vChipSelect(bool Select);
uint8_t vSPITransfer(uint8_t Data);

// void vChipSelect(bool Select) __attribute__ ((weak));
// uint8_t vSPITransfer(uint8_t Data) __attribute__ ((weak));

class MAX7456
{
private:
	void (*ChipSelect)(bool Select);
	uint8_t (*SPITransfer)(uint8_t Data);

public:
	enum TRANSFER_MODES
	{
		READ = 0,
		WRITE = 1
	};

	static const uint8_t VIDEO_MODE 				= 0x00;
	static const uint8_t VIDEO_MODE_ENABLE_OSD 		= (1 << 3);

	static const uint8_t OSD_BLACK_LEVEL 			= 0x6C;
	static const uint8_t OSD_BL_ENABLE_CONTROL 		= ~(1 << 4);

	static const uint8_t DISPLAY_MEMORY_MODE 		= 0x04;
	static const uint8_t OPERATION_MODE_8BIT 		= (1 << 6);
	static const uint8_t DISPLAY_MEMORY_MODE_CLR 	= (1 << 2);

	static const uint8_t DISPLAY_MEMORY_ADDR_H 		= 0x05;
	static const uint8_t DISPLAY_MEMORY_ADDR_L 		= 0x06;

	static const uint8_t DISPLAY_MEM_DATA_REG       = 0x07;

	static const uint8_t VERTICAL_OFFSET_REG 		= 0x03;
	static const uint8_t VERTICAL_OFFSET 			= (1 << 4) | 8;

	static const uint8_t HORIZONTAL_OFFSET_REG 		= 0x02;
	static const uint8_t HORIZONTAL_OFFSET 			= (1 << 5) | 4;

	static const uint8_t CENTER_OF_SCREEN           = 225;

	static const uint8_t NVM_A 						= 0x0B;
	static const uint8_t NVM_a 						= 0x25;
	static const uint8_t NVM__ 						= 0x00;
	static const uint8_t NVM_0 						= 0x0A;
	static const uint8_t NVM_1 						= 0x01;
	static const uint8_t NVM_COLON 					= 0x44;
	//// Replaced a chinese character with an empty character
	static const uint8_t EMPTY_MASK					= 0xEB;
	static const uint8_t NVM_EMPTY					= 0x00;

	static const uint8_t DISPLAY_LEFT				= 1;
	static const uint8_t DISPLAY_WIDTH              = 30;
	static const uint16_t DISPLAY_HEIGHT			= 450;

	static const uint8_t DISPLAY_TOP	 			= DISPLAY_WIDTH;
	static const uint8_t DISPLAY_MIDDLE 			= ((DISPLAY_HEIGHT/DISPLAY_WIDTH)/2)-1;
	static const uint8_t DISPLAY_CENTER				= (DISPLAY_WIDTH/2);

	MAX7456(void (*nChipSelect)(bool Select), uint8_t (*nSPI)(uint8_t Data));
	bool Initialize();
	uint8_t VideoTransfer(uint8_t Addr, uint8_t Data, bool Write);
	void WriteCharacterToScreen(uint16_t Position, uint8_t Character);
	void WriteString(uint16_t Position, char * Message);
	void ClearScreen();
	uint16_t CoordsToPosition(uint8_t x, uint8_t y);
	inline uint8_t READ_OPERATION(uint8_t ADDR) const;
	inline uint8_t WRITE_OPERATION(uint8_t ADDR) const;
};

#endif