#pragma once

#include <cstddef>

#include <algorithm>
#include <cstdint>

#include "L1_Drivers/gpio.hpp"
#include "L1_Drivers/pin.hpp"
#include "L1_Drivers/ssp.hpp"
#include "L2_Utilities/log.hpp"

class Ssd1306
{
 public:
  static constexpr size_t kWidth        = 128;  // pixels
  static constexpr size_t kColumns      = kWidth;
  static constexpr size_t kColumnHeight = 8;                        // bits
  static constexpr size_t kHeight       = 64;                       // pixels
  static constexpr size_t kPages        = kHeight / kColumnHeight;  // rows
  static constexpr size_t kRows         = kPages;

  constexpr Ssd1306()
      : ssp_(&ssp1_),
        cs_(&cs_gpio_),
        dc_(&dc_gpio_),
        ssp1_(Ssp::Peripheral::kSsp1),
        cs_gpio_(1, 22),
        dc_gpio_(1, 25),
        bitmap_{}
  {
  }

  constexpr Ssd1306(Ssp * ssp, Gpio * cs, Gpio * dc)
      : ssp_(ssp),
        cs_(cs),
        dc_(dc),
        ssp1_(),
        cs_gpio_(1, 22),
        dc_gpio_(1, 25),
        bitmap_{}
  {
  }

  enum class Transaction
  {
    kCommand = 0,
    kData    = 1
  };

  void Write(uint8_t data, Transaction transaction)
  {
    dc_->Set(static_cast<Gpio::State>(transaction));
    cs_->SetLow();
    ssp_->Transfer(data);
    cs_->SetHigh();
  }

