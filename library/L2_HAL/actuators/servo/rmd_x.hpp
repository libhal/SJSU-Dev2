#pragma once

#include <algorithm>
#include <cinttypes>

#include "L1_Peripheral/can.hpp"
#include "module.hpp"
#include "utility/bit.hpp"
#include "utility/enum.hpp"
#include "utility/log.hpp"
#include "utility/map.hpp"
#include "utility/math/limits.hpp"
#include "utility/time.hpp"
#include "utility/units.hpp"

namespace sjsu
{
class RmdX : public sjsu::Module
{
 public:
  enum class EncoderBitWidth
  {
    k14,
    k16,
  };

  enum class Commands : uint8_t
  {
    kReadPIDData                                 = 0x30,
    kWritePIDToRAMCommand                        = 0x31,
    kWritePIDToROMCommand                        = 0x32,
    kReadAccelerationDataCommand                 = 0x33,
    kWriteAccelerationDataToRAMCommand           = 0x34,
    kReadEncoderDataCommand                      = 0x90,
    kWriteEncoderOffsetCommand                   = 0x91,
    kWriteCurrentPositionToROMAsMotorZeroCommand = 0x19,
    kReadMultiTurnsAngleCommand                  = 0x92,
    kReadSingleCircleAngleCommand                = 0x94,
    kReadMotorStatus1AndErrorFlagCommands        = 0x9A,
    kClearMotorErrorFlagCommand                  = 0x9B,
    kReadMotorStatus2                            = 0x9C,
    kReadMotorStatus3                            = 0x9D,
    kMotorOffCommand                             = 0x80,
    kMotorStopCommand                            = 0x81,
    kMotorRunningCommand                         = 0x88,
    kTorqueClosedLoopCommand                     = 0xA1,
    kSpeedClosedLoopCommand                      = 0xA2,
    kPositionClosedLoopCommand1                  = 0xA3,
    kPositionClosedLoopCommand2                  = 0xA4,
    kPositionClosedLoopCommand3                  = 0xA5,
    kPositionClosedLoopCommand4                  = 0xA6,
  };

  struct Parameters_t
  {
    /// CANBUS operating baudrate. Default baudrate set by factory is 1MBaud.
    units::frequency::hertz_t can_baudrate;

    /// Reducer gear ratio. The RMD-X series has the encoder placed on the input
    /// motor and not the output motor, meaning that 360 rotation on the encoder
    /// is only 1/6 rotation of the output motor shaft if the motor's reducer is
    /// 6:1, thus this field is important to get proper angle mapping when using
    /// position control.
    float gear_ratio;

    /// Maximum achievable speed for the RMD-X
    units::angular_velocity::revolutions_per_minute_t maximum_speed;
  };

  struct Feedback_t
  {
    units::temperature::celsius_t temperature;
    units::current::ampere_t current;
    units::angular_velocity::revolutions_per_minute_t speed;
    units::voltage::volt_t volts;
    int16_t encoder_position;
    bool over_voltage_protection_tripped;
    bool over_temperature_protection_tripped;
    bool missed_feedback = true;

    void Print()
    {
      LogInfo("Error Flags:");
      LogInfo("  Missed Feedback: %d", missed_feedback);
      LogInfo("  Over Voltage Protection Tripped: %d",
              over_voltage_protection_tripped);
      LogInfo("  Over Temperature Protection Tripped: %d",
              over_temperature_protection_tripped);
      LogInfo("Sensors:");
      LogInfo("  Encoder Position: %" PRId16, encoder_position);
      LogInfo("  Voltage:     %f V", volts.to<double>());
      LogInfo("  Speed:       %f RPM", speed.to<double>());
      LogInfo("  Current:     %f A", current.to<double>());
      LogInfo("  Temperature: %f C", temperature.to<double>());
    }
  };

  static constexpr Parameters_t kDefaultParameters = {
    .can_baudrate  = 1_MHz,
    .gear_ratio    = 6.0f,
    .maximum_speed = 120_rpm,
  };

  ///
  ///
  /// @param canbus - canbus peripheral to use
  /// @param device_id - Device ID of the RMD motor. This can be modified by the
  /// switches on the back of the motor. Default ID set by factory on the
  /// switches is 0x140.
  /// @param params - RMD-X series parameters that must be set correctly for
  /// proper operation. If the gear ratio is incorrect, then the servo angles
  /// will be off.
  RmdX(sjsu::Can & canbus,
       uint16_t device_id  = 0x140,
       Parameters_t params = kDefaultParameters)
      : canbus_(canbus), device_id_(device_id), params_(params), feedback_{}
  {
  }

