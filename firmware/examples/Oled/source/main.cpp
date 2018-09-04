#include "L2_Utilities/log.hpp"
#include "L2_Utilities/time.hpp"
#include "L3_HAL/ssd1306.hpp"

int main(void)
{
  Ssd1306 display;
  LOG_INFO("Starting OLED Hardware Test...");

  LOG_INFO("Initializing OLED Hardware Test...");
  display.Initialize();

  LOG_INFO("Clearing Screen...");
  display.Clear();

  LOG_INFO("Clearing Screen finished. Waiting 1 second...");
  Delay(1000);

  LOG_INFO("Drawing Cross Hair...");
  display.DrawHorizontalLine(0, Ssd1306::kHeight/2, Ssd1306::kWidth);
  display.DrawVerticalLine(Ssd1306::kWidth/2, 0, Ssd1306::kHeight);
  display.DrawRectangle(Ssd1306::kWidth/2-10, Ssd1306::kHeight/2-10, 20, 20);
  display.Update();

  while (1)
  {
    Delay(1000);
    LOG_INFO("Inverting screen...");
    display.InvertScreenColor();
    Delay(1000);
    LOG_INFO("Screen normalized...");
    display.NormalScreenColor();
  }
  return 0;
}
