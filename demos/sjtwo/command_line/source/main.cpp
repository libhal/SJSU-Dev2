#include <FreeRTOS.h>
#include <task.h>

#include <cstdint>
#include <iterator>

#include "peripherals/lpc40xx/i2c.hpp"
#include "peripherals/lpc40xx/system_controller.hpp"
#include "utility/console/console.hpp"
#include "utility/console/commands/arm_system_command.hpp"
#include "utility/console/commands/common.hpp"
#include "utility/console/commands/i2c_command.hpp"
#include "utility/console/commands/rtos_command.hpp"
#include "utility/log.hpp"
#include "utility/rtos/freertos/rtos.hpp"

namespace
{
// In order to create a command line, you will need to create a command list
// object. The value between the <> is the maximum number of commands you want
// the commandline to support. If you are not going to use many commands you may
// reduce this number to reduce the size of the program.
//
// NOTE: This list must be of at least size 2 in order to fit the list and quit
// commands that are already added by the command line class.
sjsu::CommandList_t<32> command_list;
// This is an i2c command object which can be added to a CommandLine object and
// become apart of the list of commands you can run.
sjsu::lpc40xx::I2c & i2c2 = sjsu::lpc40xx::GetI2c<2>();
sjsu::I2cCommand i2c_command(i2c2);
sjsu::RtosCommand rtos_command;
sjsu::ArmSystemInfoCommand system_command;
sjsu::CommandLine<command_list> ci;

void TerminalTask([[maybe_unused]] void * ptr)
{
  sjsu::LogInfo("Press Enter to Start Command Line!");

  ci.WaitForInput();

  sjsu::LogWarning("\nUser has quit from terminal!");
  vTaskDelete(nullptr);
}

void BusyTask([[maybe_unused]] void * ptr)
{
  while (true)
  {
    for (int i = 0; i < 200; i++)
    {
      // Busy loop for a while
    }
    vTaskDelay(1);
  }
}

}  // namespace

int main()
{
  sjsu::LogInfo("Staring Command Line Application");

  sjsu::LogInfo("Adding common SJTwo commands to command line...");
  ci.AddCommand(&sjsu::command::clear);
  ci.AddCommand(&sjsu::command::reboot);
  ci.AddCommand(&sjsu::command::sensor);
  ci.AddCommand(&sjsu::command::telemetry);
  ci.AddCommand(&sjsu::command::datetime);
  ci.AddCommand(&sjsu::command::help);

  sjsu::LogInfo("Adding i2c command to command line...");
  i2c_command.Initialize();
  ci.AddCommand(&i2c_command);

  sjsu::LogInfo("Adding system command to command line...");
  ci.AddCommand(&system_command);

  sjsu::LogInfo("Adding sjsu::rtos command to command line...");
  ci.AddCommand(&rtos_command);

  sjsu::LogInfo("Initializing CommandLine object...");
  ci.Initialize();

  xTaskCreate(
      TerminalTask, "Terminal", 512, nullptr, sjsu::rtos::kLow, nullptr);
  xTaskCreate(BusyTask, "Busy", 512, nullptr, sjsu::rtos::kMedium, nullptr);

  vTaskStartScheduler();
  return 0;
}