  void ModuleInitialize() override
  {
    if (canbus_.RequiresConfiguration())
    {
      canbus_.Initialize();
      canbus_.ConfigureBaudRate(params_.can_baudrate);
      canbus_.Enable();
    }
  }

  /// Disabling the motor will assert the MOTOR OFF command which will clear all
  /// status flags and command directives and stopping control of the motor. In
  /// a sense the motor will go limp and will not drive the output shaft.
  void ModuleEnable(bool enable = true) override
  {
    if (enable)
    {
      // Nothing needed to enable the motor
    }
    else
    {
      canbus_.Send(device_id_,
                   { Value(Commands::kMotorOffCommand),
                     0x00,
                     0x00,
                     0x00,
                     0x00,
                     0x00,
                     0x00,
                     0x00 });
    }
  }

  RmdX & SetSpeed(units::angular_velocity::revolutions_per_minute_t rpm)
  {
    int32_t command_speed = ConvertRPMToCommandSpeed(rpm);
    canbus_.Send(device_id_,
                 {
                     Value(Commands::kSpeedClosedLoopCommand),
                     0x00,
                     0x00,
                     0x00,
                     static_cast<uint8_t>((command_speed >> 0) & 0xFF),
                     static_cast<uint8_t>((command_speed >> 8) & 0xFF),
                     static_cast<uint8_t>((command_speed >> 16) & 0xFF),
                     static_cast<uint8_t>((command_speed >> 24) & 0xFF),
                 });

    PollForMessages();

    return *this;
  }

  RmdX & SetAngle(
      units::angle::degree_t angle,
      units::angular_velocity::revolutions_per_minute_t rpm = 10_rpm)
  {
    int32_t command_speed = ConvertRPMToCommandSpeed(rpm, 1.0f);
    int32_t command_angle = ConvertAngleToCommandAngle(angle);

    LogDebug("Angle_command = %" PRId32, command_angle);

    canbus_.Send(device_id_,
                 {
                     Value(Commands::kPositionClosedLoopCommand2),
                     0x00,
                     static_cast<uint8_t>((command_speed >> 0) & 0xFF),
                     static_cast<uint8_t>((command_speed >> 8) & 0xFF),
                     static_cast<uint8_t>((command_angle >> 0) & 0xFF),
                     static_cast<uint8_t>((command_angle >> 8) & 0xFF),
                     static_cast<uint8_t>((command_angle >> 16) & 0xFF),
                     static_cast<uint8_t>((command_angle >> 24) & 0xFF),
                 });

    PollForMessages();

    return *this;
  }

  RmdX & RequestFeedbackFromMotor()
  {
    canbus_.Send(device_id_,
                 {
                     Value(Commands::kReadMotorStatus1AndErrorFlagCommands),
                     0x00,
                     0x00,
                     0x00,
                     0x00,
                     0x00,
                     0x00,
                     0x00,
                 });

    PollForMessages();

    canbus_.Send(device_id_,
                 {
                     Value(Commands::kReadMotorStatus2),
                     0x00,
                     0x00,
                     0x00,
                     0x00,
                     0x00,
                     0x00,
                     0x00,
                 });

    PollForMessages();

    return *this;
  }

  auto GetFeedback() const
  {
    return feedback_;
  }

