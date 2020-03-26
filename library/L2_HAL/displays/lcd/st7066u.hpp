#pragma once

#include <cstdint>
#include <cstring>

#include "L1_Peripheral/gpio.hpp"
#include "L2_HAL/io/parallel_bus.hpp"
#include "utility/enum.hpp"
#include "utility/log.hpp"
#include "utility/time.hpp"

namespace sjsu
{
/// Driver for the St7066u driver of a LCD character display
class St7066u
{
 public:
  /// Data transfer operation types.
  enum class WriteOperation : uint8_t
  {
    kCommand = 0,
    kData    = 1
  };

  /// LCD screen commands
  enum class Command : uint8_t
  {
    /// Clears all characters on the display.
    kClearDisplay = 0x01,
    /// Resets the cursor to line 0, position 0.
    kResetCursor = 0x02,
    /// Sets the cursor direction to write backwards direction.
    kCursorDirectionBackward = 0x04,
    /// Sets the cursor direction to write in a forward direction.
    kCursorDirectionForward = 0x06,
    /// Toggles the display OFF.
    kTurnDisplayOff = 0x08,
    /// Toggles the display ON, with cursor blink OFF.
    kTurnDisplayOn = 0x0C,
    /// Toggles the diplay ON, with cursor blink ON.
    kTurnCursorOn = 0x0F,
    /// Default mask for display configuration.
    kDefaultDisplayConfiguration = 0x20,
    /// Base cursor address position for line 0.
    kDisplayLineAddress0 = 0x80,
    /// Base cursor address position for line 1.
    kDisplayLineAddress1 = 0xC0,
    /// Base cursor address position for line 2, only available if the
    /// display supports more than 2 display lines.
    kDisplayLineAddress2 = 0x94,
    /// Base cursor address position for line 3, only available if the
    /// display supports more than 2 display lines.
    kDisplayLineAddress3 = 0xD4,
  };

  /// Cursor direction when writing new characters to the display.
  enum class CursorDirection : uint8_t
  {
    kForward  = 0,
    kBackward = 1
  };

  /// Structure the holds where the position of the character cursor is.
  struct CursorPosition_t
  {
    /// Display line number starting at 0 for the first line.
    uint8_t line_number;
    /// Character column position
    uint8_t position;
  };

  /// The two forms of bus modes for the LCD screen
  enum class BusMode : uint8_t
  {
    kFourBit  = 0 << 4,
    kEightBit = 1 << 4
  };

  /// Available display modes
  enum class DisplayMode : uint8_t
  {
    kSingleLine = 0 << 3,
    kMultiLine  = 1 << 3
  };

  /// Available font styles
  enum class FontStyle : uint8_t
  {
    kFont5x8  = 0 << 2,
    kFont5x11 = 1 << 2
  };

  /// Default cursor position at line 0, position 0.
  static constexpr CursorPosition_t kDefaultCursorPosition =
      CursorPosition_t{ 0, 0 };

  /// Constructor for the display driver with desired configurations.
  ///
  /// @param bus_mode 4-bit or 8-bit data transfer bus mode.
  /// @param display_mode Number of lines used for displaying characters.
  /// @param font_style Character font style
  /// @param register_select_pin Pin that outputs whether the instruction or
  ///                            data register is selected.
  /// @param read_write_pin Pin that outputs whether the operation is
  ///                       read/write.
  /// @param enable_pin Enable pin used for triggering read/write operations.
  /// @param data_bus 4-bit or 8-bit GPIO parallel bus depending on the
  ///                 specified bus_mode.
  explicit constexpr St7066u(BusMode bus_mode,
                             DisplayMode display_mode,
                             FontStyle font_style,
                             sjsu::Gpio & register_select_pin,
                             sjsu::Gpio & read_write_pin,
                             sjsu::Gpio & enable_pin,
                             ParallelBus & data_bus)
      : kBusMode(bus_mode),
        kDisplayMode(display_mode),
        kFontStyle(font_style),
        kRegisterSelectPin(register_select_pin),
        kReadWritePin(read_write_pin),
        kEnablePin(enable_pin),
        kDataBus(data_bus)
  {
  }

