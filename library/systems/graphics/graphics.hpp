#pragma once

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <cstdint>

#include "module.hpp"
#include "devices/displays/pixel_display.hpp"
#include "third_party/font8x8/font8x8_basic.h"
#include "utility/log.hpp"

namespace sjsu
{
/// Graphics library to draw shapes and characters on a pixel display
class Graphics : public Module<>
{
 public:
  /// Constructor for a graphics object.
  ///
  /// @param display - reference to a pixel display
  explicit Graphics(PixelDisplay & display)
      : display_(display), color_(), width_(0), height_(0)
  {
    width_  = display.GetWidth();
    height_ = display.GetHeight();
    color_  = display.AvailableColors();
  }

  /// Initialize display hardware.
  void ModuleInitialize() override
  {
    display_.Initialize();
  }

  /// Update the display.
  void Update()
  {
    display_.Update();
  }

  /// Clears the display.
  void Clear()
  {
    display_.Clear();
  }

  /// Set the current color of drawn elements.
  void SetColor(PixelDisplay::Color_t color)
  {
    color_ = color;
  }

  /// Get the current color of drawn elements.
  PixelDisplay::Color_t GetColor()
  {
    return color_;
  }

  /// Get available colors of the display.
  PixelDisplay::Color_t GetAvailableColor()
  {
    return display_.AvailableColors();
  }

  /// Draw a horizontal line
  ///
  /// @param x - starting x coordinate
  /// @param y - starting y coordinate
  /// @param line_width - length of the line going to the right
  void DrawHorizontalLine(int32_t x, int32_t y, int32_t line_width)
  {
    line_width = std::clamp(
        x + line_width, int32_t{ 0 }, static_cast<int32_t>(width_ - x));
    for (int32_t column = x; column < line_width; column++)
    {
      DrawPixel(column, y);
    }
  }

  /// Draw a vertical line
  ///
  /// @param x - start x coordinate
  /// @param y - start y coordinate
  /// @param line_height - length of the line going down.
  void DrawVerticalLine(int32_t x, int32_t y, int32_t line_height)
  {
    line_height = std::clamp(
        y + line_height, int32_t{ 0 }, static_cast<int32_t>(height_ - y));
    for (int32_t row = y; row < line_height; row++)
    {
      DrawPixel(x, row);
    }
  }

  /// Draw a line.
  ///
  /// @param x0 - start x position
  /// @param y0 - start y position
  /// @param x1 - end x position
  /// @param y1 - end y position
  void DrawLine(int32_t x0, int32_t y0, int32_t x1, int32_t y1)
  {
    float dx           = static_cast<float>(x1 - x0);
    float dy           = static_cast<float>(y1 - y0);
    float steps        = std::max(std::abs(dx), std::abs(dy));
    uint32_t int_steps = static_cast<uint32_t>(ceilf(steps));
    float x_increment  = dx / steps;
    float y_increment  = dy / steps;

    float x = static_cast<float>(x0);
    float y = static_cast<float>(y0);
    for (uint32_t v = 0; v < int_steps; v++)
    {
      x = x + x_increment;
      y = y + y_increment;
      DrawPixel(static_cast<uint32_t>(std::lround(x)),
                static_cast<uint32_t>(std::lround(y)));
    }
  }

  /// Draw a circle on the display.
  ///
  /// @param x0 - center x position of the circle.
  /// @param y0 - center y position of the circle.
  /// @param radius - the radius of the circle.
  void DrawCircle(int32_t x0, int32_t y0, int32_t radius)
  {
    int32_t x   = radius - 1;
    int32_t y   = 0;
    int32_t dx  = 1;
    int32_t dy  = 1;
    int32_t err = dx - (radius << 1);

    while (x >= y)
    {
      DrawPixel(x0 + x, y0 + y);
      DrawPixel(x0 + y, y0 + x);
      DrawPixel(x0 - y, y0 + x);
      DrawPixel(x0 - x, y0 + y);
      DrawPixel(x0 - x, y0 - y);
      DrawPixel(x0 - y, y0 - x);
      DrawPixel(x0 + y, y0 - x);
      DrawPixel(x0 + x, y0 - y);
      if (err <= 0)
      {
        y++;
        err += dy;
        dy += 2;
      }
      else
      {
        x--;
        dx += 2;
        err += dx - (radius << 1);
      }
    }
  }

  /// Draw a character on the screen
  ///
  /// @param x - x coordinate
  /// @param y - y coordinate
  /// @param width - width of the rectangle
  /// @param height - height of the rectangle
  void DrawRectangle(int32_t x, int32_t y, int32_t width, int32_t height)
  {
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

  /// Draw a character on the display.
  ///
  /// @param x0 - X coordinate to start printing to the screen
  /// @param y0 - Y coordinate to start printing to the screen
  /// @param letter - The character to write to the screen
  void DrawCharacter(int32_t x0, int32_t y0, char letter)
  {
    int32_t letter_position = int32_t{ letter };

    PixelDisplay::Color_t foreground = color_;

    for (int32_t y = 0; y < 8; y++)
    {
      for (int32_t x = 0; x < 8; x++)
      {
        if (font8x8_basic[letter_position][y] & (1 << x))
        {
          DrawPixel(x0 + x, y0 + y);
        }
      }
    }
    color_ = foreground;
  }

  /// Put a pixel on a specific position.
  ///
  /// @param x - x coordinate to place the coordinate.
  /// @param y - y coordinate to place the coordinate.
  void DrawPixel(uint32_t x, uint32_t y)
  {
    // Pixels outside of the bounds of the screen will not be drawn.
    if (x <= width_ && y <= height_)
    {
      display_.DrawPixel(x, y, color_);
    }
  }

 private:
  PixelDisplay & display_;
  PixelDisplay::Color_t color_;
  size_t width_;
  size_t height_;
};
}  // namespace sjsu
