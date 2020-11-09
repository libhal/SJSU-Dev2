#pragma once

#include <cstddef>
#include <cstdint>

#include "L2_HAL/displays/pixel_display.hpp"
#include "utility/error_handling.hpp"

namespace sjsu
{
/// PixelDisplay is a common set of methods that all hardware display drivers
/// must implement to work with the Graphics class.
class TestPixelDisplay : public PixelDisplay
{
 public:
  /// Returns the number of pixels wide the display is.
  size_t GetWidth() override {return 128;}

  /// Returns the number of pixels high the display is.
  size_t GetHeight() override {return 64;}

  /// @returns a color object with the available colors.
  Color_t AvailableColors() override {}

  /// Configure hardware peripherals and initialize external display hardware
  void Initialize() override {}

  /// Optional method to turn on display if applicable
  void Enable() override {}

  /// Optional method to turn off display and potentially put it into a low
  /// power mode
  void Disable() override {}

  /// Clear framebuffer
  void Clear() override {}

  /// Draw the specified pixel.
  /// Implementations of this method should not draw directly to the display but
  /// should manipulate A class implementing this interface should
  /// write/manipulate a framebuffer and update the screen after a call to the
  /// Update() method.
  ///
  /// @param x x coordinate position to draw the pixel
  /// @param y y coordinate position to draw the pixel
  /// @param color the color of the pixel. May be ignored on monochrome screens.
  void DrawPixel(int32_t x, int32_t y, Color_t color) override
  {
    pixel_test[y][x] = 1;
  }

  /// Update screen to match framebuffer.
  /// Implementations of this method that do not use a framebuffer, possibly
  /// due to memory constrains, can refrain from implementing this function.
  void Update() override {}
  /// Test 2D array of pixel locations.
  std::array<std::array<int, 128>, 64> pixel_test;
};
}  // namespace sjsu
