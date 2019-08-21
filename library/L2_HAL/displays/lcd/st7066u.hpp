// This file contains the driver class for Parallel LCD Screens that utilize the
// ST7066U controller.
// Usage:
//      St7066u lcd(St7066u::BusMode::kEightBit,
//                  St7066u::DisplayMode::kMultiLine,
//                  St7066u::FontStyle::kFont5x8, pins);
//      lcd.Initialize();
//      lcd.DisplayText("Example Text.");
#pragma once

#include <cstdint>
#include <cstring>

#include "L1_Peripheral/gpio.hpp"
#include "utility/log.hpp"
#include "utility/time.hpp"

namespace sjsu
{
class St7066u final
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
    // Resets the cursor to line 0, position 0.
    kResetCursor = 0x02,
    // Sets the cursor direction to write backwards direction.
    kCursorDirectionBackward = 0x04,
    // Sets the cursor direction to write in a forward direction.
    kCursorDirectionForward = 0x06,
    // Toggles the display OFF.
    kTurnDisplayOff = 0x08,
    // Toggles the display ON, with cursor blink OFF.
    kTurnDisplayOn = 0x0C,
    // Toggles the diplay ON, with cursor blink ON.
    kTurnCursorOn = 0x0F,
    // Default mask for display configuration.
    kDefaultDisplayConfiguration = 0x20,
    // Base cursor address position for line 0.
    kDisplayLineAddress0 = 0x80,
    // Base cursor address position for line 1.
    kDisplayLineAddress1 = 0xC0,
    // Base cursor address position for line 2, only available if the
    // display supports more than 2 display lines.
    kDisplayLineAddress2 = 0x94,
    // Base cursor address position for line 3, only available if the
    // display supports more than 2 display lines.
    kDisplayLineAddress3 = 0xD4,
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

  enum class BusMode : uint8_t
  {
    kFourBit  = 0 << 4,
    kEightBit = 1 << 4
  };

  enum class DisplayMode : uint8_t
  {
    kSingleLine = 0 << 3,
    kMultiLine  = 1 << 3
  };

  enum class FontStyle : uint8_t
  {
    kFont5x8  = 0 << 2,
    kFont5x11 = 1 << 2
  };

  struct ControlPins_t
  {
    sjsu::Gpio & rs;  // Register Selection
    sjsu::Gpio & rw;  // Read/Write
    sjsu::Gpio & e;   // Chip Enable
    sjsu::Gpio & d7;
    sjsu::Gpio & d6;
    sjsu::Gpio & d5;
    sjsu::Gpio & d4;
    sjsu::Gpio & d3;
    sjsu::Gpio & d2;
    sjsu::Gpio & d1;
    sjsu::Gpio & d0;
  };

  // Default cursor position at line 0, position 0.
  static constexpr CursorPosition_t kDefaultCursorPosition =
      CursorPosition_t{ 0, 0 };

  // Constructor for the display driver with desired configurations.
  //
  // @param bus_mode     4-bit or 8-bit data transfer bus mode.
  // @param display_mode Number of lines used for displaying characters.
  // @param font_style   Character font style
  // @param pins         GPIO control pins for controlling the display.
  constexpr St7066u(BusMode bus_mode, DisplayMode display_mode,
                    FontStyle font_style, const ControlPins_t & pins)
      : kBusMode(bus_mode),
        kDisplayMode(display_mode),
        kFontStyle(font_style),
        kControlPins(pins)
  {
  }

