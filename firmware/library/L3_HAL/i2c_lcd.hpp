#pragma once

#include <cstdint>

class I2cLcdInterface
{
public:
    enum class FontSize : uint8_t
    {
        kSmall = 0b0000'0000,
        kLarge = 0b0000'0100,
    };
    enum class DisplayLines : uint8_t
    {
        kOne = 0b0000'0000,
        kTwo = 0b0000'1000,
    };
    virtual void Init() = 0;
    virtual void Set4BitMode() = 0;
    virtual void ClearScreen() = 0;
    virtual void ReturnHome() = 0;
    virtual void PrintChar() = 0;
    virtual void CursorControl(bool show_cursor, bool blink_cursor) = 0;
    virtual void SetLineDisplay(DisplayLines lines) = 0;
    virtual bool CheckBusyFlag() = 0;
    virtual void DisplayControl() = 0;
    virtual void SetFont(FontSize size) = 0;
}
