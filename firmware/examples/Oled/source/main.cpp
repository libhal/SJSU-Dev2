#include "L2_HAL/displays/oled/ssd1306.hpp"
#include "L3_Application/graphics.hpp"
#include "utility/log.hpp"
#include "utility/time.hpp"

int main(void)
{
  Ssd1306 oled_display;
  Graphics graphics(&oled_display);
  LOG_INFO("Starting OLED Hardware Test in 5s...");
  Delay(5000);

  LOG_INFO("Initializing Graphics library along with OLED Hardware...");
  graphics.Initialize();
  Delay(1000);

  LOG_INFO("Clearing Screen...");
  graphics.Clear();
  graphics.Update();
  Delay(1000);

  LOG_INFO("Clearing Screen finished. Waiting 1 second...");
  Delay(1000);

  LOG_INFO("Drawing Some Shapes...");

  graphics.DrawHorizontalLine(0, Ssd1306::kHeight / 2, Ssd1306::kWidth);
  graphics.Update();

  graphics.DrawVerticalLine(Ssd1306::kWidth / 2, 0, Ssd1306::kHeight);
  graphics.Update();

  graphics.DrawRectangle(Ssd1306::kWidth / 2 - 10, Ssd1306::kHeight / 2 - 10,
                         20, 20);
  graphics.Update();

  graphics.DrawRectangle(Ssd1306::kWidth / 2 - 20, Ssd1306::kHeight / 2 - 20,
                         40, 40);
  graphics.Update();

  graphics.DrawLine(0, 0, Ssd1306::kWidth, Ssd1306::kHeight);
  graphics.Update();

  graphics.DrawLine(0, Ssd1306::kHeight, Ssd1306::kWidth, 0);
  graphics.Update();

  graphics.DrawCircle(Ssd1306::kWidth / 2, Ssd1306::kHeight / 2, 20);
  graphics.Update();

  graphics.DrawCircle(Ssd1306::kWidth / 2, Ssd1306::kHeight / 2, 30);
  graphics.Update();

  graphics.DrawCircle(Ssd1306::kWidth / 2, Ssd1306::kHeight / 2, 40);
  graphics.Update();

  graphics.DrawCircle(Ssd1306::kWidth / 2, Ssd1306::kHeight / 2, 60);
  graphics.Update();

  LOG_INFO("Drawing Some names...");
  const char * names[] = { "Name1", "Name2", "Name3", "Name4" };
  for (size_t i = 0; i < std::strlen(names[0]); i++)
  {
    graphics.DrawCharacter(8 * i, 0, names[0][i]);
  }
  graphics.Update();
  for (size_t i = 0; i < std::strlen(names[1]); i++)
  {
    graphics.DrawCharacter(8 * i, Ssd1306::kHeight - 8, names[1][i]);
  }
  graphics.Update();
  for (size_t i = 0; i < std::strlen(names[2]); i++)
  {
    int x_pos = (Ssd1306::kWidth - (std::strlen(names[2]) * 8)) + (8 * i);
    graphics.DrawCharacter(x_pos, 0, names[2][i]);
  }
  graphics.Update();
  for (size_t i = 0; i < std::strlen(names[3]); i++)
  {
    int x_pos = (Ssd1306::kWidth - (std::strlen(names[3]) * 8)) + (8 * i);
    graphics.DrawCharacter(x_pos, Ssd1306::kHeight - 8, names[3][i]);
  }
  graphics.Update();

  while (1)
  {
    LOG_INFO("Inverting Screen Colors...");
    oled_display.InvertScreenColor();
    Delay(5000);
    LOG_INFO("Normalizing Screen Colors...");
    oled_display.NormalScreenColor();
    Delay(5000);
  }
  return 0;
}
