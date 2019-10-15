#include <cstdint>
#include <cstdio>

#include "L1_Peripheral/lpc40xx/gpio.hpp"
#include "L2_HAL/displays/lcd/st7066u.hpp"

int main()
{
  constexpr uint8_t kMaxLines        = 4;
  constexpr uint8_t kMaxDisplayWidth = 20;

  LOG_INFO("Starting Parallel LCD Demo\n");

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
  sjsu::St7066u::ControlPins_t pins =
      sjsu::St7066u::ControlPins_t{ rs, rw, e, d7, d6, d5, d4, d3, d2, d1, d0 };

  sjsu::St7066u lcd(sjsu::St7066u::BusMode::kEightBit,
                    sjsu::St7066u::DisplayMode::kMultiLine,
                    sjsu::St7066u::FontStyle::kFont5x8, pins);

  lcd.Initialize();

  lcd.DisplayText("Parallel LCD", sjsu::St7066u::CursorPosition_t{ 1, 4 });
  lcd.DisplayText("SJSU-DEV2", sjsu::St7066u::CursorPosition_t{ 2, 5 });

  lcd.SetCursorHidden(false);
  while (true)
  {
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
