#include <cstdarg>
#include <cstdint>

#include "L1_Peripheral/lpc40xx/gpio.hpp"
#include "L1_Peripheral/lpc40xx/spi.hpp"
#include "L2_HAL/displays/oled/ssd1306.hpp"
#include "L3_Application/graphics.hpp"

namespace sjsu
{
class GraphicalTerminal
{
 public:
  static constexpr size_t kCharacterHeight = 8;
  static constexpr size_t kCharacterWidth  = 8;
  static constexpr size_t kMaxColumns      = Ssd1306::kWidth / kCharacterWidth;
  static constexpr size_t kMaxRows = Ssd1306::kHeight / kCharacterHeight;

  GraphicalTerminal() {}

  void Initialize()
  {
    graphics_.Initialize();
    graphics_.Clear();
    graphics_.Update();
  }

  int printf(const char * format, ...)  // NOLINT
  {
    char buffer[256];

    va_list args;
    va_start(args, format);
    size_t characters = vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);

    size_t pos = 0;
    for (; pos < characters; pos++)
    {
      char character = buffer[pos];
      switch (character)
      {
        case '\n':
          column_ = 0;
          row_++;
          break;
        default:
          terminal_[(row_ + row_start_) % kMaxRows][column_] = character;
          column_++;
          if (column_ >= kMaxColumns)
          {
            column_ = 0;
            row_++;
          }
          break;
      }
    }
    Update();
    if (row_ >= kMaxRows)
    {
      row_start_ = (row_start_ + 1) % kMaxRows;
      row_       = kMaxRows - 1;
      ClearRow((row_ + row_start_) % kMaxRows);
    }
    return pos;
  }
  GraphicalTerminal & SetCursor(size_t x, size_t y)
  {
    column_    = x;
    row_       = y;
    row_start_ = 0;
    return *this;
  }
  GraphicalTerminal & MoveToLineStart()
  {
    column_ = 0;
    return *this;
  }
  GraphicalTerminal & Update()
  {
    for (size_t i = 0; i < kMaxRows; i++)
    {
      for (size_t j = 0; j < kMaxColumns; j++)
      {
        size_t x = j * kCharacterWidth;
        size_t y = i * kCharacterHeight;
        graphics_.DrawCharacter(x, y,
                                terminal_[(i + row_start_) % kMaxRows][j]);
      }
    }
    graphics_.Update();
    return *this;
  }
  void ClearRow(size_t row_location)
  {
    for (size_t i = 0; i < kMaxColumns; i++)
    {
      terminal_[row_location][i] = ' ';
    }
  }
  void Clear()
  {
    graphics_.Clear();
    memset(terminal_, '\0', sizeof(terminal_));
    SetCursor(0, 0);
    graphics_.Update();
  }

 private:
  lpc40xx::Spi ssp1_     = lpc40xx::Spi(lpc40xx::Spi::Bus::kSpi1);
  lpc40xx::Gpio cs_gpio_ = lpc40xx::Gpio(1, 22);
  lpc40xx::Gpio dc_gpio_ = lpc40xx::Gpio(1, 25);

  Ssd1306 oled_display_ = Ssd1306(ssp1_, cs_gpio_, dc_gpio_);
  Graphics graphics_    = Graphics(&oled_display_);

  size_t row_                           = 0;
  size_t column_                        = 0;
  size_t row_start_                     = 0;
  char terminal_[kMaxRows][kMaxColumns] = { 0 };
};
}  // namespace sjsu
