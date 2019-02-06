#include <cstdarg>
#include <cstdint>

#include "L3_Application/oled_terminal.hpp"
#include "utility/log.hpp"
#include "utility/time.hpp"

int main(void)
{
  LOG_INFO("Starting OledPrint Application...");
  OledTerminal oled_terminal;

  oled_terminal.Initialize();

  LOG_INFO("Demonstrating printf capabilities...");
  oled_terminal.printf("Float: %.1f\nInteger: %d", 234.5, 15);

  Delay(2000);
  LOG_INFO("Demonstrating that you can move the print cursor as well...");
  oled_terminal.SetCursor(2, 3);
  oled_terminal.printf("Offset by 2-3");

  Delay(3000);

  LOG_INFO("Clearing screen...");
  oled_terminal.Clear();
  oled_terminal.printf("Printing\nParagraph:\n\n");
  const char kParagraph[] =
      "The important thing is not to stop questioning. "
      "Curiosity has its own reason for existing.\n"
      "-Albert Einstein";

  LOG_INFO("Printing characters one by one...");
  LOG_INFO("Notice how the terminal will scroll down as more text is added...");
  for (size_t i = 0; i < sizeof(kParagraph); i++)
  {
    char buffer[2];
    buffer[0] = kParagraph[i];
    buffer[1] = '\0';
    oled_terminal.printf("%1s", buffer);
  }
  LOG_INFO("Finished printing message!");
  Delay(3000);

  LOG_INFO("Clearing screen...");
  oled_terminal.Clear();
  LOG_INFO("Screen Cleared!");

  LOG_INFO("Printing \"Hello World\" to oled display!");
  oled_terminal.printf("Printing\nHelloWorld:\n\n");
  while (true)
  {
    for (int i = 0; i < 16; i++)
    {
      oled_terminal.printf("Hello World 0x%X\n", i);
      Delay(300);
    }
  }
  return 0;
}
