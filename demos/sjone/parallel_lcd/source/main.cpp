#include <cstdint>
#include <cstdio>

#include "L1_Peripheral/lpc17xx/gpio.hpp"
#include "L2_HAL/displays/lcd/st7066u.hpp"
#include "L2_HAL/io/parallel_bus/parallel_gpio.hpp"

int main()
{
  sjsu::LogInfo("Starting Parallel LCD Demo");

  // set up control pins for lcd
  sjsu::lpc17xx::Gpio rs(0, 29);  // RS:    Register Select
  sjsu::lpc17xx::Gpio rw(0, 30);  // RW:    Read / Write
  sjsu::lpc17xx::Gpio e(1, 19);   // E:     Chip Enable
  sjsu::lpc17xx::Gpio d7(2, 0);   // D7-D0: Parallel Data Pins
  sjsu::lpc17xx::Gpio d6(2, 1);
  sjsu::lpc17xx::Gpio d5(2, 2);
  sjsu::lpc17xx::Gpio d4(2, 3);
  sjsu::lpc17xx::Gpio d3(2, 4);
  sjsu::lpc17xx::Gpio d2(2, 5);
  sjsu::lpc17xx::Gpio d1(2, 6);
  sjsu::lpc17xx::Gpio d0(2, 7);

  std::array<sjsu::Gpio *, 8> data_pins = {
    &d0, &d1, &d2, &d3, &d4, &d5, &d6, &d7,
  };

  sjsu::ParallelGpio data_bus(data_pins);

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
  lcd.Enable();

  sjsu::LogInfo("Drawing text to screen at different locations...");
  lcd.DrawText("Parallel LCD Demo", sjsu::St7066u::CursorPosition_t{ 1, 4 });
  lcd.DrawText("SJSU-DEV2", sjsu::St7066u::CursorPosition_t{ 2, 5 });

  lcd.SetCursorHidden(false);

  sjsu::LogInfo("Making the cursor traverse through each row...");

  while (true)
  {
    // Make the cursor traverse through each row
    for (uint8_t i = 0; i < kMaxLines; i++)
    {
      for (uint8_t j = 0; j < kMaxDisplayWidth; j++)
      {
        lcd.SetCursorPosition(sjsu::St7066u::CursorPosition_t{ i, j });
        sjsu::Delay(100ms);
      }
    }
  }

  return 0;
}