  /// Initialize the pins needed to communicate with the LCD screen
  void Initialize() const
  {
    kRegisterSelectPin.SetAsOutput();
    kReadWritePin.SetAsOutput();
    kEnablePin.SetAsOutput();
    kEnablePin.SetHigh();
    kDataBus.Initialize();
    kDataBus.SetAsOutput();

    WriteCommand(Value(Command::kDefaultDisplayConfiguration) |
                 Value(kBusMode) | Value(kDisplayMode) | Value(kFontStyle));
    SetDisplayOn();
    ClearDisplay();
  }

  /// @param operation Operation transfer type.
  /// @param data      The 4-bit or 8-bit data to write to the device.
  void Write(WriteOperation operation, uint8_t data) const
  {
    kEnablePin.SetHigh();
    kRegisterSelectPin.Set(sjsu::Gpio::State(operation));
    kReadWritePin.SetLow();

    kDataBus.Write(data);
    sjsu::Delay(80ns);  // Data setup time minimum of 80ns
    // Toggle chip enable to trigger write on falling edge
    kEnablePin.SetLow();
    sjsu::Delay(10ns);  // Data hold time minimum of 10ns
    kEnablePin.SetHigh();
  }

  /// @param command 8-bit command to send.
  void WriteCommand(Command command) const
  {
    WriteCommand(Value(command));
  }

  /// Perform a write command
  ///
  /// @param command - the command to send to the LCD screen.
  void WriteCommand(uint8_t command) const
  {
    switch (kBusMode)
    {
      case BusMode::kFourBit:
        Write(WriteOperation::kCommand, (command >> 4) & 0xF);
        Write(WriteOperation::kCommand, (command >> 0) & 0xF);
        break;
      case BusMode::kEightBit: Write(WriteOperation::kCommand, command); break;
    }
  }

  /// Writes a byte to the current cursor address position.
  ///
  /// @param data Byte to send to device.
  void WriteData(uint8_t data) const
  {
    switch (kBusMode)
    {
      case BusMode::kFourBit:
        Write(WriteOperation::kData, (data >> 4) & 0xF);
        Write(WriteOperation::kData, (data >> 0) & 0xF);
        break;
      case BusMode::kEightBit: Write(WriteOperation::kData, data); break;
    }
  }

  /// Clears all characters on the display by sending the clear display command
  /// to the device.
  void ClearDisplay() const
  {
    WriteCommand(Command::kClearDisplay);
    sjsu::Delay(1600us);  // Clear display operation requires 1.52ms
  }

  /// @param on Toggles the display on if TRUe.
  void SetDisplayOn(bool on = true) const
  {
    WriteCommand(on ? Command::kTurnDisplayOn : Command::kTurnDisplayOff);
  }

  /// Make the cursor hidden
  ///
  /// @param hidden - set to false to make the cursor appear.
  void SetCursorHidden(bool hidden = true) const
  {
    WriteCommand(hidden ? Command::kTurnDisplayOn : Command::kTurnCursorOn);
  }

  /// Set the drawing direction of the cursor and characters
  void SetCursorDirection(CursorDirection direction) const
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

  /// Sets the cursor at a specified position.
  ///
  /// @param position Line number (row) and character position to move cursor
  ///                 to.
  void SetCursorPosition(CursorPosition_t position) const
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
    WriteCommand(static_cast<uint8_t>(line_address + position.position));
  }

  /// Move cursor back to the starting position.
  void ResetCursorPosition() const
  {
    WriteCommand(Command::kResetCursor);
    sjsu::Delay(1600us);  // requires 1.52ms
  }

  /// Displays a desired text string on the display.
  ///
  /// @param text     String to write on the display.
  /// @param position Position to start writing the characters
  void DisplayText(const char * text,
                   CursorPosition_t position = kDefaultCursorPosition) const
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

 private:
  const BusMode kBusMode;
  const DisplayMode kDisplayMode;
  const FontStyle kFontStyle;
  /// Pin that outputs whether the instruction or data register is selected.
  const Gpio & kRegisterSelectPin;  // NOLINT
  /// Pin that outputs whether the operation is read/write.
  const Gpio & kReadWritePin;  // NOLINT
  /// Enable pin used for triggering read/write operations.
  const Gpio & kEnablePin;  // NOLINT
  /// 4-bit or 8-bit data bus.
  ParallelBus & kDataBus;  // NOLINT
};
}  // namespace sjsu
