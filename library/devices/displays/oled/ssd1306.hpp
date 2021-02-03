#pragma once

#include <cstddef>
#include <cstdint>
#include <cstring>

#include "peripherals/gpio.hpp"
#include "peripherals/spi.hpp"
#include "devices/displays/pixel_display.hpp"
#include "config.hpp"
#include "utility/log.hpp"

namespace sjsu
{
/// Display driver for the SSD1306 OLED display driver chip, usually found on
/// 0.98" oled displays.
/// User manual: https://cdn-shop.adafruit.com/datasheets/SSD1306.pdf
class Ssd1306 final : public PixelDisplay
{
 public:
  /// Defines the number of columns the device has
  static constexpr size_t kColumns = 128;

  /// Defines the width in pixels, which is simply the columns
  static constexpr size_t kWidth = kColumns;

  /// Defines the height in pixels
  static constexpr size_t kHeight = 64;

  /// Defines the column height which is a single byte or 8 bits
  static constexpr size_t kColumnHeight = 8;

  /// Calculates the number of rows that exist for the device.
  static constexpr size_t kRows = kHeight / kColumnHeight;

  /// Calculates the number of rows that exist for the device.
  static constexpr auto kDefaultClockRate = 2_MHz;

  /// Defines the types of communication that can occur with the display driver.
  enum class Transaction
  {
    kCommand = 0,
    kData    = 1
  };

  /// Construct SSD1306 driver
  ///
  /// @param spi   - spi bus that is connected to the device
  /// @param cs    - gpio that is connected to the chip select pin of the
  ///                device.
  /// @param dc    - gpio that is connected to the data/command
  ///                pin of the device.
  /// @param reset - gpio connected to the reset line. If an RC reset circuit is
  ///                used, this can be replaced with
  ///                `sjsu::GetInactive<sjsu::Gpio>()`
  /// @param clock_rate - clock rate to communication with the OLED display
  ///                     with.
  constexpr Ssd1306(sjsu::Spi & spi,
                    sjsu::Gpio & cs,
                    sjsu::Gpio & dc,
                    sjsu::Gpio & reset,
                    units::frequency::hertz_t clock_rate = kDefaultClockRate)
      : spi_(spi),
        cs_(cs),
        dc_(dc),
        reset_(reset),
        clock_rate_(clock_rate),
        bitmap_{}
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

  void ModuleInitialize() override
  {
    SpiSettings_t default_settings;  // Default construct
    default_settings.clock_rate = clock_rate_;
    spi_.settings               = default_settings;

    spi_.Initialize();
    cs_.Initialize();
    dc_.Initialize();
    reset_.Initialize();

    cs_.SetAsOutput();
    dc_.SetAsOutput();
    reset_.SetAsOutput();

    cs_.SetHigh();
    dc_.SetHigh();

    reset_.SetLow();
    Delay(100us);
    reset_.SetHigh();
    Delay(100us);

    Clear();
    InitializationPanel();
  }

  /// Clears the internal bitmap_ to zero (or a user defined clear_value)
  void Clear() override
  {
    for (auto & row : bitmap_)
    {
      for (auto & pixel_block : row)
      {
        pixel_block = 0x00;
      }
    }
  }

  /// Fill the screen with white (or what ever color the screen is)
  void Fill()
  {
    for (auto & row : bitmap_)
    {
      for (auto & pixel_block : row)
      {
        pixel_block = 0xFF;
      }
    }
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

  /// Invert the colors of the screen in a single command.
  void InvertScreenColor()
  {
    Write(0xA7, Transaction::kCommand);
  }

  /// Change the color scheme of the screen back to the normal color scheme.
  /// Usually done after executing `InvertScreenColor()`.
  void NormalScreenColor()
  {
    Write(0xA6, Transaction::kCommand);
  }

 private:
  /// Run the sequence of commands found in the user manual that initializes the
  /// device for dislaying images.
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

  /// Set the addressing mode of the device to Horizontal Address Mode. See user
  /// manual for more details as to what this is and why we use it.
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

  /// Transmits data to the display over the communication bus
  ///
  /// @param data        - data to be sent
  /// @param transaction - which form of transaction (data or command) to
  ///                      perform.
  /// @param size        - the number of bytes within the 32-bit data to send.
  void Write(uint32_t data, Transaction transaction, size_t size = 1)
  {
    dc_.Set(static_cast<sjsu::Gpio::State>(transaction));
    cs_.Set(sjsu::Gpio::State::kLow);

    bit::Mask mask = {
      .position = static_cast<uint32_t>((size - 1) * 8),
      .width    = 8,
    };

    for (size_t i = 0; i < size; i++)
    {
      spi_.Transfer(static_cast<uint8_t>(bit::Extract(data, mask)));
      mask = mask >> 8;
    }

    cs_.Set(sjsu::Gpio::State::kHigh);
  }

  sjsu::Spi & spi_;
  sjsu::Gpio & cs_;
  sjsu::Gpio & dc_;
  sjsu::Gpio & reset_;
  units::frequency::hertz_t clock_rate_;

  std::array<std::array<uint8_t, kColumns + 5>, kRows + 5> bitmap_;
};
}  // namespace sjsu
