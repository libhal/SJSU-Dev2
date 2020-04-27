#include "L0_Platform/startup.hpp"

#include "L1_Peripheral/stm32f10x/gpio.hpp"
#include "L1_Peripheral/spi.hpp"

#include "L2_HAL/displays/oled/ssd1306.hpp"

#include "L3_Application/graphics.hpp"

#include "utility/time.hpp"
#include "utility/log.hpp"

namespace
{
class BitBangSpi : public sjsu::Spi
{
 public:
  BitBangSpi(sjsu::Gpio & sck, sjsu::Gpio & mosi) : sck_(sck), mosi_(mosi) {}

  sjsu::Status Initialize() const override
  {
    mosi_.SetAsOutput();
    sck_.SetAsOutput();
    sck_.SetLow();
    mosi_.SetLow();
    return sjsu::Status::kSuccess;
  }

  uint16_t Transfer(uint16_t data) const override
  {
    for (int i = 0; i < 8; i++)
    {
      // Set MOSI pin to the correct bit
      mosi_.Set(sjsu::Gpio::State{ sjsu::bit::Read(data, 7 - i) });

      // Clock the SCK pin
      sck_.SetHigh();
      sck_.SetLow();
    }

    sck_.SetLow();
    return 0xFF;
  }

  void SetDataSize(DataSize size) const override
  {
    size_ = size;
  }

  void SetClock(units::frequency::hertz_t,
                bool = false,
                bool = false) const override
  {
  }

 private:
  sjsu::Gpio & sck_;
  sjsu::Gpio & mosi_;
  mutable DataSize size_;
};

void SetMaximumClockSpeed()
{
  using sjsu::stm32f10x::SystemController;
  auto & config =
      sjsu::SystemController::GetPlatformController()
          .GetClockConfiguration<SystemController::ClockConfiguration>();

  // Set the speed of the high speed external oscillator.
  // NOTE: Change this if its different for your board.
  config.high_speed_external = 8_MHz;
  // Set the source of the PLL's oscillator to 4MHz.
  // See page 93 in RM0008 to see that the internal high speed oscillator,
  // which is 8MHz, is divided by 2 before being fed to the PLL to get 4MHz.
  config.pll.source = SystemController::PllSource::kHighSpeedExternal;
  // Enable PLL to increase the frequency of the system
  config.pll.enable = true;
  // Multiply the 8MHz * 9 => 72 MHz
  config.pll.multiply = SystemController::PllMultiply::kMultiplyBy9;
  // Set the system clock to the PLL
  config.system_clock = SystemController::SystemClockSelect::kPll;
  // APB1's maximum frequency is 36 MHz, so we divide 72 MHz / 2 => 36 MHz.
  config.ahb.apb1.divider = SystemController::APBDivider::kDivideBy2;
  // Keep APB1's clock undivided as it can handle up to 72 MHz.
  config.ahb.apb2.divider = SystemController::APBDivider::kDivideBy1;
  // Maximum frequency for ADC is 12 MHz, thus we divide 72 MHz / 6 to get
  // 12 MHz.
  config.ahb.apb2.adc.divider = SystemController::AdcDivider::kDivideBy6;

  // Initialize platform with new clock configuration settings.
  sjsu::InitializePlatform();
}
}  // namespace

int main()
{
  sjsu::LogWarning(
      "Starting OLED Application (targeted for Super Bluepill board)...");

  // Pin PB3 is used by JTAG and the OLED display's data pin.
  // We need to release the PB3 pin from being used as a JTAG pin so we can use
  // it for transmitting data to the OLED display.
  sjsu::stm32f10x::Pin::ReleaseJTAGPins();

  SetMaximumClockSpeed();

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

    graphics.DrawHorizontalLine(0, sjsu::Ssd1306::kHeight / 2,
                                sjsu::Ssd1306::kWidth);
    graphics.Update();

    graphics.DrawVerticalLine(sjsu::Ssd1306::kWidth / 2, 0,
                              sjsu::Ssd1306::kHeight);
    graphics.Update();

    graphics.DrawRectangle(sjsu::Ssd1306::kWidth / 2 - 10,
                           sjsu::Ssd1306::kHeight / 2 - 10, 20, 20);
    graphics.Update();

    graphics.DrawRectangle(sjsu::Ssd1306::kWidth / 2 - 20,
                           sjsu::Ssd1306::kHeight / 2 - 20, 40, 40);
    graphics.Update();

    graphics.DrawLine(0, 0, sjsu::Ssd1306::kWidth, sjsu::Ssd1306::kHeight);
    graphics.Update();

    graphics.DrawLine(0, sjsu::Ssd1306::kHeight, sjsu::Ssd1306::kWidth, 0);
    graphics.Update();

    graphics.DrawCircle(sjsu::Ssd1306::kWidth / 2, sjsu::Ssd1306::kHeight / 2,
                        20);
    graphics.Update();

    graphics.DrawCircle(sjsu::Ssd1306::kWidth / 2, sjsu::Ssd1306::kHeight / 2,
                        30);
    graphics.Update();

    graphics.DrawCircle(sjsu::Ssd1306::kWidth / 2, sjsu::Ssd1306::kHeight / 2,
                        40);
    graphics.Update();

    graphics.DrawCircle(sjsu::Ssd1306::kWidth / 2, sjsu::Ssd1306::kHeight / 2,
                        60);
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