  void Initialize()
  {
    kControlPins.rs.SetDirection(sjsu::Gpio::Direction::kOutput);
    kControlPins.rw.SetDirection(sjsu::Gpio::Direction::kOutput);
    kControlPins.e.SetDirection(sjsu::Gpio::Direction::kOutput);
    kControlPins.e.Set(sjsu::Gpio::kHigh);
    kControlPins.d7.SetDirection(sjsu::Gpio::Direction::kOutput);
    kControlPins.d6.SetDirection(sjsu::Gpio::Direction::kOutput);
    kControlPins.d5.SetDirection(sjsu::Gpio::Direction::kOutput);
    kControlPins.d4.SetDirection(sjsu::Gpio::Direction::kOutput);

    if (kBusMode == BusMode::kEightBit)
    {
      kControlPins.d3.SetDirection(sjsu::Gpio::Direction::kOutput);
      kControlPins.d2.SetDirection(sjsu::Gpio::Direction::kOutput);
      kControlPins.d1.SetDirection(sjsu::Gpio::Direction::kOutput);
      kControlPins.d0.SetDirection(sjsu::Gpio::Direction::kOutput);
    }

    WriteCommand(static_cast<uint8_t>(Command::kDefaultDisplayConfiguration) |
                 static_cast<uint8_t>(kBusMode) |
                 static_cast<uint8_t>(kDisplayMode) |
                 static_cast<uint8_t>(kFontStyle));
    SetDisplayOn();
    ClearDisplay();
  }

  // Transfers 4-bits of a command or data to the device.
  //
  // @param operation Operation transfer type.
  // @param nibble    4-bit data to transfer.
  void WriteNibble(WriteOperation operation, uint8_t nibble)
  {
    kControlPins.e.Set(sjsu::Gpio::State::kHigh);
    kControlPins.rs.Set(sjsu::Gpio::State(operation));
    kControlPins.rw.Set(sjsu::Gpio::State::kLow);
    // set nibble on 4-bit data bus
    kControlPins.d7.Set(sjsu::Gpio::State((nibble >> 3) & 0x01));
    kControlPins.d6.Set(sjsu::Gpio::State((nibble >> 2) & 0x01));
    kControlPins.d5.Set(sjsu::Gpio::State((nibble >> 1) & 0x01));
    kControlPins.d4.Set(sjsu::Gpio::State((nibble >> 0) & 0x01));
    sjsu::Delay(1ms);
    // Toggle chip enable to trigger write on falling edge
    kControlPins.e.Set(sjsu::Gpio::State::kLow);
    kControlPins.e.Set(sjsu::Gpio::State::kHigh);
  }

  // Transfers a command or data byte to the device.
  //
  // @param operation Operation transfer type.
  // @param byte      Byte to transfer.
  void WriteByte(WriteOperation operation, uint8_t byte)
  {
    kControlPins.e.Set(sjsu::Gpio::State::kHigh);
    kControlPins.rs.Set(sjsu::Gpio::State(operation));
    kControlPins.rw.Set(sjsu::Gpio::State::kLow);
    // set byte on 8-bit data bus
    kControlPins.d7.Set(sjsu::Gpio::State((byte >> 7) & 0x01));
    kControlPins.d6.Set(sjsu::Gpio::State((byte >> 6) & 0x01));
    kControlPins.d5.Set(sjsu::Gpio::State((byte >> 5) & 0x01));
    kControlPins.d4.Set(sjsu::Gpio::State((byte >> 4) & 0x01));
    kControlPins.d3.Set(sjsu::Gpio::State((byte >> 3) & 0x01));
    kControlPins.d2.Set(sjsu::Gpio::State((byte >> 2) & 0x01));
    kControlPins.d1.Set(sjsu::Gpio::State((byte >> 1) & 0x01));
    kControlPins.d0.Set(sjsu::Gpio::State((byte >> 0) & 0x01));
    sjsu::Delay(1ms);
    // Toggle chip enable to trigger write on falling edge
    kControlPins.e.Set(sjsu::Gpio::State::kLow);
    kControlPins.e.Set(sjsu::Gpio::State::kHigh);
  }

  // @param command 8-bit command to send.
  [[gnu::always_inline]] void WriteCommand(Command command)
  {
    WriteCommand(static_cast<uint8_t>(command));
  }
  virtual void WriteCommand(uint8_t command)
  {
    switch (kBusMode)
    {
      case BusMode::kFourBit:
        WriteNibble(WriteOperation::kCommand, (command >> 4) & 0xF);
        WriteNibble(WriteOperation::kCommand, (command >> 0) & 0xF);
        break;
      case BusMode::kEightBit:
        WriteByte(WriteOperation::kCommand, command);
        break;
    }
  }

