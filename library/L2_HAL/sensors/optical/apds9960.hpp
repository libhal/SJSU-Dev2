// Low level Gesture Driver for setting up APDS-9960 device
// Usage:
//  Apds9960 sensor;
//  sensor.Initialize();
//  sensor.EnableGesture();
//  Apds9960::Gesture value = sensor.GetGesture();
#pragma once

#include <cstdint>
#include <cstdlib>

#include "L1_Peripheral/i2c.hpp"
#include "L2_HAL/device_memory_map.hpp"
#include "utility/bit.hpp"
#include "utility/enum.hpp"
#include "utility/log.hpp"
#include "utility/time.hpp"

namespace sjsu
{
class Apds9960
{
 public:
  template <device::Endian endianess, WriteFnt write, ReadFnt read>
  SJ2_PACKED(struct)
  MemoryMap_t  // APDS-9960
  {
    template <typename Int>
    using Register = device::Register_t<Int, endianess, write, read>;

    template <typename Int, size_t kLength>
    using Array = device::Array_t<Int, kLength, write, read>;

    device::Reserved_t<0x80> ram;
    Register<uint8_t> enable;
    Register<uint8_t> adc_integration_time;
    device::Reserved_t<0x01> reserved0;
    Register<uint8_t> wait_time_nongesture;
    Register<uint16_t> als_interrupt_low_threshold;
    Register<uint16_t> als_interrupt_high_threshold;
    device::Reserved_t<0x01> reserved1;
    Register<uint8_t> proximity_interrupt_low_threshold;
    device::Reserved_t<0x01> reserved2;
    Register<uint8_t> proximity_interrupt_high_threshold;
    Register<uint8_t> interrupt_persistence_filters_nongesture;
    Register<uint8_t> configuration_1;
    Register<uint8_t> proximity_pulse_count_and_length;
    Register<uint8_t> gain_control;
    Register<uint8_t> configuration_2;
    device::Reserved_t<0x01> reserved3;
    Register<uint8_t> device_id;
    Register<uint8_t> device_status;
    Register<uint16_t> clear_channel_data;
    Register<uint16_t> red_channel_data;
    Register<uint16_t> green_channel_data;
    Register<uint16_t> blue_channel_data;
    Register<uint8_t> proximity_data;
    Register<uint16_t> proximity_offset_up_right_down_left_photodiodes;
    Register<uint8_t> configuration_3;
    Register<uint8_t> gesture_proximity_enter_threshold;
    Register<uint8_t> gesture_exit_threshold;
    Register<uint16_t> gesture_configuration_1_2;
    Register<uint16_t> gesture_up_down_offset;
    Register<uint8_t> gesture_pulse_count_and_length;
    Register<uint8_t> gesture_left_offset;
    device::Reserved_t<0x01> reserved4;
    Register<uint8_t> gesture_right_offset;
    Register<uint16_t> gesture_configuration_3_4;
    device::Reserved_t<0x02> reserved5;
    Register<uint8_t> gesture_fifo_level;
    Register<uint8_t> gesture_status;
    device::Reserved_t<0x34> reserved6;
    Register<uint8_t> force_interrupt;
    Register<uint8_t> proximity_interrupt_clear;
    Register<uint8_t> als_clear_channel_interrupt_clear;
    Register<uint8_t> all_nongesture_interrupts_clear;
    device::Reserved_t<0x14> reserved7;
    union {
      Array<uint8_t, 4> gesture_fifo_data;
      struct
      {
        Register<uint8_t> up_value;
        Register<uint8_t> down_value;
        Register<uint8_t> left_value;
        Register<uint8_t> right_value;
      };
    };
  };
  enum Direction : char
  {
    kNone  = 'x',
    kUp    = 'u',
    kDown  = 'd',
    kLeft  = 'l',
    kRight = 'r',
    kFar   = 'f',
    kNear  = 'n'
  };
  enum Gesture : char
  {
    kSwipeUP    = 'U',
    kSwipeDOWN  = 'D',
    kSwipeLEFT  = 'L',
    kSwipeRIGHT = 'R',
    kNEAR       = 'N',
    kFAR        = 'F',
    kError      = 'E'
  };
  enum Mode : uint8_t
  {
    kPOWERON = 0,
    kALSENABLE,
    kPROXIMITYDETECT,
    kWAIT,
    kALSINTERRUPT,
    kPROXIMITYINTERRUPT,
    kGESTURE
  };

