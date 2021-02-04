#pragma once

#include <algorithm>
#include <cstdint>
#include <string_view>

#include "peripherals/gpio.hpp"
#include "devices/io/parallel_bus.hpp"
#include "module.hpp"
#include "utility/enum.hpp"
#include "utility/log.hpp"
#include "utility/time/time.hpp"

namespace sjsu
{
/// Driver for the St7066u driver of a LCD character display
class St7066u : public Module<>
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
    size_t line_number = 0;
    /// Character column position
    size_t position = 0;
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
  static constexpr CursorPosition_t kDefaultCursorPosition{ 0, 0 };

  /// Constructor for the display driver with desired configurations.
  ///
  /// @param bus_mode 4-bit or 8-bit data transfer bus mode.
  /// @param display_mode Number of lines used for displaying characters.
  /// @param font_style Character font style
  /// @param register_select_pin - Pin that outputs whether the instruction or
  ///        data register is selected.
  /// @param read_write_pin - Pin that outputs whether the operation is
  ///        read/write.
  /// @param enable_pin - Enable pin used for triggering read/write operations.
  /// @param data_bus - 4-bit or 8-bit GPIO parallel bus depending on the
  ///        specified bus_mode.
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
        register_select_pin_(register_select_pin),
        read_write_pin_(read_write_pin),
        enable_pin_(enable_pin),
        data_bus_(data_bus)
  {
  }

  /// Initialize the pins needed to communicate with the LCD screen
  void ModuleInitialize() override
  {
    register_select_pin_.Initialize();
    read_write_pin_.Initialize();
    enable_pin_.Initialize();
    data_bus_.Initialize();

    register_select_pin_.SetAsOutput();
    read_write_pin_.SetAsOutput();
    enable_pin_.SetAsOutput();
    data_bus_.SetAsOutput();

    enable_pin_.SetHigh();

    WriteCommand(Value(Command::kDefaultDisplayConfiguration) |
                 Value(kBusMode) | Value(kDisplayMode) | Value(kFontStyle));
    SetDisplayOn();
    ClearDisplay();
  }

  /// Clears all characters on the display by sending the clear display command
  /// to the device.
  void ClearDisplay()
  {
    WriteCommand(Command::kClearDisplay);
    sjsu::Delay(1600us);  // Clear display operation requires 1.52ms
  }

  /// @param on Toggles the display on if true.
  void SetDisplayOn(bool on = true)
  {
    WriteCommand(on ? Command::kTurnDisplayOn : Command::kTurnDisplayOff);
  }

  /// Hide the cursor
  ///
  /// @param hidden - set to false to make the cursor appear.
  void SetCursorHidden(bool hidden = true)
  {
    WriteCommand(hidden ? Command::kTurnDisplayOn : Command::kTurnCursorOn);
  }

  /// Set the drawing direction of the cursor and characters
  void SetCursorDirection(CursorDirection direction)
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
  /// @param cursor - Line number (row) and character position to move cursor to
  ///
  /// @throw sjsu::Exception with error code std::errc::argument_out_of_domain.
  void SetCursorPosition(CursorPosition_t cursor)
  {
    if (cursor.line_number > 4 || cursor.position > 20)
    {
      throw sjsu::Exception(std::errc::argument_out_of_domain,
                            "Line number must be between 0 to 4 and cursor "
                            "position from 0 to 19.");
    }

    constexpr std::array<Command, 4> kLineAddress = {
      Command::kDisplayLineAddress0,
      Command::kDisplayLineAddress1,
      Command::kDisplayLineAddress2,
      Command::kDisplayLineAddress3,
    };

    uint8_t line_address = Value(kLineAddress[cursor.line_number]);

    WriteCommand(static_cast<uint8_t>(line_address + cursor.position));
  }

  /// Move cursor back to the starting position.
  void ResetCursorPosition()
  {
    WriteCommand(Command::kResetCursor);
    sjsu::Delay(1600us);  // requires at least 1.52ms
  }

  /// Displays a desired text string on the display.
  ///
  /// @param text   - String to write on the display.
  /// @param cursor - Position to start writing the characters
  void DrawText(std::string_view text, CursorPosition_t cursor)
  {
    constexpr size_t kMaxDisplayWidth = 20;

    if (cursor.position >= kMaxDisplayWidth)
    {
      return;
    }

    // Distance from position to the end of the screen
    const size_t kWidth = kMaxDisplayWidth - cursor.position;

    // set cursor start position for writing
    SetCursorPosition(cursor);

    std::string_view sub_string = text.substr(0, std::min(text.size(), kWidth));
    for (const auto & character : sub_string)
    {
      WriteData(character);
    }
  }

 private:
  /// @param operation Operation transfer type.
  /// @param data      The 4-bit or 8-bit data to write to the device.
  void Write(WriteOperation operation, uint8_t data)
  {
    enable_pin_.SetHigh();
    register_select_pin_.Set(sjsu::Gpio::State(operation));
    read_write_pin_.SetLow();

    data_bus_.Write(data);
    sjsu::Delay(80ns);  // Data setup time minimum of 80ns

    // Toggle chip enable to trigger write on falling edge
    enable_pin_.SetLow();

    sjsu::Delay(10ns);  // Data hold time minimum of 10ns
    enable_pin_.SetHigh();
  }

  /// @param command 8-bit command to send.
  void WriteCommand(Command command)
  {
    WriteCommand(Value(command));
  }

  /// Perform a write command
  ///
  /// @param command - the command to send to the LCD screen.
  void WriteCommand(uint8_t command)
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
  void WriteData(uint8_t data)
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

  const BusMode kBusMode;
  const DisplayMode kDisplayMode;
  const FontStyle kFontStyle;

  /// Pin that outputs whether the instruction or data register is selected.
  Gpio & register_select_pin_;

  /// Pin that outputs whether the operation is read/write.
  Gpio & read_write_pin_;

  /// Enable pin used for triggering read/write operations.
  Gpio & enable_pin_;

  /// 4-bit or 8-bit data bus.
  ParallelBus & data_bus_;
};
}  // namespace sjsu
