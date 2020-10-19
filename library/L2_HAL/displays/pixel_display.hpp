#pragma once

#include <cstddef>
#include <cstdint>

#include "module.hpp"
#include "utility/error_handling.hpp"

namespace sjsu
{
/// PixelDisplay is a common set of methods that all hardware display drivers
/// must implement to work with the Graphics class.
class PixelDisplay : public Module
{
 public:
  /// Describes the color space and resolution of the display.
  struct Color_t
  {
    /// Bits of the red channel
    uint8_t red = 0;
    /// Bits of the green channel
    uint8_t green = 0;
    /// Bits of the blue channel
    uint8_t blue = 0;
    /// Bits of alpha (transparent) channel
    uint8_t alpha = 0;
    /// @returns true if the Color_t definitions is
    ///          { .red = 0, .green = 0, .blue = 0, .alpha = 0} which indicates
    ///           monochrome, when looking
    bool IsMonoChrome()
    {
      return red == 0 && green == 0 && blue == 0 && alpha == 0;
    }
    /// @returns true if the Color_t definitions is
    ///          { .red = 0, .green = 0, .blue = 0, .alpha = 0} which indicates
    ///           no color and invisible.
    bool IsBlank()
    {
      return red == 0 && green == 0 && blue == 0 && alpha == 0;
    }
  };

  /// Returns the number of pixels wide the display is.
  virtual size_t GetWidth() = 0;

  /// Returns the number of pixels high the display is.
  virtual size_t GetHeight() = 0;

  /// @returns a color object with the available colors.
  virtual Color_t AvailableColors() = 0;

  /// Clear framebuffer
  virtual void Clear() = 0;

  /// Draw the specified pixel.
  /// Implementations of this method should not draw directly to the display but
  /// should manipulate A class implementing this interface should
  /// write/manipulate a framebuffer and update the screen after a call to the
  /// Update() method.
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
}  // namespace sjsu
