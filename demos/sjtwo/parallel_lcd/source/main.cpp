#include <cstdint>
#include <cstdio>

#include "L1_Peripheral/lpc40xx/gpio.hpp"
#include "L2_HAL/displays/lcd/st7066u.hpp"
#include "L2_HAL/io/parallel_bus/parallel_gpio.hpp"

namespace
{
// set up control pins for lcd
sjsu::lpc40xx::Gpio rs(0, 15);  // RS:    Register Select
sjsu::lpc40xx::Gpio rw(0, 16);  // RW:    Read / Write
sjsu::lpc40xx::Gpio e(0, 17);   // E:     Chip Enable
sjsu::lpc40xx::Gpio d7(2, 2);   // D7-D0: Parallel Data Pins
sjsu::lpc40xx::Gpio d6(2, 1);
sjsu::lpc40xx::Gpio d5(2, 3);
sjsu::lpc40xx::Gpio d4(2, 4);
sjsu::lpc40xx::Gpio d3(2, 5);
sjsu::lpc40xx::Gpio d2(2, 6);
sjsu::lpc40xx::Gpio d1(2, 7);
sjsu::lpc40xx::Gpio d0(2, 8);

std::array<sjsu::Gpio *, 8> data_pins = {
  &d0, &d1, &d2, &d3, &d4, &d5, &d6, &d7
};
sjsu::ParallelGpio data_bus(data_pins.data(), data_pins.size());
}  // namespace

int main()
{
  sjsu::LogInfo("Starting Parallel LCD Demo");

  constexpr uint8_t kMaxLines        = 4;
  constexpr uint8_t kMaxDisplayWidth = 20;

  sjsu::St7066u lcd(sjsu::St7066u::BusMode::kEightBit,
                    sjsu::St7066u::DisplayMode::kMultiLine,
                    sjsu::St7066u::FontStyle::kFont5x8,
                    rs,
                    rw,
                    e,
                    data_bus);
  lcd.Initialize();
  lcd.DisplayText("Parallel LCD", sjsu::St7066u::CursorPosition_t{ 1, 4 });
  lcd.DisplayText("SJSU-DEV2", sjsu::St7066u::CursorPosition_t{ 2, 5 });
  lcd.SetCursorHidden(false);

  while (true)
  {
    // Make the cursor traverse through each row
    for (uint8_t i = 0; i < kMaxLines; i++)
    {
      for (uint8_t j = 0; j < kMaxDisplayWidth; j++)
      {
        lcd.SetCursorPosition(sjsu::St7066u::CursorPosition_t{ i, j });
        sjsu::Delay(1s);
      }
    }
  }

  return 0;
}
