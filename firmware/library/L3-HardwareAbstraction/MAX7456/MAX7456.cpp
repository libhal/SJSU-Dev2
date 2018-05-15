#include <stdint.h>
#include <stdio.h>
#include "MAX7456.hpp"

__attribute__((weak)) void vChipSelect(bool Select)
{
    printf("[REAL] ChipSelect = %d\n", Select);
}

__attribute__((weak)) uint8_t vSPITransfer(uint8_t Data)
{
    printf("[REAL] Transfer = %c\n", Data);
    return Data;
}

// void vChipSelect(bool Select)
// {
//     printf("[BAD] ChipSelect = %d\n", Select);
// }

// uint8_t vSPITransfer(uint8_t Data)
// {
//     printf("[BAD] Transfer = %c\n", Data);
//     return Data;
// }

MAX7456::MAX7456(void (*nChipSelect)(bool Select), uint8_t (*nSPI)(uint8_t data))
{
    printf("MAX7456 initializing!");
    ChipSelect = nChipSelect;
    SPITransfer = nSPI;
}
inline uint8_t MAX7456::READ_OPERATION(uint8_t ADDR) const
{
    return (ADDR |  (1 << 7));
}
inline uint8_t MAX7456::WRITE_OPERATION(uint8_t ADDR) const
{
    return (ADDR & ~(1 << 7));
}
bool MAX7456::Initialize()
{
    //// Shift Vertical Offset defined Pixels
    VideoTransfer(
        HORIZONTAL_OFFSET_REG,
        HORIZONTAL_OFFSET,
        WRITE
    );
    //// Shift Vertical Offset defined Pixels
    VideoTransfer(
        VERTICAL_OFFSET_REG,
        VERTICAL_OFFSET,
        WRITE
    );
    //// Clear Display Memory
    VideoTransfer(
        DISPLAY_MEMORY_MODE,
        DISPLAY_MEMORY_MODE_CLR,
        WRITE
    );
    //// Enable OSD
    VideoTransfer(
        VIDEO_MODE,
        VIDEO_MODE_ENABLE_OSD,
        WRITE
    );
    //// Read OSD Black Level Register
    uint8_t OSDBLReg = VideoTransfer(
        OSD_BLACK_LEVEL,
        0x00,
        READ
    );
    //// Enable Automatic Black Level Control
    ////    By clearing OSDBL[4] = 0
    VideoTransfer(
        OSD_BLACK_LEVEL,
        OSDBLReg & OSD_BL_ENABLE_CONTROL,
        WRITE
    );
    //// Set Display Memory to 8Bit mode
    VideoTransfer(
        DISPLAY_MEMORY_MODE,
        OPERATION_MODE_8BIT,
        WRITE
    );
    return true;
}

void MAX7456::ClearScreen()
{
    VideoTransfer(
        DISPLAY_MEMORY_MODE,
        DISPLAY_MEMORY_MODE_CLR,
        WRITE
    );
}

uint8_t MAX7456::VideoTransfer(uint8_t Addr, uint8_t Data, bool Write)
{
    //// Select Chip
    ChipSelect(true);

    if(Write)
    {
        SPITransfer(WRITE_OPERATION(Addr));
    }
    else
    {
        SPITransfer(READ_OPERATION(Addr));
    }

    uint8_t result = SPITransfer(Data);

    ChipSelect(false);

    return result;
}

void MAX7456::WriteCharacterToScreen(uint16_t Position, uint8_t Character)
{
    //// Loaded 8th bit MSB to Display memory address High byte
    //// DISPLAY_MEMORY_ADDR_H[1] is set to 0 to keep it on
    //// Character memory mode
    VideoTransfer(
        DISPLAY_MEMORY_ADDR_H,
        ((Position >> 8) & 0x01),
        WRITE
    );
    //// Load [7:0] bits of position
    VideoTransfer(
        DISPLAY_MEMORY_ADDR_L,
        (Position & 0xFF),
        WRITE
    );
    //// Load NVM Character address into display memory data
    //// register to be written to display memory matrix
    VideoTransfer(
        DISPLAY_MEM_DATA_REG,
        Character,
        WRITE
    );
}

void MAX7456::WriteString(uint16_t Position, char * Message)
{
    for(int i=0; Message[i] != 0; i++)
    {
        // if(Message[i] >= 'A' && Message[i] <= 'Z')
        // {
        //  WriteCharacterToScreen(Position+i, NVM_A+(Message[i]-'A'));
        // }
        // else if(Message[i] >= 'a' && Message[i] <= 'z')
        // {
        //  WriteCharacterToScreen(Position+i, NVM_a+(Message[i]-'a'));
        // }
        // else if(Message[i] >= '1' && Message[i] <= '9')
        // {
        //  WriteCharacterToScreen(Position+i, NVM_1+(Message[i]-'1'));
        // }
        // else if(Message[i] == '0')
        // {
        //  WriteCharacterToScreen(Position+i, NVM_0);
        // }
        // else if(Message[i] == ' ')
        // {
        //  WriteCharacterToScreen(Position+i, NVM__);
        // }
        // else if(Message[i] == ':')
        // {
        //  WriteCharacterToScreen(Position+i, NVM_COLON);
        // }
        switch(Message[i])
        {
            case 'A' ... 'Z':
                WriteCharacterToScreen(Position+i, NVM_A+(Message[i]-'A'));
                break;
            case 'a' ... 'z':
                WriteCharacterToScreen(Position+i, NVM_a+(Message[i]-'a'));
                break;
            case '1' ... '9':
                WriteCharacterToScreen(Position+i, NVM_1+(Message[i]-'1'));
                break;
            case '0':
                WriteCharacterToScreen(Position+i, NVM_0);
                break;
            case ' ':
                WriteCharacterToScreen(Position+i, NVM__);
                break;
            case ':':
                WriteCharacterToScreen(Position+i, NVM_COLON);
                break;
        }
    }
}

uint16_t MAX7456::CoordsToPosition(uint8_t X, uint8_t Y)
{
    return (Y*DISPLAY_WIDTH)+(X);
}