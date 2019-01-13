#pragma once

#include <cstddef>
#include <cstdint>

#include "utility/macros.hpp"

/// A "framebuffer" (frame buffer, or sometimes framestore) is a portion of RAM
/// containing a bitmap that drives a video display. It is a memory buffer
/// containing a complete frame of data.
///
/// DisplayInterface is a common set of methods that all hardware display
/// drivers must implement to work with the Graphics class.
class PixelDisplayInterface
{
 public:
  SJ2_PACKED(struct) Color_t
  {
    Color_t(bool r, bool g, bool b, uint8_t a, uint8_t bits,
            bool mono)
        : monochrome(mono),
          red(r),
          green(g),
          blue(b),
          padding(0),
          color_bits(bits),
          alpha(a)
    {
    }
    Color_t()
        : monochrome(false),
          red(false),
          green(false),
          blue(false),
          padding(0),
          color_bits(0),
          alpha(0)
    {
    }
    bool monochrome: 1;
    bool red: 1;
    bool green: 1;
    bool blue: 1;
    unsigned padding: 4;
    uint8_t color_bits;
    uint8_t alpha;
  };

  virtual size_t GetWidth()  = 0;
  virtual size_t GetHeight() = 0;
  /// @returns a color object with the available colors.
  virtual Color_t AvailableColors() = 0;
  /// Configure hardware peripherals and initialize external display hardware
  virtual void Initialize() = 0;
  /// Optional method to turn on display if applicable
  virtual void Enable() {}
  /// Optional method to turn off display and potentially put it into a low
  /// power mode
  virtual void Disable() {}
  /// Clear the screen of the display
  virtual void Clear() = 0;
  /// Draw the specified pixel.
  /// Implementations of this method should not be expected to the draw the
  /// pixel on the display, immediately after this method has finished.
  /// A class implementing this interface should write/manipulate a framebuffer
  /// and update the screen after a call to the Update() method.
  ///
  /// @param x x coordinate position to draw the pixel
  /// @param y y coordinate position to draw the pixel
  /// @param color the color of the pixel. May be ignored on monochrome screens.
  virtual void DrawPixel(int32_t x, int32_t y, Color_t color) = 0;
  /// Update screen to match framebuffer.
  /// Implementations of this method that do not use a framebuffer, possibly
  /// due to memory constrains, can refrain from implementing this function.
  virtual void Update() {}
};
