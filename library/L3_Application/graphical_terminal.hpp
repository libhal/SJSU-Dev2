#pragma once

#include <cstdarg>
#include <cstdint>

#include "L1_Peripheral/lpc40xx/gpio.hpp"
#include "L1_Peripheral/lpc40xx/spi.hpp"
#include "L2_HAL/displays/oled/ssd1306.hpp"
#include "L3_Application/graphics.hpp"

namespace sjsu
{
template <uint32_t kMaxRows, uint32_t kMaxColumns>
struct TerminalCache_t
{
  char buffer[kMaxRows * kMaxColumns] = { 0 };
};

class GraphicalTerminal
{
 public:
  static constexpr uint8_t kCharacterHeight = 8;
  static constexpr uint8_t kCharacterWidth  = 8;

  template <uint32_t kMaxRows, uint32_t kMaxColumns>
  explicit GraphicalTerminal(Graphics * graphics,
                             TerminalCache_t<kMaxRows, kMaxColumns> * cache)
      : max_rows_(kMaxRows),
        max_columns_(kMaxColumns),
        graphics_(graphics),
        cache_(cache->buffer)
  {
  }

  void Initialize()
  {
    graphics_->Initialize();
    graphics_->Clear();
    graphics_->Update();
  }

  int printf(const char * format, ...)  // NOLINT
  {
    char buffer[256];

    va_list args;
    va_start(args, format);
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wformat-nonliteral"
    uint32_t characters = vsnprintf(buffer, sizeof(buffer), format, args);
#pragma GCC diagnostic pop
    va_end(args);

    uint32_t pos = 0;
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
          GetChar((row_ + row_start_) % max_rows_, column_) = character;
          column_++;
          if (column_ >= max_columns_)
          {
            column_ = 0;
            row_++;
          }
          break;
      }
    }
    Update();
    if (row_ >= max_rows_)
    {
      row_start_ = (row_start_ + 1) % max_rows_;
      row_       = max_rows_ - 1;
      ClearRow((row_ + row_start_) % max_rows_);
    }
    return pos;
  }

  GraphicalTerminal & SetCursor(uint32_t x, uint32_t y)
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
    graphics_->Clear();
    for (int32_t i = 0; i < static_cast<int32_t>(max_rows_); i++)
    {
      for (int32_t j = 0; j < static_cast<int32_t>(max_columns_); j++)
      {
        int32_t x = j * kCharacterWidth;
        int32_t y = i * kCharacterHeight;
        graphics_->DrawCharacter(
            x, y, GetChar(((i + row_start_) % max_rows_), j));
      }
    }
    graphics_->Update();
    return *this;
  }

  GraphicalTerminal & ClearRow(uint32_t row_location)
  {
    for (uint32_t i = 0; i < max_columns_; i++)
    {
      GetChar(row_location, i) = ' ';
    }
    return *this;
  }

  GraphicalTerminal & Clear()
  {
    graphics_->Clear();
    memset(cache_, '\0', max_rows_ * max_columns_);
    SetCursor(0, 0);
    graphics_->Update();
    return *this;
  }

 private:
  char & GetChar(uint32_t row, uint32_t column)
  {
    return cache_[(row * max_columns_) + column];
  }

  uint32_t row_       = 0;
  uint32_t column_    = 0;
  uint32_t row_start_ = 0;
  uint32_t max_rows_;
  uint32_t max_columns_;
  Graphics * graphics_;
  char * cache_;
};
}  // namespace sjsu
