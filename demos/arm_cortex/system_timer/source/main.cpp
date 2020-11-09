#include "L1_Peripheral/cortex/system_timer.hpp"
#include "L1_Peripheral/inactive.hpp"
#include "L1_Peripheral/system_controller.hpp"
#include "L1_Peripheral/lpc17xx/system_controller.hpp"
#include "L1_Peripheral/lpc40xx/system_controller.hpp"
#include "L1_Peripheral/msp432p401r/system_controller.hpp"
#include "L1_Peripheral/stm32f10x/system_controller.hpp"
#include "L1_Peripheral/stm32f4xx/system_controller.hpp"
#include "utility/log.hpp"
#include "utility/build_info.hpp"

void DemoSystemIsr()
{
  static int counter = 0;
  sjsu::LogInfo("System Timer Tick #%d ...", counter++);
}

sjsu::SystemController::ResourceID GetSystemTimerID()
{
  if constexpr (sjsu::build::IsPlatform(sjsu::build::Platform::lpc17xx))
  {
    return sjsu::lpc17xx::SystemController::Clocks::kCpu;
  }
  else if constexpr (sjsu::build::IsPlatform(sjsu::build::Platform::lpc40xx))
  {
    return sjsu::lpc40xx::SystemController::Peripherals::kCpu;
  }
  else if constexpr (sjsu::build::IsPlatform(
                         sjsu::build::Platform::msp432p401r))
  {
    return sjsu::msp432p401r::SystemController::Modules::kMasterClock;
  }
  else if constexpr (sjsu::build::IsPlatform(sjsu::build::Platform::stm32f10x))
  {
    return sjsu::stm32f10x::SystemController::Peripherals::kSystemTimer;
  }
  else if constexpr (sjsu::build::IsPlatform(sjsu::build::Platform::stm32f4xx))
  {
    return sjsu::stm32f4xx::SystemController::Peripherals::kSystemTimer;
  }
  else
  {
    // For all other systems just return some
    return sjsu::SystemController::ResourceID::Define<0xFF>();
  }
}

int main()
{
  sjsu::LogInfo("System Timer Application Starting...");

  // The specific clock associated with the system timer is different for
  // platform, thus we need to get the system timer's ID
  sjsu::SystemController::ResourceID system_timer_id = GetSystemTimerID();
  sjsu::cortex::SystemTimer system_timer(system_timer_id);

  system_timer.Initialize();
  system_timer.ConfigureCallback(DemoSystemIsr);
  system_timer.ConfigureTickFrequency(10_Hz);
  system_timer.Enable();

  sjsu::LogInfo("Halting any action.");
  return 0;
}