  static constexpr uint8_t kApds9960Address = 0x39;
  // Avago-APDS-9960 : 32 x 4 byte FIFO = 128-bytes of data to store (page 33)
  static constexpr uint8_t kMaxFifoSize = 128;
  // User may change kMaxDataSize to adjust how many Direction Data points
  // to be collected, then DecodeGestureData(uint8_t) can be overloaded to
  // take care of new patterns of Direction points for different gestures
  static constexpr uint8_t kMaxDataSize = 8;
  static constexpr uint8_t kMaxFIFOLevel = 33;

  // Defualt values for Gesture sensor initialization
  enum DefaultValues : uint8_t
  {
    kNoMode                 = 0x00,
    kAtime                  = 0xDB,
    kWtime                  = 0xF6,
    kPilt                   = 0x00,
    kPiht                   = 0x32,
    kPers                   = 0x11,
    kConfig1                = 0x60,
    kPpulse                 = 0x89,
    kControl                = 0x05,
    kConfig2                = 0x01,
    kPoffsetUpRightDownLeft = 0x00,
    kConfig3                = 0x00,
    kGPEnTh                 = 0x14,
    kGExTh                  = 0x64,
    kGconfig1               = 0x43,
    kGconfig2               = 0x66,
    kGoffsetUpDown          = 0x00,
    kGoffsetLeftRight       = 0x00,
    kGPulse                 = 0xC9,
    kGconfig3               = 0x00,
    kGconfig4               = 0x02,
    kAilt                   = 0xFF,
    kAiht                   = 0x00
  };

