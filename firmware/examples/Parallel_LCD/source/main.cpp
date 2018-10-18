#include <cstdint>
#include <cstdio>

#include "L3_HAL/st7066u.hpp"

int main()
{
  constexpr uint8_t kMaxLines        = 4;
  constexpr uint8_t kMaxDisplayWidth = 20;

  DEBUG_PRINT("Starting Parallel LCD Demo\n");

  // set up control pins for lcd
  Gpio rs(0, 15);  // RS:    Register Select
  Gpio rw(0, 16);  // RW:    Read / Write
  Gpio e(0, 17);   // E:     Chip Enable
  Gpio d7(2, 2);   // D7-D0: Parallel Data Pins
  Gpio d6(2, 1);
  Gpio d5(2, 3);
  Gpio d4(2, 4);
  Gpio d3(2, 5);
  Gpio d2(2, 6);
  Gpio d1(2, 7);
  Gpio d0(2, 8);
  St7066u::ControlPins_t pins =
      St7066u::ControlPins_t{ rs, rw, e, d7, d6, d5, d4, d3, d2, d1, d0 };

  St7066u lcd =
      St7066u(St7066u::BusMode::kEightBit, St7066u::DisplayMode::kMultiLine,
              St7066u::FontStyle::kFont5x8, pins);

  lcd.Initialize();

  lcd.DisplayText("Parallel LCD", St7066u::CursorPosition_t{ 1, 4 });
  lcd.DisplayText("SJSU-DEV2", St7066u::CursorPosition_t{ 2, 5 });

  lcd.SetCursorHidden(false);
  while (1)
  {
    for (uint8_t i = 0; i < kMaxLines; i++)
    {
      for (uint8_t j = 0; j < kMaxDisplayWidth; j++)
      {
        lcd.SetCursorPosition(St7066u::CursorPosition_t{ i, j });
        Delay(1000);
      }
    }
  }

  return 0;
}
