#include "L1_Peripheral/lpc40xx/pwm.hpp"
#include "L2_HAL/actuators/servo/servo.hpp"
#include "utility/log.hpp"
#include "utility/time.hpp"

int main()
{
  sjsu::LogInfo("Servo application starting...");

  // Creating PWM on pin 2.0
  sjsu::lpc40xx::Pwm pwm(sjsu::lpc40xx::Pwm::Channel::kPwm0);

  // Create servo class and and give it the PWM.
  sjsu::Servo servo(pwm);

  sjsu::LogInfo("Initalizing Servo");
  servo.Initialize();

  // When all of the bounds of the servo class are set, the servo class will
  // map your degrees to microseconds. With the below example, 0 degrees will
  // represent 500 us, 180 will represent 2500 us, and by linear correlation
  // 90 degrees will representation 1500 us.

  // Set RC servo PWM frequency to default (50 Hz)
  servo.ConfigureFrequency();

  // Set the pulse wideth bounds to be 500 us and 2500 us.
  sjsu::LogInfo("Setting Servo pulse width bounds from 500us to 2500us.");
  servo.ConfigurePulseBounds(500us, 2500us);

  // Set the angle bounds of the servo to be 0 degrees and 180 degrees
  sjsu::LogInfo("Setting Servo angle bounds from 0 deg to 180 deg.");
  servo.ConfigureAngleBounds(0_deg, 180_deg);

  sjsu::LogInfo("Enabling Servo!");
  servo.Enable();

  while (true)
  {
    // Command the servo to go to 180 degrees as fast as the servo will allow.
    servo.SetAngle(180_deg);
    sjsu::Delay(1s);

    // Command the servo to go to 0 degrees as fast as the servo will allow.
    servo.SetAngle(0_deg);
    sjsu::Delay(1s);

    // Take ~ 10 seconds to go from 0 degrees to 180 degrees
    for (units::angle::degree_t servo_angle = 0_deg; servo_angle < 180_deg;
         servo_angle++)
    {
      servo.SetAngle(servo_angle);
      sjsu::Delay(27ms);
    }

    // Take ~ 10 seconds to go from 180 degrees to 0 degrees by directly
    //  manipulating the pulse width.
    for (std::chrono::microseconds pulse_width = 2500us; pulse_width > 500us;
         pulse_width--)
    {
      servo.SetPulseWidthInMicroseconds(pulse_width);
      sjsu::Delay(10ms);
    }
  }
  return 0;
}
