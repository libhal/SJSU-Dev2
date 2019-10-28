#pragma once

#include <cstddef>
#include <cstdint>
#include <cstring>

#include "config.hpp"
#include "L1_Peripheral/gpio.hpp"
#include "L1_Peripheral/spi.hpp"
#include "L2_HAL/displays/pixel_display.hpp"
#include "utility/log.hpp"

namespace sjsu
{
class Ssd1306 final : public PixelDisplay
{
 public:
  static constexpr size_t kColumns      = 128;
  static constexpr size_t kWidth        = kColumns;                 // pixels
  static constexpr size_t kColumnHeight = 8;                        // bits
  static constexpr size_t kHeight       = 64;                       // pixels
  static constexpr size_t kPages        = kHeight / kColumnHeight;  // rows
  static constexpr size_t kRows         = kPages;

  enum class Transaction
  {
    kCommand = 0,
    kData    = 1
  };

  constexpr Ssd1306(sjsu::Spi & spi, sjsu::Gpio & cs, sjsu::Gpio & dc)
      : spi_(spi), cs_(cs), dc_(dc), bitmap_{}
  {
  }

  size_t GetWidth() override
  {
    return kWidth;
  }
  size_t GetHeight() override
  {
    return kHeight;
  }
  Color_t AvailableColors() override
  {
    return Color_t{
      .red   = 0,
      .green = 0,
      .blue  = 0,
      .alpha = 1,
    };
  }

  void Write(uint32_t data, Transaction transaction, size_t size = 1)
  {
    dc_.Set(static_cast<sjsu::Gpio::State>(transaction));
    cs_.Set(sjsu::Gpio::State::kLow);
    for (size_t i = 0; i < size; i++)
    {
      uint8_t send = static_cast<uint8_t>(data >> (((size - 1) - i) * 8));
      if (transaction == Transaction::kCommand)
      {
        LOG_DEBUG("send = 0x%X", send);
      }
      spi_.Transfer(send);
    }
    cs_.Set(sjsu::Gpio::State::kHigh);
  }

  void Initialize() override
  {
    cs_.SetDirection(sjsu::Gpio::Direction::kOutput);
    dc_.SetDirection(sjsu::Gpio::Direction::kOutput);
    cs_.Set(sjsu::Gpio::State::kHigh);
    dc_.Set(sjsu::Gpio::State::kHigh);

    spi_.Initialize();
    spi_.SetDataSize(sjsu::Spi::DataSize::kEight);
    spi_.SetClock(2_MHz);

    Clear();
    InitializationPanel();
  }

  void InitializationPanel()
  {
    // This sequence of commands was found in:
    //   datasheets/OLED-display/ER-OLED0.96-1_Series_Datasheet.pdf, page 15

    // turn off oled panel
    Write(0xAE, Transaction::kCommand);

    // set display clock divide ratio/oscillator frequency
    // set divide ratio
    Write(0xD5'80, Transaction::kCommand, 2);

    // set multiplex ratio(1 to 64)
    // 1/64 duty
    Write(0xA8'3F, Transaction::kCommand, 2);

    // set display offset = not offset
    Write(0xD3'00, Transaction::kCommand, 2);

    // Set display start line
    Write(0x40, Transaction::kCommand);

    // Disable Charge Pump
    Write(0x8D'14, Transaction::kCommand, 2);

    // set segment re-map 128 to 0
    Write(0xA1, Transaction::kCommand);

    // Set COM Output Scan Direction 64 to 0
    Write(0xC8, Transaction::kCommand);

    // set com pins hardware configuration
    Write(0xDA'12, Transaction::kCommand, 2);

    // set contrast control register
    Write(0x81'CF, Transaction::kCommand, 2);

    // Set pre-charge period
    Write(0xD9'F1, Transaction::kCommand, 2);

    // Set Vcomh
    Write(0xDB'40, Transaction::kCommand, 2);

    SetHorizontalAddressMode();

    // Enable entire display
    Write(0xA4, Transaction::kCommand);

    // Set display to normal colors
    Write(0xA6, Transaction::kCommand);

    // Set Display On
    Write(0xAF, Transaction::kCommand);
  }

  void SetHorizontalAddressMode()
  {
    // Set Addressing mode
    // Addressing mode = Horizontal Mode (0b00)
    Write(0x20'00, Transaction::kCommand, 2);
    // Set Column Addresses
    // Set Column Address start = Column 0
    // Set Column Address start = Column 127
    Write(0x21'00'7F, Transaction::kCommand, 3);
    // Set Page Addresses
    // Set Page Address start = Page 0
    // Set Page Address start = Page 7
    Write(0x22'00'07, Transaction::kCommand, 3);
  }
  /// Clears the internal bitmap_ to zero (or a user defined clear_value)
  void Clear() override
  {
    memset(bitmap_, 0x00, sizeof(bitmap_));
  }
  void Fill()
  {
    memset(bitmap_, 0xFF, sizeof(bitmap_));
  }
  void DrawPixel(int32_t x, int32_t y, Color_t color) override
  {
    // The 3 least significant bits hold the bit position within the byte
    uint32_t bit_position = y & 0b111;
    // Each byte makes up a vertical column.
    // Shifting by 3, which also divides by 8 (the 8-bits of a column), will
    // be the row that we need to edit.
    uint32_t row = y >> 3;
    // Mask to clear the bit
    uint32_t clear_mask = ~(1 << bit_position);
    // Mask to set the bit, if color.alpha != 0
    bool pixel_is_on  = !color.IsBlank();
    uint32_t set_mask = pixel_is_on << bit_position;
    // Address of the pixel column to edit
    uint8_t * pixel_column = &(bitmap_[row][x]);
    // Read pixel column and update the pixel
    uint32_t result = (*pixel_column & clear_mask) | set_mask;
    // Update pixel with the result of this operation
    *pixel_column = static_cast<uint8_t>(result);
  }
  /// Writes internal bitmap_ to the screen
  void Update() override
  {
    SetHorizontalAddressMode();
    for (size_t row = 0; row < kRows; row++)
    {
      for (size_t column = 0; column < kColumns; column++)
      {
        Write(bitmap_[row][column], Transaction::kData);
      }
    }
  }
  void InvertScreenColor() __attribute__((used))
  {
    Write(0xA7, Transaction::kCommand);
  }
  void NormalScreenColor() __attribute__((used))
  {
    Write(0xA6, Transaction::kCommand);
  }

 private:
  sjsu::Spi & spi_;
  sjsu::Gpio & cs_;
  sjsu::Gpio & dc_;

  uint8_t bitmap_[kRows + 5][kColumns + 5];
};
}  // namespace sjsu