  void InitializationPanel()
  {
    // This sequence of commands was found in:
    //   datasheets/OLED-display/ER-OLED0.96-1_Series_Datasheet.pdf, page 15

    // turn off oled panel
    Write(0xae, Transaction::kCommand);

    // set display clock divide ratio/oscillator frequency
    Write(0xd5, Transaction::kCommand);
    // set divide ratio
    Write(0x80, Transaction::kCommand);

    // set multiplex ratio(1 to 64)
    Write(0xa8, Transaction::kCommand);
    // 1/64 duty
    Write(0x3f, Transaction::kCommand);

    // set display offset
    Write(0xd3, Transaction::kCommand);
    // not offset
    Write(0x00, Transaction::kCommand);

    // set Charge Pump enable/disable
    Write(0x8d, Transaction::kCommand);
    // set(0x10) disable
    Write(0x14, Transaction::kCommand);

    // set start line address
    Write(0x40, Transaction::kCommand);

    // set normal display
    Write(0xa6, Transaction::kCommand);

    // Disable Entire Display On
    Write(0xa4, Transaction::kCommand);

    // set segment re-map 128 to 0
    Write(0xa1, Transaction::kCommand);

    // Set COM Output Scan Direction 64 to 0
    Write(0xC8, Transaction::kCommand);

    // set com pins hardware configuration
    Write(0xda, Transaction::kCommand);
    Write(0x12, Transaction::kCommand);

    // set contrast control register
    Write(0x81, Transaction::kCommand);
    Write(0xCF, Transaction::kCommand);

    // Set pre-charge period
    Write(0xd9, Transaction::kCommand);
    Write(0xf1, Transaction::kCommand);

    // Set Vcomh
    Write(0xdb, Transaction::kCommand);
    Write(0x40, Transaction::kCommand);

    // Set Addressing mode
    Write(0x20, Transaction::kCommand);
    // Addressing mode = Horizontal Mode
    Write(0x00, Transaction::kCommand);
    // Set Column Addresses
    Write(0x21, Transaction::kCommand);
    // Set Column Address start = Column 0
    Write(0x00, Transaction::kCommand);
    // Set Column Address start = Column 127
    Write(0xff, Transaction::kCommand);
    // Set Page Addresses
    Write(0x22, Transaction::kCommand);
    // Set Page Address start = Page 0
    Write(0x00, Transaction::kCommand);
    // Set Page Address start = Page 7
    Write(0xff, Transaction::kCommand);

    // turn on oled panel
    Write(0xaf, Transaction::kCommand);
  }
  void Initialize()
  {
    cs_->SetAsOutput();
    dc_->SetAsOutput();
    cs_->SetHigh();
    dc_->SetHigh();

    ssp_->SetPeripheralMode(Ssp::MasterSlaveMode::kMaster, Ssp::FrameMode::kSpi,
                            Ssp::DataSize::kEight);
    ssp_->SetClock(false, false, 1, 48);
    ssp_->SetPeripheralPower();
    ssp_->Initialize();

    InitializationPanel();
  }
  /// Clears the internal bitmap_ to zero (or a user defined clear_value)
  ///
  /// @param clear_value the value you would like to use to fill internal
  ///        datastructure with. Defaults to writing 0x00 (zeros)
  void Clear(uint8_t clear_value = 0x00)
  {
    for (size_t i = 0; i < kPages * kWidth; i++)
    {
      Write(clear_value, Transaction::kData);
    }
  }
  /// Writes internal bitmap_ to the screen
  void Update()
  {
    for (size_t row = 0; row < kRows; row++)
    {
      for (size_t column = 0; column < kColumns; column++)
      {
        Write(bitmap_[row][column], Transaction::kData);
      }
    }
  }
  void InvertScreenColor()
  {
    Write(0xA7, Transaction::kCommand);
  }
  void NormalScreenColor()
  {
    Write(0xA6, Transaction::kCommand);
  }
  void SetPixel(size_t x, size_t y, bool pixel_is_on = true)
  {
    // The 3 least significant bits hold the bit position within the byte
    uint32_t bit_position = y & 0b111;
    // Each byte makes up a vertical column.
    // Shifting by 3, which also divides by 8 (the 8-bits of a column), will
    // be the row that we need to edit.
    uint32_t row = y >> 3;
    // Mask to clear the bit
    uint32_t clear_mask = ~(1 << bit_position);
    // Mask to set the bit, if pixel_is_on = true
    uint32_t set_mask = pixel_is_on << bit_position;
    // Address of the pixel column to edit
    uint8_t * pixel_column = &(bitmap_[row][x]);
    // Read pixel column and update the pixel
    uint32_t result = (*pixel_column & clear_mask) | set_mask;
    // Write back resulting operation
    *pixel_column = static_cast<uint8_t>(result);
  }
  void DrawHorizontalLine(size_t x, size_t y, size_t line_width)
  {
    LOG_DEBUG("x = %zu :: y = %zu :: line_width = %zu", x, y, line_width);
    line_width = std::clamp(x + line_width, size_t(0), kColumns - x);
    for (size_t column = x; column < line_width; column++)
    {
      SetPixel(column, y);
    }
  }
  void DrawVerticalLine(size_t x, size_t y, size_t line_height)
  {
    line_height = std::clamp(y + line_height, size_t(0), kHeight - y);
    LOG_DEBUG("x = %zu :: y = %zu :: line_height = %zu", x, y, line_height);
    for (size_t row = y; row < line_height; row++)
    {
      SetPixel(x, row);
    }
  }
  void DrawRectangle(size_t x, size_t y, size_t width, size_t height)
  {
    height = std::clamp(height, size_t(0), kHeight - y);
    width  = std::clamp(width, size_t(0), kWidth - x);
    LOG_DEBUG("x = %zu :: y = %zu :: height = %zu :: width = %zu", x, y, width,
              height);
    // +---------------------
    //
    //
    //
    DrawHorizontalLine(x, y, width);
    // ----------------------
    //
    //
    // +---------------------
    DrawHorizontalLine(x, y + height, width);
    // +---------------------
    // |
    // |
    // ----------------------
    DrawVerticalLine(x, y, height);
    // ---------------------+
    // |                    |
    // |                    |
    // ----------------------
    DrawVerticalLine(x + width, y, height);
  }

 private:
  Ssp * ssp_;
  Gpio * cs_;
  Gpio * dc_;

  Ssp ssp1_;
  Gpio cs_gpio_;
  Gpio dc_gpio_;
  uint8_t bitmap_[kRows][kColumns];
};
