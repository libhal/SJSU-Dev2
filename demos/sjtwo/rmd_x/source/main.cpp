#include <algorithm>

#include "L1_Peripheral/lpc40xx/can.hpp"
#include "L2_HAL/actuators/servo/rmd_x.hpp"
#include "utility/log.hpp"

int main(void)
{
  constexpr auto kSpeedLimit = 20_rpm;
  constexpr auto kTimeDelay  = 1000ms;

  sjsu::LogInfo("Starting RMD-X demo in 5s...");
  sjsu::lpc40xx::Can can(sjsu::lpc40xx::Can::Channel::kCan2);
  sjsu::RmdX rmd_x7(can, 0x148);

  sjsu::Delay(5s);

  sjsu::LogInfo("Initializing and Enabling RMD-X driver...");
  rmd_x7.Initialize();
  rmd_x7.Enable();
  sjsu::LogInfo("RMD-X driver Intialized and Enabled!");

  sjsu::LogInfo("Spinning the motor for 3s @ %f RPM...",
                kSpeedLimit.to<double>());
  rmd_x7.SetSpeed(kSpeedLimit);
  sjsu::Delay(3s);

  sjsu::LogInfo("Requesting feedback from motor and printing it...");
  rmd_x7.RequestFeedbackFromMotor().GetFeedback().Print();

  sjsu::LogInfo("Setting Motor Angle Position to 0 deg...");
  rmd_x7.SetAngle(0_deg, kSpeedLimit);
  sjsu::LogInfo("Waiting 4s for this operation to complete...");
  sjsu::Delay(4s);

  sjsu::LogInfo("Rotating motor 360 deg in 45 deg increments...");
  for (units::angle::degree_t angle = 0_deg; angle <= 360_deg; angle += 45_deg)
  {
    sjsu::LogInfo(">>> %d deg angle", angle.to<int>());
    rmd_x7.SetAngle(angle, kSpeedLimit);
    sjsu::Delay(kTimeDelay);
    rmd_x7.RequestFeedbackFromMotor().GetFeedback().Print();
  }

  rmd_x7.RequestFeedbackFromMotor().GetFeedback().Print();

  sjsu::LogInfo("Rotating motor to -360 deg in -45 deg increments...");
  for (units::angle::degree_t angle = 360_deg; angle >= -360_deg;
       angle -= 45_deg)
  {
    sjsu::LogInfo(">>> %d deg angle", angle.to<int>());
    rmd_x7.SetAngle(angle, kSpeedLimit);
    sjsu::Delay(kTimeDelay);
    rmd_x7.RequestFeedbackFromMotor().GetFeedback().Print();
  }

  sjsu::LogInfo("Setting motor angle back to 0 degrees...");
  rmd_x7.SetAngle(0_deg, kSpeedLimit);

  return 0;
}

int main2(void)
{
  constexpr auto kSpeedLimit = 20_rpm;
  constexpr auto kTimeDelay  = 1s;

  sjsu::LogInfo("Starting RMD-X demo in 5s...");
  sjsu::lpc40xx::Can can(sjsu::lpc40xx::Can::Channel::kCan2);
  sjsu::RmdX rmd_x7(can, 0x148);

  rmd_x7.Initialize();
  rmd_x7.Enable();

  // Set motor speed
  rmd_x7.SetSpeed(kSpeedLimit);
  sjsu::Delay(3s);

  // Get status information from motor and print out the details
  rmd_x7.RequestFeedbackFromMotor().GetFeedback().Print();

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

  rmd_x7.SetAngle(0_deg, kSpeedLimit);

  return 0;
}