  // Writes a byte to the current cursor address position.
  //
  // @param data Byte to send to device.
  virtual void WriteData(uint8_t data)
  {
    switch (kBusMode)
    {
      case BusMode::kFourBit:
        WriteNibble(WriteOperation::kData, (data >> 4) & 0xF);
        WriteNibble(WriteOperation::kData, (data >> 0) & 0xF);
        break;
      case BusMode::kEightBit: WriteByte(WriteOperation::kData, data); break;
    }
  }

  // Clears all characters on the display by sending the clear display command
  // to the device.
  virtual void ClearDisplay()
  {
    WriteCommand(Command::kClearDisplay);
    sjsu::Delay(2ms);  // Clear display operation requires 1.52ms
  }

  // @param on Toggles the display on if TRUe.
  virtual void SetDisplayOn(bool on = true)
  {
    WriteCommand(on ? Command::kTurnDisplayOn : Command::kTurnDisplayOff);
  }

  virtual void SetCursorHidden(bool hidden = true)
  {
    WriteCommand(hidden ? Command::kTurnDisplayOn : Command::kTurnCursorOn);
  }

  virtual void SetCursorDirection(CursorDirection direction)
  {
    switch (direction)
    {
      case CursorDirection::kBackward:
        WriteCommand(Command::kCursorDirectionBackward);
        break;
      case CursorDirection::kForward:
        WriteCommand(Command::kCursorDirectionForward);
        break;
    }
  }

  // Sets the cursor at a specified position.
  //
  // @param line Line number to move cursor to.
  // @param pos  Character position to move cursor to.
  virtual void SetCursorPosition(CursorPosition_t position)
  {
    SJ2_ASSERT_FATAL(position.line_number < 5,
                     "SetCursorPosition() - The driver does not support "
                     "more than 4 display lines");
    SJ2_ASSERT_FATAL(position.position < 20,
                     "SetCursorPosition() - The character position should "
                     "not exceed the max display width");

    constexpr Command kLineAddresses[] = {
      Command::kDisplayLineAddress0,
      Command::kDisplayLineAddress1,
      Command::kDisplayLineAddress2,
      Command::kDisplayLineAddress3,
    };
    uint8_t line_address =
        static_cast<uint8_t>(kLineAddresses[position.line_number]);
    WriteCommand(uint8_t(line_address + position.position));
  }

  [[gnu::always_inline]] virtual void ResetCursorPosition()
  {
    WriteCommand(Command::kResetCursor);
    sjsu::Delay(2ms);  // requires 1.52ms
  }

  // Displays a desired text string on the display.
  //
  // @param text     String to write on the display.
  // @param position Position to start writing the characters
  virtual void DisplayText(const char * text,
                   CursorPosition_t position = kDefaultCursorPosition)
  {
    constexpr uint8_t kMaxDisplayWidth = 20;
    const uint8_t kStartOffset         = position.position;
    uint8_t termination_index          = uint8_t(strlen(text));
    // calculate the termination_index to stop writing to the display
    // if the string length of text exceeds the display's width
    if (termination_index > kMaxDisplayWidth)
    {
      termination_index = kMaxDisplayWidth;
    }
    if (termination_index > (kMaxDisplayWidth - kStartOffset))
    {
      termination_index = uint8_t(kMaxDisplayWidth - kStartOffset);
    }
    // set cursor start position for writing
    SetCursorPosition(position);
    for (uint8_t i = 0; i < termination_index; i++)
    {
      WriteData(text[i]);
    }
  }

 protected:
  const BusMode kBusMode;
  const DisplayMode kDisplayMode;
  const FontStyle kFontStyle;
  const ControlPins_t & kControlPins;
};
}  // namespace sjsu
