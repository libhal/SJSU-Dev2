#include <algorithm>

#include "peripherals/lpc40xx/can.hpp"
#include "devices/actuators/servo/rmd_x.hpp"
#include "utility/log.hpp"

int main(void)
{
  constexpr auto kSpeedLimit = 20_rpm;
  constexpr auto kTimeDelay  = 1s;

  sjsu::LogInfo("Starting RMD-X demo in 5s...");
  sjsu::lpc40xx::Can & can = sjsu::lpc40xx::GetCan<2>();
  sjsu::StaticMemoryResource<1024> memory_resource;
  sjsu::CanNetwork can_network(can, &memory_resource);
  sjsu::RmdX rmd_x7(can_network, 0x148);

  sjsu::Delay(5s);

  sjsu::LogInfo("RMD-X initializing...");
  rmd_x7.Initialize();

  sjsu::LogInfo("Setting motor speed to +%f RPM...", kSpeedLimit.to<double>());
  rmd_x7.SetSpeed(kSpeedLimit);
  sjsu::Delay(3s);

  // Get status information from motor and print out the details
  sjsu::LogInfo("Requesting feedback from motor and printing it to STDOUT...");
  rmd_x7.RequestFeedbackFromMotor().GetFeedback().Print();

  sjsu::LogInfo("Setting angles positions of motor...");
  rmd_x7.SetAngle(0_deg, kSpeedLimit);
  sjsu::Delay(4s);

  rmd_x7.SetAngle(45_deg, kSpeedLimit);
  sjsu::Delay(1s);

  rmd_x7.SetAngle(90_deg, kSpeedLimit);
  sjsu::Delay(1s);

  rmd_x7.SetAngle(180_deg, kSpeedLimit);
  sjsu::Delay(2s);

  rmd_x7.SetAngle(360_deg, kSpeedLimit);
  sjsu::Delay(4s);

  rmd_x7.SetAngle(360_deg, kSpeedLimit);
  sjsu::Delay(4s);

  sjsu::LogInfo("Setting motor angle back to 0 deg...");
  rmd_x7.SetAngle(0_deg, kSpeedLimit);

  return 0;
}
