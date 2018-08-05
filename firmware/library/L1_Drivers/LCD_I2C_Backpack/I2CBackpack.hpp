#pragma once

#include <cstdint>



class LcdBackpackInterface
{
 public:
    enum FontSize
    {
        small;
        large;
    };
    enum DisplayLines
    {
        one;
        two;
        four;
    };
    virtual void Init() = 0;
    virtual void Set4BitMode() = 0;
    virtual void ClearScreen() = 0;
    virtual void SetPosition(uint8_t row, uint8_t col) = 0;
    virtual void ReturnHome() = 0;
    virtual void PrintChar() = 0;
    virtual void CursorControl(bool show_cursor, bool blink_cursor) = 0;
    virtual void SetLineDisplay(DisplayLines lines) = 0;
    virtual bool CheckBusyFlag() = 0;
    virtual void DisplayControl() = 0;
    virtual void ShiftCursor() = 0;
    virtual void SetFont(FontSize size) = 0;
    virtual void FunctionSet(FontSize size, DisplayLines lines);
}