  explicit Apds9960(const I2c & i2c)
      : i2c_(i2c),
        up_sensitivity_(-75),
        down_sensitivity_(75),
        left_sensitivity_(-75),
        right_sensitivity_(50),
        far_sensitivity_(750),
        near_sensitivity_(1000),
        far_count_(0),
        near_count_(0),
        index_(0)
  {
    for (int i = 0; i < kMaxDataSize; i++)
    {
      points_[i] = Direction::kNone;
    }
    for (int i = 0; i < kMaxFifoSize; i++)
    {
      gfifo_data_[i] = 0;
    }
  }
  virtual bool FindDevice()
  {
    bool device_found = false;
    uint8_t device_id = gesture_.memory.device_id;
    if (device_id == 0xAB)
    {
      device_found = true;
    }
    return device_found;
  }
  virtual void Initialize()
  {
    i2c_.Initialize();
    constexpr uint16_t kGestureConfig1And2    = (kGconfig2 << 8) | (kGconfig1);
    constexpr uint16_t kGestureConfig3And4    = (kGconfig4 << 8) | (kGconfig3);
    constexpr uint16_t kAlsInterruptThreshold = (kAilt << 8) | (kAilt);
    if (FindDevice())
    {
      gesture_.memory.enable                       = kNoMode;
      gesture_.memory.adc_integration_time         = kAtime;
      gesture_.memory.wait_time_nongesture         = kWtime;
      gesture_.memory.als_interrupt_low_threshold  = kAlsInterruptThreshold;
      gesture_.memory.als_interrupt_high_threshold = kAiht;
      gesture_.memory.proximity_interrupt_low_threshold        = kPilt;
      gesture_.memory.proximity_interrupt_high_threshold       = kPiht;
      gesture_.memory.interrupt_persistence_filters_nongesture = kPers;
      gesture_.memory.configuration_1                          = kConfig1;
      gesture_.memory.proximity_pulse_count_and_length         = kPpulse;
      gesture_.memory.gain_control                             = kControl;
      gesture_.memory.configuration_2                          = kConfig2;
      gesture_.memory.proximity_offset_up_right_down_left_photodiodes =
          kPoffsetUpRightDownLeft;
      gesture_.memory.configuration_3                   = kConfig3;
      gesture_.memory.gesture_proximity_enter_threshold = kGPEnTh;
      gesture_.memory.gesture_exit_threshold            = kGExTh;
      gesture_.memory.gesture_configuration_1_2         = kGestureConfig1And2;
      gesture_.memory.gesture_up_down_offset            = kGoffsetUpDown;
      gesture_.memory.gesture_left_offset               = kGoffsetLeftRight;
      gesture_.memory.gesture_right_offset              = kGoffsetLeftRight;
      gesture_.memory.gesture_pulse_count_and_length    = kGPulse;
      gesture_.memory.gesture_configuration_3_4         = kGestureConfig3And4;
    }
    else
    {
      SJ2_ASSERT_WARNING(false, "Error Initializing Gesture Sensor");
    }
  }
  /// SetMode(uint8_t mode, bool enable)
  /// @param mode -> enables/disables individual bits of the ENABLE register
  /// Modes:
  /// 7: reserved | 6: Gesture | 5: Proximity Interrupt | 4: ALS Interrupt
  /// 3: Wait   | 2: Proximity | 1: ALS  | 0: Power ON
  /// @param enable -> true: sets bit; false: resets bit
  /// @return -> true if successful; false otherwise
  virtual bool SetMode(Mode mode, bool enable)
  {
    bool result   = true;
    int reg_value = 0;
    reg_value     = gesture_.memory.enable;

    if (mode > 7)
    {
      result = false;
    }
    if (enable)
    {
      reg_value |= (1 << mode);
    }
    else
    {
      reg_value &= ~(1 << mode);
    }
    gesture_.memory.enable = static_cast<uint8_t>(reg_value);
    return result;
  }
  virtual void EnableGesture()
  {
    SetMode(Mode::kPOWERON, false);
    constexpr uint8_t kShortWaitTime                                   = 0xFF;
    constexpr uint8_t kK16UsPulseLengthAnd10PulseCount                 = 0x89;
    constexpr uint8_t kClearProximityLedInterruptChannelAndLedBoost150 = 0xD0;

    gesture_.memory.wait_time_nongesture = kShortWaitTime;
    gesture_.memory.proximity_pulse_count_and_length =
        kK16UsPulseLengthAnd10PulseCount;
    gesture_.memory.configuration_2 =
        kClearProximityLedInterruptChannelAndLedBoost150;

    SetMode(Mode::kPOWERON, true);
    SetMode(Mode::kWAIT, true);
    SetMode(Mode::kPROXIMITYDETECT, true);
    SetMode(Mode::kGESTURE, true);
  }
  virtual bool DisableGesture()
  {
    bool result = true;
    if (!SetMode(Mode::kGESTURE, 0))
    {
      result = false;
    }
    return result;
  }
  virtual bool ReadGestureMode()
  {
    uint16_t value = gesture_.memory.gesture_configuration_3_4;
    return (value & 0x0100);
  }
  virtual bool CheckIfGestureOccured()
  {
    return (gesture_.memory.gesture_status & 0b1);
  }
  virtual void ReadGestureFIFO(uint8_t * data, uint8_t fifolevel)
  {
    int amount_of_data_to_read = (4 * fifolevel);
    intptr_t address =
        reinterpret_cast<intptr_t>(&gesture_.memory.gesture_fifo_data[0]);
    gesture_.Read(address, amount_of_data_to_read, data);
  }
  virtual uint8_t GetGestureFIFOLevel()
  {
    constexpr uint8_t kOverflowFlagPosition = 1;
    uint8_t value                           = 0;
    uint8_t gesture_status = gesture_.memory.gesture_status;
    if (bit::Read(gesture_status,
                    kOverflowFlagPosition))  // if overflow, clear FIFO data
    {
      LOG_INFO("Overflow. No Gesture Detected");
      value = kMaxFIFOLevel;  // Overflow happens at FIFO level 33
    }
    else
    {
      value = gesture_.memory.gesture_fifo_level;
    }
    return value;
  }
  virtual void ProcessGestureData(uint8_t level)
  {
    constexpr uint8_t kMaxPointsIndex = 7;
    // Read Gesture FIFOs (U/D/L/R)
    // Store raw gesture data in a 4x32 uint8_t array
    for (int i = 0; i < (4 * level); i += 4)
    {
      if (index_ > kMaxPointsIndex)
      {
        index_ = 0;
      }
      // Check which photodiode pair sensed more IR light
      if (abs((gfifo_data_[i] - gfifo_data_[i + 1])) >
          abs((gfifo_data_[i + 2] - gfifo_data_[i + 3])))
      {
        // If a photodiode has a smaller value,
        // then that photodiode was covered and movement detected
        if ((gfifo_data_[i] - gfifo_data_[i + 1]) < up_sensitivity_)  // Up
        {
          points_[index_] = Direction::kUp;
          index_++;
        }
        else if ((gfifo_data_[i] - gfifo_data_[i + 1]) >
                 down_sensitivity_)  // Down
        {
          points_[index_] = Direction::kDown;
          index_++;
        }
      }
      else if (abs((gfifo_data_[i] - gfifo_data_[i + 1])) <
          abs((gfifo_data_[i + 2] - gfifo_data_[i + 3])))
      {
        // If a photodiode has a smaller value,
        // then that photodiode was covered and movement detected
        if ((gfifo_data_[i + 2] - gfifo_data_[i + 3]) <
            left_sensitivity_)  // Left
        {
          points_[index_] = Direction::kLeft;
          index_++;
        }
        else if ((gfifo_data_[i + 2] - gfifo_data_[i + 3]) >
                 right_sensitivity_)  // Right
        {
          points_[index_] = Direction::kRight;
          index_++;
        }
      }
      // Check the sum of all photodiodes
      if ((gfifo_data_[i] + gfifo_data_[i + 1] + gfifo_data_[i + 2] +
           gfifo_data_[i + 3]) > near_sensitivity_)  // Near
      {
        near_count_++;
      }
      else if ((gfifo_data_[i] + gfifo_data_[i + 1] + gfifo_data_[i + 2] +
                gfifo_data_[i + 3]) < far_sensitivity_)  // Far
      {
        far_count_++;
      }
    }
  }
  virtual Gesture DecodeGestureData(uint8_t level)
  {
    Gesture result = kError;
    constexpr uint8_t kFarNearSensitivity = 15;
    constexpr uint8_t kNearFarCountMinimum = 5;

    if (level == kMaxFIFOLevel)
    {
        result = kError;
    }
    else if (level > kFarNearSensitivity)
    {
      if (far_count_ > kNearFarCountMinimum)
      {
        result = kFAR;
      }
      else if (near_count_ > kNearFarCountMinimum)
      {
        result = kNEAR;
      }
    }
    sjsu::Delay(30ms);
    for (int i = 0; i < kMaxDataSize - 1; i++)
    {
      // Check initial point, then check next point
      // to figure out the direction of the swipe
      switch (points_[i])
      {
        case Direction::kUp:
          if (points_[i + 1] == Direction::kDown)
          {
            result = kSwipeDOWN;
          }
          break;
        case Direction::kDown:
          if (points_[i + 1] == Direction::kUp)
          {
            result = kSwipeUP;
          }
          break;
        case Direction::kLeft:
          if (points_[i + 1] == Direction::kRight)
          {
            result = kSwipeRIGHT;
          }
          break;
        case Direction::kRight:
          if (points_[i + 1] == Direction::kLeft)
          {
            result = kSwipeLEFT;
          }
          break;
          // If no valid point, then check near/far counts
        default: break;
      }
    }
    return result;
  }
  virtual Gesture GetGesture()
  {
    Gesture result = kError;
    if (CheckIfGestureOccured())
    {
      // check if gesture ended
      if (ReadGestureMode() == 0)
      {
        uint8_t level = 0;
        level         = (GetGestureFIFOLevel());
        ReadGestureFIFO(gfifo_data_, level);

        // reset processing variables
        index_      = 0;
        near_count_ = 0;
        far_count_  = 0;
        for (int i = 0; i < 8; i++)
        {
          points_[i] = Direction::kNone;
        }

        ProcessGestureData(level);

        result = DecodeGestureData(level);
      }
    }
    return result;
  }

 private:
  const I2c & i2c_;
  I2cDevice<0x39, device::Endian::kLittle, MemoryMap_t> gesture_ =
      I2cDevice<0x39, device::Endian::kLittle, MemoryMap_t>(&i2c_);

  int8_t up_sensitivity_;
  int8_t down_sensitivity_;
  int8_t left_sensitivity_;
  int8_t right_sensitivity_;
  int16_t far_sensitivity_;
  int16_t near_sensitivity_;

  uint8_t gfifo_data_[kMaxFifoSize] = {};
  Direction points_[kMaxDataSize]   = {};
  uint8_t far_count_                = 0;
  uint8_t near_count_               = 0;
  uint8_t index_                    = 0;
};
}  // namespace sjsu
