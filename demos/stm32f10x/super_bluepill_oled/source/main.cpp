#include "platforms/utility/startup.hpp"
#include "peripherals/spi.hpp"
#include "peripherals/stm32f10x/gpio.hpp"
#include "peripherals/stm32f10x/system_controller.hpp"
#include "devices/displays/oled/ssd1306.hpp"
#include "systems/graphics/graphics.hpp"
#include "utility/log.hpp"
#include "utility/time/time.hpp"

namespace
{
class BitBangSpi : public sjsu::Spi
{
 public:
  BitBangSpi(sjsu::Gpio & sck, sjsu::Gpio & mosi) : sck_(sck), mosi_(mosi) {}

  void ModuleInitialize() override
  {
    mosi_.Initialize();
    sck_.Initialize();

    mosi_.SetAsOutput();
    sck_.SetAsOutput();
    sck_.SetLow();
    mosi_.SetLow();
  }

  void Transfer(std::span<uint8_t> buffer) override
  {
    for (auto & data : buffer)
    {
      for (int i = 0; i < 8; i++)
      {
        // Set MOSI pin to the correct bit
        mosi_.Set(sjsu::Gpio::State{ sjsu::bit::Read(data, 7 - i) });

        // Clock the SCK pin
        sck_.SetHigh();
        sck_.SetLow();
      }
    }
    sck_.SetLow();
  }

  void Transfer(std::span<uint16_t>) override
  {
    sjsu::LogInfo("NOT SUPPORTED!");
  }

 private:
  sjsu::Gpio & sck_;
  sjsu::Gpio & mosi_;
};
}  // namespace

int main()
{
  sjsu::LogWarning(
      "Starting OLED Application (targeted for Super Bluepill board)...");

  /// OLED display for this chip is not connected to a SPI bus, thus clocking is
  /// done manually by the MCU, which will require clocking the system up to its
  /// maximum to get a decent framerate.
  sjsu::stm32f10x::SetMaximumClockSpeedUsingInternalOscillator();

  sjsu::stm32f10x::Gpio sck('B', 1);
  sjsu::stm32f10x::Gpio mosi('B', 3);
  sjsu::stm32f10x::Gpio reset('B', 5);
  BitBangSpi spi(sck, mosi);

  sjsu::stm32f10x::Gpio dc_gpio('B', 7);
  sjsu::stm32f10x::Gpio cs_gpio('B', 9);

  sjsu::Ssd1306 oled_display(spi, cs_gpio, dc_gpio, reset);
  sjsu::Graphics graphics(oled_display);
  sjsu::LogInfo("Starting OLED Hardware Test in 1s...");
  sjsu::Delay(1s);

  sjsu::LogInfo("Starting OLED Display Initialized...");
  oled_display.Initialize();

  while (true)
  {
    sjsu::LogInfo("Clearing Screen...");
    graphics.Clear();
    graphics.Update();
    sjsu::Delay(1s);

    sjsu::LogInfo("Clearing Screen finished. Waiting 1 second...");
    sjsu::Delay(1s);

    sjsu::LogInfo("Drawing Some Shapes...");

    graphics.DrawHorizontalLine(
        0, sjsu::Ssd1306::kHeight / 2, sjsu::Ssd1306::kWidth);
    graphics.Update();

    graphics.DrawVerticalLine(
        sjsu::Ssd1306::kWidth / 2, 0, sjsu::Ssd1306::kHeight);
    graphics.Update();

    graphics.DrawRectangle(sjsu::Ssd1306::kWidth / 2 - 10,
                           sjsu::Ssd1306::kHeight / 2 - 10,
                           20,
                           20);
    graphics.Update();

    graphics.DrawRectangle(sjsu::Ssd1306::kWidth / 2 - 20,
                           sjsu::Ssd1306::kHeight / 2 - 20,
                           40,
                           40);
    graphics.Update();

    graphics.DrawLine(0, 0, sjsu::Ssd1306::kWidth, sjsu::Ssd1306::kHeight);
    graphics.Update();

    graphics.DrawLine(0, sjsu::Ssd1306::kHeight, sjsu::Ssd1306::kWidth, 0);
    graphics.Update();

    graphics.DrawCircle(
        sjsu::Ssd1306::kWidth / 2, sjsu::Ssd1306::kHeight / 2, 20);
    graphics.Update();

    graphics.DrawCircle(
        sjsu::Ssd1306::kWidth / 2, sjsu::Ssd1306::kHeight / 2, 30);
    graphics.Update();

    graphics.DrawCircle(
        sjsu::Ssd1306::kWidth / 2, sjsu::Ssd1306::kHeight / 2, 40);
    graphics.Update();

    graphics.DrawCircle(
        sjsu::Ssd1306::kWidth / 2, sjsu::Ssd1306::kHeight / 2, 60);
    graphics.Update();

    sjsu::LogInfo("Drawing Some names...");
    const char * names[] = { "Name1", "Name2", "Name3", "Name4" };
    for (size_t i = 0; i < std::strlen(names[0]); i++)
    {
      graphics.DrawCharacter(8 * i, 0, names[0][i]);
    }
    graphics.Update();

    for (size_t i = 0; i < std::strlen(names[1]); i++)
    {
      graphics.DrawCharacter(8 * i, sjsu::Ssd1306::kHeight - 8, names[1][i]);
    }
    graphics.Update();

    for (size_t i = 0; i < std::strlen(names[2]); i++)
    {
      int x_pos =
          (sjsu::Ssd1306::kWidth - (std::strlen(names[2]) * 8)) + (8 * i);
      graphics.DrawCharacter(x_pos, 0, names[2][i]);
    }
    graphics.Update();

    for (size_t i = 0; i < std::strlen(names[3]); i++)
    {
      int x_pos =
          (sjsu::Ssd1306::kWidth - (std::strlen(names[3]) * 8)) + (8 * i);
      graphics.DrawCharacter(x_pos, sjsu::Ssd1306::kHeight - 8, names[3][i]);
    }
    graphics.Update();

    while (1)
    {
      sjsu::LogInfo("Inverting Screen Colors...");
      oled_display.InvertScreenColor();
      sjsu::Delay(2500ms);
      sjsu::LogInfo("Normalizing Screen Colors...");
      oled_display.NormalScreenColor();
      sjsu::Delay(2500ms);
    }
  }

  return 0;
}
