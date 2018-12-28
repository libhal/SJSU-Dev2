#include <cstdint>
#include <iterator>

#include "L2_Utilities/log.hpp"
#include "L4_Application/commandline.hpp"
#include "L4_Application/commands/common.hpp"
#include "L4_Application/commands/i2c_command.hpp"

// In order to create a command line, you will need to create a command list
// object. The value between the <> is the maximum number of commands you want
// the commandline to support. If you are not going to use many commands you may
// reduce this number to reduce the size of the program.
//
// NOTE: This list must be of at least size 2 in order to fit the list and quit
// commands that are already added by the command line class.
CommandList_t<32> command_list;
// This is an i2c command object which can be added to a CommandLine object and
// become apart of the list of commands you can run.
I2cCommand i2c_command;

int main(void)
{
  LOG_INFO("Staring Command Line Application");
  CommandLine<command_list> ci;

  LOG_INFO("Adding common SJTwo commands to command line...");
  AddCommonCommands(ci);

  LOG_INFO("Adding i2c command to command line...");
  ci.AddCommand(&i2c_command);

  LOG_INFO("Initializing CommandLine object...");
  ci.Initialize();

  LOG_INFO("Press Enter to Start Command Line!");
  ci.WaitForInput();

  printf("\n");
  LOG_WARNING("User has quit from terminal!");
  return 0;
}
