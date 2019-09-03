#pragma once

#include <stdio.h>

namespace sjsu
{
class LinuxLed
{
 public:
  constexpr LinuxLed(const char * led_path): led_path_(led_path) {}

  void On()
  {
    constexpr const char * kOnString = "1";
    WriteToFile(kOnString);
  }
  void Off()
  {
    constexpr const char * kOffString = "0";
    WriteToFile(kOffString);
  }

 private:
  void WriteToFile(const char* str)
  {
    char buffer[256];
    // sprintf(buffer, "%s-brightness", led_path_);
    sprintf(buffer, "/sys/class/leds/%s/brightness", led_path_);
    LOG_DEBUG("buffer = %s", buffer);
    FILE * fd = fopen(buffer, "w");
    fputs(str, fd);
    fclose(fd);
  }
  const char * led_path_;
};
} // namespace sjsu