 private:
  bool ResponseHandler(const Can::Message_t & message)
  {
    if (!(message.length == 8 && message.id == device_id_))
    {
      return false;
    }

    switch (Commands(message.payload[0]))
    {
      case Commands::kReadMotorStatus2:
      case Commands::kTorqueClosedLoopCommand:
      case Commands::kSpeedClosedLoopCommand:
      case Commands::kPositionClosedLoopCommand1:
      case Commands::kPositionClosedLoopCommand2:
      case Commands::kPositionClosedLoopCommand3:
      case Commands::kPositionClosedLoopCommand4:
      {
        constexpr float kMax      = 33.0f;
        constexpr int16_t kMaxInt = BitLimits<16, int16_t>::Max();

        using units::angular_velocity::degrees_per_second_t;
        using units::current::ampere_t;
        using units::temperature::celsius_t;

        auto & payload = message.payload;

        auto temperature = static_cast<int8_t>(payload[1]);
        auto current     = static_cast<int16_t>((payload[3] << 8) | payload[2]);
        auto speed       = static_cast<int16_t>((payload[5] << 8) | payload[4]);
        auto encoder     = static_cast<int16_t>((payload[7] << 8) | payload[6]);

        auto temperature_float = static_cast<float>(temperature);
        auto speed_float       = static_cast<float>(speed) / params_.gear_ratio;
        auto current_float     = Map(current, -kMaxInt, kMaxInt, -kMax, kMax);

        feedback_.encoder_position = encoder;
        feedback_.temperature      = celsius_t{ temperature_float };
        feedback_.current          = ampere_t{ current_float };
        feedback_.speed            = degrees_per_second_t{ speed_float };

        break;
      }
      case Commands::kReadMotorStatus1AndErrorFlagCommands:
      {
        using units::temperature::celsius_t;
        using units::voltage::volt_t;

        auto & payload = message.payload;

        uint8_t error    = payload[7];
        auto temperature = static_cast<int8_t>(payload[1]);
        auto volts = static_cast<uint16_t>((payload[4] << 8) | payload[3]);

        auto temperature_float = static_cast<float>(temperature);
        auto volts_float       = static_cast<float>(volts);

        feedback_.temperature = celsius_t{ temperature_float };
        feedback_.volts       = volt_t{ volts_float * 0.1f };

        feedback_.over_voltage_protection_tripped     = bit::Read(error, 0);
        feedback_.over_temperature_protection_tripped = bit::Read(error, 3);

        break;
      }
      default: return false;
    }

    return true;
  }

  void PollForMessages(std::chrono::nanoseconds receive_timeout = 1ms)
  {
    sjsu::Can::Message_t feedback;

    bool receive_successful = Wait(receive_timeout, [this, &feedback]() {
      if (canbus_.HasData())
      {
        feedback = canbus_.Receive();
        return true;
      }
      return false;
    });

    if (!receive_successful)
    {
      feedback_.missed_feedback = false;
    }

    LogDebug("CAN Message Contents:");
    for (int i = 0; i < feedback.length; i++)
    {
      LogDebug("  0x%02X", feedback.payload[i]);
    }

    // Invert the response of ResponseHandler(). If it was successful in parsing
    // feedback from the motor
    feedback_.missed_feedback = !ResponseHandler(feedback);
  }

  int32_t ConvertRPMToCommandSpeed(
      units::angular_velocity::revolutions_per_minute_t rpm,
      float degree_per_second_ratio = 0.01f)
  {
    // Convert RPM to DPS (degrees per second)
    units::angular_velocity::degrees_per_second_t dps = rpm;

    // Since the RPM is determined by the input motor speed, in order to make
    // the output spin at that speed, the DSP must be scaled by the gear_ratio
    // factor, otherwise for a gear ratio of 2:1, the output speed of the motor
    // shaft will be half as fast. The RMD motors have a conversion of 0.01
    // DPS/LSB meaning that the integer value of the scaled DPS must be x100 of
    // the original value.
    auto scaled_dps =
        dps * (1.0f / degree_per_second_ratio) * params_.gear_ratio;

    // Return the scaled DPS as an int32_t
    return scaled_dps.to<int32_t>();
  }

  int32_t ConvertAngleToCommandAngle(units::angle::degree_t angle,
                                     float degree_to_int_ratio = 0.01f)
  {
    // Since the RPM is determined by the input motor speed, in order to make
    // the output spin at that speed, the DSP must be scaled by the gear_ratio
    // factor, otherwise for a gear ratio of 2:1, the output speed of the motor
    // shaft will be half as fast. The RMD motors have a conversion of 0.01
    // Degree/LSB meaning that the integer value of the scaled DPS must be x100
    // of the original value.
    auto scaled_angle =
        angle * (1.0f / degree_to_int_ratio) * params_.gear_ratio;

    // Return the scaled DPS as an int32_t
    return scaled_angle.to<int32_t>();
  }

  sjsu::Can & canbus_;
  const uint16_t device_id_;
  const Parameters_t params_;
  Feedback_t feedback_;
};  // namespace sjsu
}  // namespace sjsu
