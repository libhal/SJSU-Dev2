#pragma once

#include <cstdio>
#include <iterator>

#include "utility/console/console.hpp"
#include "utility/log.hpp"

namespace sjsu
{
namespace command
{
inline Command clear("clear",
                     "Clear console",
                     [](int, const char * const[]) -> int {
                       // Clear entire screen
                       // And move cursor to home
                       printf("\033[2J\033[H");
                       return 0;
                     });

inline Command reboot("reboot", "Reboot the board");
inline Command sensor("sensor",
                      "Display the current information from the sensors");
inline Command telemetry(
    "telemetry",
    "Retrieve information about registered variables and memory in realtime");
inline Command datetime("datetime", "Display or set current datetime");
inline Command help("help", "Get information about how to use the SJ2 board");
}  // namespace command
}  // namespace sjsu
