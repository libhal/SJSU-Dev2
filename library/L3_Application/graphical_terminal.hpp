#pragma once

#include <cstdarg>
#include <cstdint>

#include "module.hpp"
#include "L1_Peripheral/lpc40xx/gpio.hpp"
#include "L1_Peripheral/lpc40xx/spi.hpp"
#include "L2_HAL/displays/oled/ssd1306.hpp"
#include "L3_Application/graphics.hpp"

namespace sjsu
{
/// Character storage for a terminal. Used with classes such as
/// GraphicalTerminal to hold and buffer the characters to be drawn on a
/// display.
///
/// @tparam kMaxRows - number of rows
/// @tparam kMaxColumns - number of columns
template <uint32_t kMaxRows, uint32_t kMaxColumns>
struct TerminalCache_t
{
  /// Buffer containing the characters to display on the terminal
  char buffer[kMaxRows * kMaxColumns] = { 0 };
};

/// Utilizes a pixel display and a terminal character cache to create a
/// Graphical Terminal on that display.
class GraphicalTerminal : public Module
{
 public:
  /// Maximum height of the font used for the graphical display
  static constexpr uint8_t kCharacterHeight = 8;
  /// Maximum width of the font used for the graphical display
  static constexpr uint8_t kCharacterWidth = 8;

  /// Constructs the GraphicalTerminal Object
  ///
  /// @tparam kMaxRows - Do not enter these in manually. These are deduced by
  ///         the passed in TerminalCache object.
  /// @tparam kMaxColumns - Do not enter these in manually. These are deduced by
  ///         the passed in TerminalCache object.
  /// @param graphics - graphical pixel display driver
  /// @param cache - terminal cache to hold the characters to be written to the
  ///                display.
  template <uint32_t kMaxRows, uint32_t kMaxColumns>
  explicit GraphicalTerminal(Graphics * graphics,
                             TerminalCache_t<kMaxRows, kMaxColumns> * cache)
      : max_rows_(kMaxRows),
        max_columns_(kMaxColumns),
        graphics_(graphics),
        cache_(cache->buffer)
  {
  }

  /// Initialize the graphics driver.
  void ModuleInitialize() override
  {
    graphics_->Initialize();
  }

  void ModuleEnable(bool enable = true) override
  {
    if (enable)
    {
      graphics_->Enable();
      graphics_->Clear();
      graphics_->Update();
    }
    else
    {
      graphics_->Enable(false);
    }
  }

  /// Prints to the screen as printf would to STDOUT.
  ///
  /// @param format - format string.
  /// @param ... - set of variables to write into the format.
  /// @return int - number of characters written to the screen.
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

  /// Move the position where text will be written from to this x & y location.
  ///
  /// @param x - x position to place cursor (column)
  /// @param y - y position to place cursor (row)
  /// @return GraphicalTerminal& - a reference to itself so that these methods
  ///        can be chained.
  GraphicalTerminal & SetCursor(uint32_t x, uint32_t y)
  {
    column_    = x;
    row_       = y;
    row_start_ = 0;
    return *this;
  }

  /// Moves the cursor back to the start of the line, the first column.
  ///
  /// @return GraphicalTerminal& - a reference to itself so that these methods
  ///         can be chained.
  GraphicalTerminal & MoveToLineStart()
  {
    column_ = 0;
    return *this;
  }

  /// Update the screen with the contents of the graphical display.
  ///
  /// @return GraphicalTerminal& - a reference to itself so that these methods
  ///        can be chained.
  GraphicalTerminal & Update()
  {
    graphics_->Clear();
    for (int32_t i = 0; i < static_cast<int32_t>(max_rows_); i++)
    {
      for (int32_t j = 0; j < static_cast<int32_t>(max_columns_); j++)
      {
        int32_t x = j * kCharacterWidth;
        int32_t y = i * kCharacterHeight;
        graphics_->DrawCharacter(x, y,
                                 GetChar(((i + row_start_) % max_rows_), j));
      }
    }
    graphics_->Update();
    return *this;
  }

  /// Clear an entire row of its contents
  ///
  /// @param row_location - which row to clear
  /// @return GraphicalTerminal& - a reference to itself so that these methods
  ///        can be chained.
  GraphicalTerminal & ClearRow(uint32_t row_location)
  {
    for (uint32_t i = 0; i < max_columns_; i++)
    {
      GetChar(row_location, i) = ' ';
    }
    return *this;
  }

  /// Clear the whole terminal of its contents and updates the screen.
  ///
  /// @return GraphicalTerminal& - a reference to itself so that these methods
  ///        can be chained.
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
