#pragma once

#include <cstdio>
#include <iterator>

#include "L3_Application/commandline.hpp"
#include "utility/log.hpp"

namespace command
{
inline Command clear("clear", "Clear console",
                     [](int, const char * const[]) -> int {
                       // Clear entire screen
                       // And move cursor to home
                       printf("\033[2J\033[H");
                       return 0;
                     });

inline Command reboot("reboot", "Reboot the board", DoNothingCommand);
inline Command sensor("sensor",
                      "Display the current information from the sensors",
                      DoNothingCommand);
inline Command telemetry(
    "telemetry",
    "Retrieve information about registered variables and memory in realtime",
    DoNothingCommand);
inline Command datetime("datetime", "Display or set current datetime",
                            DoNothingCommand);
inline Command help("help", "Get information about how to use the SJ2 board",
                    DoNothingCommand);
}  // namespace command

/// Adds the common set of commands: clear, reboot, sensor, memory, telemetry
/// datetime, help and rtos to the passed command_line object
///
/// @tparam CommandListType should not be defined within the template argument
///         list. This type is automatically deduced by the compiler.
/// @param  command_line is the CommandLine object you would like to have the
///         command list of commands added to.
template <template <auto> class CommandLineType,
          template <size_t> class CommandListType, size_t kSize,
          CommandListType<kSize> & list>
inline void AddCommonCommands(CommandLineType<list> & command_line)
{
  command_line.AddCommand(&command::clear);
  command_line.AddCommand(&command::reboot);
  command_line.AddCommand(&command::sensor);
  command_line.AddCommand(&command::telemetry);
  command_line.AddCommand(&command::datetime);
  command_line.AddCommand(&command::help);
}
