#pragma once

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <cstdint>

#include "L2_HAL/displays/pixel_display.hpp"
#include "third_party/font8x8/font8x8_basic.h"
#include "utility/log.hpp"

class Graphics
{
 public:
  explicit Graphics(PixelDisplayInterface * display)
      : display_(display), color_(), width_(0), height_(0)
  {
    width_  = display->GetWidth();
    height_ = display->GetHeight();
    color_  = display->AvailableColors();
  }

  void SetColor(PixelDisplayInterface::Color_t color)
  {
    color_ = color;
  }

  void Initialize()
  {
    display_->Initialize();
    display_->Enable();
  }

  void DrawHorizontalLine(int32_t x, int32_t y, int32_t line_width)
  {
    LOG_DEBUG("x = %" PRId32 " :: y = %" PRId32 " :: line_width = %" PRId32 "",
              x, y, line_width);
    line_width = std::clamp(x + line_width, int32_t(0), int32_t(width_ - x));
    for (int32_t column = x; column < line_width; column++)
    {
      DrawPixel(column, y);
    }
  }

  void DrawVerticalLine(int32_t x, int32_t y, int32_t line_height)
  {
    line_height = std::clamp(y + line_height, int32_t(0), int32_t(height_ - y));
    LOG_DEBUG("x = %" PRId32 " :: y = %" PRId32 " :: line_height = %" PRId32 "",
              x, y, line_height);
    for (int32_t row = y; row < line_height; row++)
    {
      DrawPixel(x, row);
    }
  }

  void DrawLine(int32_t x0, int32_t y0, int32_t x1, int32_t y1)
  {
    LOG_DEBUG("(x0, y0, x1, y1) -> (%" PRId32 ", %" PRId32 ", %" PRId32
              ", %" PRId32 ")",
              x0, y0, x1, y1);
    float dx           = static_cast<float>(x1 - x0);
    float dy           = static_cast<float>(y1 - y0);
    float steps        = std::max(std::abs(dx), std::abs(dy));
    uint32_t int_steps = static_cast<uint32_t>(ceilf(steps));
    float x_increment  = dx / steps;
    float y_increment  = dy / steps;
    LOG_DEBUG("steps = %f :: dx = %f :: dy = %f :: x-inc = %f :: y-inc = %f",
              static_cast<double>(steps), static_cast<double>(dx),
              static_cast<double>(dy), static_cast<double>(x_increment),
              static_cast<double>(y_increment));

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

  void DrawRectangle(int32_t x, int32_t y, int32_t width, int32_t height)
  {
    LOG_DEBUG("x: %" PRId32 " :: y: %" PRId32 " :: h: %" PRId32
              " :: w: %" PRId32,
              x, y, width, height);
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

  void DrawCharacter(int32_t x0, int32_t y0, char letter,
                     bool fill_background = true)
  {
    int32_t letter_position = static_cast<int32_t>(letter);

    PixelDisplayInterface::Color_t foreground = color_;
    PixelDisplayInterface::Color_t background =
        PixelDisplayInterface::Color_t(0, 0, 0, 0, 1, true);

    for (int32_t y = 0; y < 8; y++)
    {
      for (int32_t x = 0; x < 8; x++)
      {
        if (font8x8_basic[letter_position][y] & (1 << x))
        {
          color_ = foreground;
        }
        else if (fill_background)
        {
          color_ = background;
        }
        DrawPixel(x0 + x, y0 + y);
      }
    }
    color_ = foreground;
  }

  void DrawPixel(uint32_t x, uint32_t y)
  {
    // Pixels outside of the bounds of the screen will not be drawn.
    if (x <= width_ && y <= height_)
    {
      display_->DrawPixel(x, y, color_);
    }
  }
  void Update()
  {
    display_->Update();
  }
  void Clear()
  {
    display_->Clear();
  }

 private:
  PixelDisplayInterface * display_;
  PixelDisplayInterface::Color_t color_;
  size_t width_;
  size_t height_;
};
