#include <project_config.hpp>

#include <cstdint>
#include <iterator>

#include "utility/log.hpp"
#include "utility/time.hpp"

int main()
{
  LOG_INFO("Staring Hello World Application");
  while (true)
  {
    for (uint8_t i = 0; i < 16; i++)
    {
      LOG_INFO("Hello World 0x%X", i);
      sjsu::Delay(500);
    }
  }

  return 0;
}
