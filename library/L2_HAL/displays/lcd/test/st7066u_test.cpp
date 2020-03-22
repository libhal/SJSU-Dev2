// Tests for the St7066u Parallel LCD Driver class.
#include "L2_HAL/displays/lcd/st7066u.hpp"
#include "L2_HAL/io/parallel_bus.hpp"
#include "L4_Testing/testing_frameworks.hpp"

namespace sjsu
{
EMIT_ALL_METHODS(St7066u);

TEST_CASE("Testing St7066u Parallel LCD Driver", "[st70668]")
{
  Mock<Gpio> mock_rs;  // RS: Register Select
  Mock<Gpio> mock_rw;  // RW: Read / Write
  Mock<Gpio> mock_e;   // E   Chip Enable
  Mock<ParallelBus> mock_data_bus;

  Fake(Method(mock_rs, SetDirection), Method(mock_rs, Set));
  Fake(Method(mock_rw, SetDirection), Method(mock_rw, Set));
  Fake(Method(mock_e, SetDirection), Method(mock_e, Set));
  Fake(Method(mock_data_bus, Initialize),
       Method(mock_data_bus, SetDirection),
       Method(mock_data_bus, Write));

  const St7066u kFourBitBusLcd(St7066u::BusMode::kFourBit,
                               St7066u::DisplayMode::kSingleLine,
                               St7066u::FontStyle::kFont5x8,
                               mock_rs.get(),
                               mock_rw.get(),
                               mock_e.get(),
                               mock_data_bus.get());
  const St7066u kEightBitBusLcd(St7066u::BusMode::kEightBit,
                                St7066u::DisplayMode::kSingleLine,
                                St7066u::FontStyle::kFont5x8,
                                mock_rs.get(),
                                mock_rw.get(),
                                mock_e.get(),
                                mock_data_bus.get());

  SECTION("Initialize")
  {
    constexpr St7066u::Command kDefaultConfiguration =
        St7066u::Command::kDefaultDisplayConfiguration;
    constexpr St7066u::DisplayMode kDisplayMode =
        St7066u::DisplayMode::kSingleLine;
    constexpr St7066u::FontStyle kFontStyle = St7066u::FontStyle::kFont5x8;
    St7066u::BusMode bus_mode;
    /// The expected bytes that are sent for initialization.
    std::array<uint8_t, 6> expected_data;
    size_t write_count = 0;

    When(Method(mock_data_bus, Write))
        .AlwaysDo([&expected_data, &write_count](uint32_t data) {
          INFO("write_count: " << write_count);
          INFO("data: " << data);
          CHECK(data == expected_data[write_count]);
          write_count++;
        });

    SECTION("With 4-bit Bus")
    {
      bus_mode                         = St7066u::BusMode::kFourBit;
      const uint8_t kConfigurationData = Value(kDefaultConfiguration) |
                                         Value(bus_mode) | Value(kDisplayMode) |
                                         Value(kFontStyle);
      // For 4-bit bus mode, the most significant nibble is sent first followed
      // by the least significant nibble.
      expected_data = {
        static_cast<uint8_t>(kConfigurationData >> 4),
        static_cast<uint8_t>(kConfigurationData & 0xF),
        0x0,
        Value(St7066u::Command::kTurnDisplayOn),
        0x0,
        Value(St7066u::Command::kClearDisplay),
      };
      kFourBitBusLcd.Initialize();
    }

    SECTION("With 8-bit Bus")
    {
      bus_mode      = St7066u::BusMode::kEightBit;
      expected_data = {
        static_cast<uint8_t>(Value(kDefaultConfiguration) | Value(bus_mode) |
                             Value(kDisplayMode) | Value(kFontStyle)),
        Value(St7066u::Command::kTurnDisplayOn),
        Value(St7066u::Command::kClearDisplay),
      };
      kEightBitBusLcd.Initialize();
    }

    Verify(Method(mock_rs, SetDirection).Using(Gpio::Direction::kOutput),
           Method(mock_rw, SetDirection).Using(Gpio::Direction::kOutput),
           Method(mock_e, SetDirection).Using(Gpio::Direction::kOutput),
           Method(mock_e, Set).Using(Gpio::kHigh),
           Method(mock_data_bus, Initialize),
           Method(mock_data_bus, SetDirection).Using(Gpio::Direction::kOutput));
  }

  SECTION("Write Nibble")
  {
    constexpr uint8_t kExpectedNibble = 0b1010;
    St7066u lcd                       = kFourBitBusLcd;

    lcd.WriteNibble(St7066u::WriteOperation::kCommand, kExpectedNibble);
    Verify(Method(mock_rs, Set)
               .Using(Gpio::State(St7066u::WriteOperation::kCommand)),
           Method(mock_rw, Set).Using(Gpio::State::kLow),
           Method(mock_data_bus, Write).Using(kExpectedNibble),
           Method(mock_e, Set).Using(Gpio::State::kLow),
           Method(mock_e, Set).Using(Gpio::State::kHigh));

    lcd.WriteNibble(St7066u::WriteOperation::kData, kExpectedNibble);
    Verify(
        Method(mock_rs, Set).Using(Gpio::State(St7066u::WriteOperation::kData)),
        Method(mock_rw, Set).Using(Gpio::State::kLow),
        Method(mock_data_bus, Write).Using(kExpectedNibble),
        Method(mock_e, Set).Using(Gpio::State::kLow),
        Method(mock_e, Set).Using(Gpio::State::kHigh));
  }

  SECTION("WriteByte")
  {
    constexpr uint8_t kExpectedByte = 0b1010'1010;
    St7066u lcd                     = kEightBitBusLcd;

    lcd.WriteNibble(St7066u::WriteOperation::kCommand, kExpectedByte);
    Verify(Method(mock_rs, Set)
               .Using(Gpio::State(St7066u::WriteOperation::kCommand)),
           Method(mock_rw, Set).Using(Gpio::State::kLow),
           Method(mock_data_bus, Write).Using(kExpectedByte),
           Method(mock_e, Set).Using(Gpio::State::kLow),
           Method(mock_e, Set).Using(Gpio::State::kHigh));

    lcd.WriteNibble(St7066u::WriteOperation::kData, kExpectedByte);
    Verify(
        Method(mock_rs, Set).Using(Gpio::State(St7066u::WriteOperation::kData)),
        Method(mock_rw, Set).Using(Gpio::State::kLow),
        Method(mock_data_bus, Write).Using(kExpectedByte),
        Method(mock_e, Set).Using(Gpio::State::kLow),
        Method(mock_e, Set).Using(Gpio::State::kHigh));
  }

  SECTION("ClearDisplay")
  {
    constexpr uint8_t kClearDisplay =
        static_cast<uint8_t>(St7066u::Command::kClearDisplay);
    St7066u lcd = kEightBitBusLcd;

    lcd.ClearDisplay();
    Verify(Method(mock_rs, Set)
               .Using(Gpio::State(St7066u::WriteOperation::kCommand)),
           Method(mock_rw, Set).Using(Gpio::State::kLow),
           Method(mock_data_bus, Write).Using(kClearDisplay),
           Method(mock_e, Set).Using(Gpio::State::kLow),
           Method(mock_e, Set).Using(Gpio::State::kHigh));
  }

  SECTION("SetDisplayOn")
  {
    const uint8_t kDisplayOn =
        static_cast<uint8_t>(St7066u::Command::kTurnDisplayOn);
    const uint8_t kDisplayOff =
        static_cast<uint8_t>(St7066u::Command::kTurnDisplayOff);
    St7066u lcd = kEightBitBusLcd;

    lcd.SetDisplayOn(true);
    Verify(Method(mock_rs, Set)
               .Using(Gpio::State(St7066u::WriteOperation::kCommand)),
           Method(mock_rw, Set).Using(Gpio::State::kLow),
           Method(mock_data_bus, Write).Using(kDisplayOn),
           Method(mock_e, Set).Using(Gpio::State::kLow),
           Method(mock_e, Set).Using(Gpio::State::kHigh));

    lcd.SetDisplayOn(false);
    Verify(Method(mock_rs, Set)
               .Using(Gpio::State(St7066u::WriteOperation::kCommand)),
           Method(mock_rw, Set).Using(Gpio::State::kLow),
           Method(mock_data_bus, Write).Using(kDisplayOff),
           Method(mock_e, Set).Using(Gpio::State::kLow),
           Method(mock_e, Set).Using(Gpio::State::kHigh));
  }

  SECTION("SetCursorHidden")
  {
    const uint8_t kShowCursor =
        static_cast<uint8_t>(St7066u::Command::kTurnCursorOn);
    const uint8_t kHideCursor =
        static_cast<uint8_t>(St7066u::Command::kTurnDisplayOn);
    St7066u lcd = kEightBitBusLcd;

    lcd.SetCursorHidden(false);
    Verify(Method(mock_rs, Set)
               .Using(Gpio::State(St7066u::WriteOperation::kCommand)),
           Method(mock_rw, Set).Using(Gpio::State::kLow),
           Method(mock_data_bus, Write).Using(kShowCursor),
           Method(mock_e, Set).Using(Gpio::State::kLow),
           Method(mock_e, Set).Using(Gpio::State::kHigh));

    lcd.SetCursorHidden();
    Verify(Method(mock_rs, Set)
               .Using(Gpio::State(St7066u::WriteOperation::kCommand)),
           Method(mock_rw, Set).Using(Gpio::State::kLow),
           Method(mock_data_bus, Write).Using(kHideCursor),
           Method(mock_e, Set).Using(Gpio::State::kLow),
           Method(mock_e, Set).Using(Gpio::State::kHigh));
  }

  SECTION("Setting Cursor Direction")
  {
    constexpr uint8_t kCursorDirectionBackward =
        static_cast<uint8_t>(St7066u::Command::kCursorDirectionBackward);
    constexpr uint8_t kCursorDirectionForward =
        static_cast<uint8_t>(St7066u::Command::kCursorDirectionForward);
    St7066u lcd = kEightBitBusLcd;

    lcd.SetCursorDirection(St7066u::CursorDirection::kBackward);
    Verify(Method(mock_rs, Set)
               .Using(Gpio::State(St7066u::WriteOperation::kCommand)),
           Method(mock_rw, Set).Using(Gpio::State::kLow),
           Method(mock_data_bus, Write).Using(kCursorDirectionBackward),
           Method(mock_e, Set).Using(Gpio::State::kLow),
           Method(mock_e, Set).Using(Gpio::State::kHigh));

    lcd.SetCursorDirection(St7066u::CursorDirection::kForward);
    Verify(Method(mock_rs, Set)
               .Using(Gpio::State(St7066u::WriteOperation::kCommand)),
           Method(mock_rw, Set).Using(Gpio::State::kLow),
           Method(mock_data_bus, Write).Using(kCursorDirectionForward),
           Method(mock_e, Set).Using(Gpio::State::kLow),
           Method(mock_e, Set).Using(Gpio::State::kHigh));
  }

  SECTION("Setting Cursor Position")
  {
    constexpr uint8_t kLineNumber = 2;
    constexpr uint8_t kPosition   = 0x12;
    // Address should be 0x94 + 0x12 = 0xA6
    constexpr uint8_t kCursorAddress =
        static_cast<uint8_t>(St7066u::Command::kDisplayLineAddress2) +
        kPosition;
    St7066u lcd = kEightBitBusLcd;

    lcd.SetCursorPosition(St7066u::CursorPosition_t{ kLineNumber, kPosition });
    Verify(Method(mock_rs, Set)
               .Using(Gpio::State(St7066u::WriteOperation::kCommand)),
           Method(mock_rw, Set).Using(Gpio::State::kLow),
           Method(mock_data_bus, Write).Using(kCursorAddress),
           Method(mock_e, Set).Using(Gpio::State::kLow),
           Method(mock_e, Set).Using(Gpio::State::kHigh));
  }

  SECTION("Reset Cursor Position")
  {
    constexpr uint8_t kResetCursor =
        static_cast<uint8_t>(St7066u::Command::kResetCursor);
    St7066u lcd = kEightBitBusLcd;

    lcd.ResetCursorPosition();
    Verify(Method(mock_rs, Set)
               .Using(Gpio::State(St7066u::WriteOperation::kCommand)),
           Method(mock_rw, Set).Using(Gpio::State::kLow),
           Method(mock_data_bus, Write).Using(kResetCursor),
           Method(mock_e, Set).Using(Gpio::State::kLow),
           Method(mock_e, Set).Using(Gpio::State::kHigh));
  }

  SECTION("Display Text")
  {
    const char kText[] = "Text String";
    St7066u lcd        = kEightBitBusLcd;

    lcd.DisplayText(kText);
    for (uint8_t i = 0; i < strlen(kText); i++)
    {
      Verify(Method(mock_rs, Set)
                 .Using(Gpio::State(St7066u::WriteOperation::kCommand)),
             Method(mock_rw, Set).Using(Gpio::State::kLow),
             Method(mock_data_bus, Write).Using(kText[i]),
             Method(mock_e, Set).Using(Gpio::State::kLow),
             Method(mock_e, Set).Using(Gpio::State::kHigh));
    }
  }
}
}  // namespace sjsu
