#include <cstdarg>
#include <cstdint>
#include <string_view>

#include "L2_HAL/boards/sjtwo.hpp"
#include "L3_Application/graphical_terminal.hpp"
#include "utility/log.hpp"
#include "utility/time.hpp"

int main()
{
  sjsu::LogInfo("Starting OledPrint Application...");
  sjsu::Graphics & oled_graphics = sjtwo::Oled();
  sjsu::TerminalCache_t<
      sjsu::Ssd1306::kHeight / sjsu::GraphicalTerminal::kCharacterHeight,
      sjsu::Ssd1306::kWidth / sjsu::GraphicalTerminal::kCharacterWidth>
      cache;
  sjsu::GraphicalTerminal oled_terminal(&oled_graphics, &cache);

  oled_terminal.Initialize();
  oled_terminal.Enable();

  sjsu::LogInfo("Demonstrating printf capabilities...");
  oled_terminal.printf("Float: %.1f\nInteger: %d", 234.5, 15);

  sjsu::Delay(2000ms);
  sjsu::LogInfo("Demonstrating that you can move the print cursor as well...");
  oled_terminal.SetCursor(2, 3);
  oled_terminal.printf("Offset by 2-3");

  sjsu::Delay(3000ms);

  sjsu::LogInfo("Clearing screen...");
  oled_terminal.Clear();
  oled_terminal.printf("Printing\nParagraph:\n\n");
  std::string_view paragraph =
      "The important thing is not to stop questioning. "
      "Curiosity has its own reason for existing.\n"
      "-Albert Einstein";

  sjsu::LogInfo("Printing characters one by one...");
  sjsu::LogInfo(
      "Notice how the terminal will scroll down as more text is added...");

  for (const auto & character : paragraph)
  {
    oled_terminal.printf("%c", character);
    sjsu::Delay(100ms);
  }

  sjsu::LogInfo("Finished printing message!");
  sjsu::Delay(6000ms);

  sjsu::LogInfo("Clearing screen...");
  oled_terminal.Clear();
  sjsu::LogInfo("Screen Cleared!");

  sjsu::LogInfo("Printing \"Count #\" to oled display!");
  oled_terminal.printf("Printing\nHelloWorld:\n\n");

  for (int i = 0; i < 16; i++)
  {
    oled_terminal.printf("Count 0x%X\n", i);
    sjsu::Delay(300ms);
  }

  oled_terminal.Clear();
  oled_terminal.printf("Reset device to run demo again!");

  sjsu::LogInfo("Reset device to run demo again!");
  return 0;
}
