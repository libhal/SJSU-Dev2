#pragma once

#include <cstdint>

class St7066uInterface
{
 public:
    // Data transfer operation types.
    enum class WriteOperation : uint8_t
    {
        kCommand = 0,
        kData    = 1
    };

    enum class Command : uint8_t
    {
        // Clears all characters on the display.
        kClearDisplay = 0x01,
        // Resets the cursor  to line 0, position 0.
        kResetCursor = 0x02,
        // Sets the cursor direction to write leftwards.
        kCursorDirectionLeft = 0x04,
        // Sets the cursor direction to write rightwards.
        kCursorDirectionRight = 0x06,
        // Toggles the display OFF.
        kTurnDisplayOff = 0x08,
        // Toggles the display ON, with cursor blink OFF.
        kTurnDisplayOn = 0x0C,
        // Toggles the diplay ON, with cursor blink ON.
        kTurnCursorOn = 0x0F,
        // Default mask for display configuration.
        kDefaultConfig = 0x20,
        // Base cursor address position for line 0.
        kLineAddress0 = 0x80,
        // Base cursor address position for line 1.
        kLineAddress1 = 0xC0
    };

    // Cursor direction when writing new characters to the display.
    enum class CursorDirection : uint8_t
    {
        kForward  = 0,
        kBackward = 1
    };

    struct CursorPosition_t
    {
        // Display line number starting at 0 for the first line.
        uint8_t line_number;
        // Character column position
        uint8_t position;
    };

    virtual void Initialize()                                              = 0;
    virtual void ClearDisplay()                                            = 0;
    virtual void SetDisplayOn(bool on)                                     = 0;
    virtual void SetCursorHidden(bool hidden)                              = 0;
    virtual void WriteNibble(WriteOperation operation, uint8_t nibble)     = 0;
    virtual void WriteByte(WriteOperation operation, uint8_t byte)         = 0;
    virtual void WriteCommand(Command command)                             = 0;
    virtual void WriteCommand(uint8_t command)                             = 0;
    virtual void WriteData(uint8_t data)                                   = 0;
    virtual void SetCursorDirection(CursorDirection direction)             = 0;
    virtual void SetCursorPosition(CursorPosition_t position)              = 0;
    virtual void ResetCursorPosition()                                     = 0;
    virtual void DisplayText(const char * text, CursorPosition_t position) = 0;
};
