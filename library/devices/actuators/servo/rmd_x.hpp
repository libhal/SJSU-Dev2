#pragma once

#include <algorithm>
#include <cinttypes>

#include "peripherals/can.hpp"
#include "module.hpp"
#include "utility/math/bit.hpp"
#include "utility/enum.hpp"
#include "utility/log.hpp"
#include "utility/math/map.hpp"
#include "utility/math/limits.hpp"
#include "utility/time/time.hpp"
#include "utility/time/timeout_timer.hpp"
#include "utility/math/units.hpp"

namespace sjsu
{
/// Settings for an RMD-X series smart servo
struct RmdXSettings_t
{
  /// CANBUS operating baudrate. Default baudrate set by factory is 1MBaud.
  units::frequency::hertz_t can_baudrate = 1_MHz;

  /// Reducer gear ratio. The RMD-X series has the encoder that reads rotation
  /// on the driving motor and not the output shaft of the motor. So as an
  /// example, if the gear ratio for the motor is 6:1, a request to rotate the
  /// motor by 360 rotation only accounts for 1/6 rotation of the output motor
  /// shaft. In order for the commanded rotation of the motor to equal the
  /// rotation output shaft this field must be set correctly.
  float gear_ratio = 8.0f;

  /// Maximum achievable speed for the RMD-X.
  units::angular_velocity::revolutions_per_minute_t maximum_speed = 350_rpm;
};

/// Device driver class for the RMD-X series of smart servos. The RMD-X servos
/// have precision speed and position control and many forms of feedback. The
/// RMD-X smart servo motors handle all of the motor control in its own MCU,
/// freeing up the host MCU to focus on application level tasks.
///
/// Find out more at this website: https://www.myactuator.com/
class RmdX : public sjsu::Module<RmdXSettings_t>
{
 public:
  /// Defines the set of encoder bit resolutions available for the RMD-X line of
  /// smart servos.
  enum class EncoderBitWidth
  {
    k14,
    k16,
  };

  /// Defines the set of all commands that can be issued to a RMD-X motor.
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

  /// Structure containing all of the forms of feedback acquired by an RMD-X
  /// motor
  struct Feedback_t
  {
    /// Core temperature of the motor
    units::temperature::celsius_t temperature;
    /// Current flowing through the motor windings
    units::current::ampere_t current;
    /// Rotational velocity in RMP of the motor
    units::angular_velocity::revolutions_per_minute_t speed;
    /// Motor's supply voltage
    units::voltage::volt_t volts;
    /// Signed 16-bit raw encoder count value of the motor
    int16_t encoder_position;
    /// Error code indicating an over voltage protection event on from the motor
    /// winding output.
    bool over_voltage_protection_tripped;
    /// Error code indicating an over temperature protection event on from the
    /// motor winding output.
    bool over_temperature_protection_tripped;
    /// When true, indicates that an attempt to read the feedback from the motor
    /// failed.
    bool missed_feedback = true;

    /// Print out motor feedback information
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

  /// @param network - canbus network object that uses the CAN BUS to which this
  /// motor is connected
  /// @param device_id - Device ID of the RMD motor. This can be modified by the
  /// switches on the back of the motor. Default ID set by factory on the
  /// switches is 0x140.
  explicit constexpr RmdX(CanNetwork & network,
                          uint16_t device_id = 0x140) noexcept
      : network_(network),
        device_id_(device_id),
        feedback_{},
        node_(nullptr)
  {
  }

  void ModuleInitialize() override
  {
    network_.CanBus().settings.baud_rate = settings.can_baudrate;
    network_.Initialize();
    node_ = network_.CaptureMessage(device_id_);

    network_.CanBus().Send(device_id_,
                           { Value(Commands::kMotorOffCommand),
                             0x00,
                             0x00,
                             0x00,
                             0x00,
                             0x00,
                             0x00,
                             0x00 });
  }

  /// Set the rotational speed of the motor.
  ///
  /// @param rpm - desired RPM to move motor. Can be negative to change rotation
  ///        direction.
  /// @return RmdX& - reference to self to allow method chaining
  RmdX & SetSpeed(units::angular_velocity::revolutions_per_minute_t rpm)
  {
    int32_t command_speed = ConvertRPMToCommandSpeed(rpm);
    network_.CanBus().Send(
        device_id_,
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

    return *this;
  }

  /// Set the angle of the motor's output shaft.
  ///
  /// @param angle - angle to move motor output shaft to.
  /// @param rpm - Can only be a positive value.
  /// @return RmdX& - reference to self to allow method chaining
  RmdX & SetAngle(
      units::angle::degree_t angle,
      units::angular_velocity::revolutions_per_minute_t rpm = 10_rpm)
  {
    int32_t command_speed = ConvertRPMToCommandSpeed(rpm, 1.0f);
    int32_t command_angle = ConvertAngleToCommandAngle(angle);

    LogDebug("Angle_command = %" PRId32, command_angle);

    network_.CanBus().Send(
        device_id_,
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

    return *this;
  }

  /// Request error status and operating information from the motor.
  ///
  /// @param timeout - Amount of time to wait for feedback from the motor.
  /// @return RmdX& - reference to self to allow method chaining
  RmdX & RequestFeedbackFromMotor(std::chrono::nanoseconds timeout = 2ms)
  {
    TimeoutTimer timeout_timer(timeout);

    network_.CanBus().Send(
        device_id_,
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

    PollForMessages(Value(Commands::kReadMotorStatus1AndErrorFlagCommands),
                    timeout_timer.GetTimeLeft());

    network_.CanBus().Send(device_id_,
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

    PollForMessages(Value(Commands::kReadMotorStatus2),
                    timeout_timer.GetTimeLeft());

    return *this;
  }

  /// @return Feedback_t - copy of the motor feedback.
  Feedback_t GetFeedback() const
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
        auto gear_ratio        = CurrentSettings().gear_ratio;
        auto speed_float       = static_cast<float>(speed) / gear_ratio;
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

  void PollForMessages(uint8_t command_id,
                       std::chrono::nanoseconds receive_timeout)
  {
    sjsu::Can::Message_t response_message;
    // Assume that the message has been missed. If the Wait() statement fails
    // this will come true. If ResponseHandler() returns successful (true), then
    // assign false to this variable.
    feedback_.missed_feedback = true;

    // Poll/Wait for the CAN message retrieved from SecureGet() to return with
    // payload contents where the first byte is equal to the expected
    // command_id.
    bool receive_successful =
        Wait(receive_timeout, [this, command_id, &response_message]() {
          response_message = node_->SecureGet();
          if (response_message.payload[0] == command_id)
          {
            return true;
          }
          return false;
        });

    LogDebug("CAN Message Contents:");
    for (int i = 0; i < response_message.length; i++)
    {
      LogDebug("  0x%02X", response_message.payload[i]);
    }

    // Return early and keep feedback_.missed_feedback = true. This way the user
    // knows if the previous feedback has been missed.
    if (!receive_successful)
    {
      return;
    }

    // Invert the response of ResponseHandler(). If it was successful in parsing
    // feedback from the motor
    feedback_.missed_feedback = !ResponseHandler(response_message);
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
        dps * (1.0f / degree_per_second_ratio) * CurrentSettings().gear_ratio;

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
        angle * (1.0f / degree_to_int_ratio) * CurrentSettings().gear_ratio;

    // Return the scaled DPS as an int32_t
    return scaled_angle.to<int32_t>();
  }

  sjsu::CanNetwork & network_;
  const uint16_t device_id_;
  Feedback_t feedback_;
  sjsu::CanNetwork::Node_t * node_;
};
}  // namespace sjsu
