// Low level Gesture Driver for setting up APDS-9960 device
// Usage:
//  Apds9960 sensor;
//  sensor.Initialize();
//  sensor.EnableGesture();
//  Apds9960::Gesture value = sensor.GetGesture();
#pragma once

#include <cstdint>
#include <cstdlib>

#include "L1_Drivers/i2c.hpp"
#include "L2_HAL/device_memory_map.hpp"
#include "third_party/etl/vector.h"
#include "utility/enum.hpp"
#include "utility/log.hpp"

class Apds9960Interface
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

  virtual bool FindDevice()                                       = 0;
  virtual bool Initialize()                                       = 0;
  virtual bool SetMode(Mode mode, bool enable)                    = 0;
  virtual void EnableGesture()                                    = 0;
  virtual bool DisableGesture()                                   = 0;
  virtual bool ReadGestureMode()                                  = 0;
  virtual bool CheckIfGestureOccured()                            = 0;
  virtual uint8_t GetGestureFIFOLevel()                           = 0;
  virtual void ReadGestureFIFO(uint8_t * data, uint8_t fifolevel) = 0;
  virtual void ProcessGestureData(uint8_t level)                  = 0;
  virtual Gesture DecodeGestureData()                             = 0;
  virtual Gesture GetGesture()                                    = 0;
};

class Apds9960 : public Apds9960Interface
{
 public:
  static constexpr uint8_t kApds9960Address = 0x39;
  // Avago-APDS-9960 : 33 x 4 byte FIFO = 132 bytes of data to store at
  // max/overflow (page 33)
  static constexpr uint8_t kMaxFifoSize = 132;
  // User may change kMaxDataSize to adjust how many Direction Data points
  // to be collected, then DecodeGestureData(uint8_t) can be overloaded to
  // take care of new patterns of Direction points for different gestures
  static constexpr uint8_t kMaxDataSize = 8;
  // Sensitivity variables may be adjusted if there are different environements
  // in light, glass or screen on top of sensor to hide it, adjust the height
  // of hands passing above, etc.
  static constexpr int8_t kUpSensitivity    = 0;
  static constexpr int8_t kDownSensitivity  = 0;
  static constexpr int8_t kLeftSensitivity  = 0;
  static constexpr int8_t kRightSensitivity = 0;
  static constexpr int16_t kFarSensitivity  = 750;
  static constexpr int16_t kNearSensitivity = 1000;

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
    kGconfig1               = 0x40,
    kGconfig2               = 0x66,
    kGoffsetUpDown          = 0x00,
    kGoffsetLeftRight       = 0x00,
    kGPulse                 = 0xC5,
    kGconfig3               = 0x00,
    kGconfig4               = 0x02,
    kAilt                   = 0xFF,
    kAiht                   = 0x00
  };
  // TODO(#1): Move I2c out of static to make this driver more platform angostic
  inline static I2c i2c;
  inline static I2cDevice<&i2c, 0x39, device::Endian::kLittle,
                          Apds9960Interface::MemoryMap_t>
      gesture;
  inline static etl::vector<Direction, kMaxDataSize> points;

  constexpr Apds9960() : gfifo_data_{ 0 }, far_count_(0), near_count_(0)
  {
    i2c.Initialize();
  }
  bool FindDevice() override
  {
    bool device_found = false;
    uint8_t device_id = gesture.memory.device_id;

    LOG_DEBUG("ID: %02X", device_id);

    if (device_id == 0xAB)
    {
      device_found = true;
    }
    return device_found;
  }
  bool Initialize() override
  {
    bool success = false;
    i2c.Initialize();
    constexpr uint16_t kGestureConfig1And2 = ((kGconfig2 << 8) | (kGconfig1));
    constexpr uint16_t kGestureConfig3And4 = ((kGconfig4 << 8) | (kGconfig3));
    constexpr uint16_t kAlsInterruptThreshold = ((kAilt << 8) | (kAilt));
    constexpr uint16_t kGestureUpDownOffset =
        ((kGoffsetUpDown << 8) | (kGoffsetUpDown));
    if (FindDevice())
    {
      gesture.memory.enable                            = kNoMode;
      gesture.memory.adc_integration_time              = kAtime;
      gesture.memory.wait_time_nongesture              = kWtime;
      gesture.memory.als_interrupt_low_threshold       = kAlsInterruptThreshold;
      gesture.memory.als_interrupt_high_threshold      = kAiht;
      gesture.memory.proximity_interrupt_low_threshold = kPilt;
      gesture.memory.proximity_interrupt_high_threshold       = kPiht;
      gesture.memory.interrupt_persistence_filters_nongesture = kPers;
      gesture.memory.configuration_1                          = kConfig1;
      gesture.memory.proximity_pulse_count_and_length         = kPpulse;
      gesture.memory.gain_control                             = kControl;
      gesture.memory.configuration_2                          = kConfig2;
      gesture.memory.proximity_offset_up_right_down_left_photodiodes =
          kPoffsetUpRightDownLeft;
      gesture.memory.configuration_3                   = kConfig3;
      gesture.memory.gesture_proximity_enter_threshold = kGPEnTh;
      gesture.memory.gesture_exit_threshold            = kGExTh;
      gesture.memory.gesture_configuration_1_2         = kGestureConfig1And2;
      gesture.memory.gesture_up_down_offset            = kGestureUpDownOffset;
      gesture.memory.gesture_left_offset               = kGoffsetLeftRight;
      gesture.memory.gesture_right_offset              = kGoffsetLeftRight;
      gesture.memory.gesture_pulse_count_and_length    = kGPulse;
      gesture.memory.gesture_configuration_3_4         = kGestureConfig3And4;
      success                                          = true;
    }
    else
    {
      SJ2_ASSERT_WARNING(false, "Error Initializing Gesture Sensor");
    }
    return success;
  }
  /// SetMode(uint8_t mode, bool enable)
  /// @param mode -> enables/disables individual bits of the ENABLE register
  /// Modes:
  /// 7: reserved | 6: Gesture | 5: Proximity Interrupt | 4: ALS Interrupt
  /// 3: Wait   | 2: Proximity | 1: ALS  | 0: Power ON
  /// @param enable -> true: sets bit; false: resets bit
  /// @return -> true if successful; false otherwise
  bool SetMode(Mode mode, bool enable) override
  {
    bool result   = true;
    int reg_value = 0;
    reg_value     = gesture.memory.enable;

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
    gesture.memory.enable = static_cast<uint8_t>(reg_value);
    return result;
  }
  void EnableGesture() override
  {
    SetMode(Mode::kPOWERON, false);
    constexpr uint8_t kShortWaitTime                                   = 0xFF;
    constexpr uint8_t kK16UsPulseLengthAnd10PulseCount                 = 0x89;
    constexpr uint8_t kClearProximityLedInterruptChannelAndLedBoost150 = 0xD0;

    gesture.memory.wait_time_nongesture = kShortWaitTime;
    gesture.memory.proximity_pulse_count_and_length =
        kK16UsPulseLengthAnd10PulseCount;
    gesture.memory.configuration_2 =
        kClearProximityLedInterruptChannelAndLedBoost150;

    SetMode(Mode::kPOWERON, true);
    SetMode(Mode::kWAIT, true);
    SetMode(Mode::kPROXIMITYDETECT, true);
    SetMode(Mode::kGESTURE, true);
  }
  bool DisableGesture() override
  {
    bool result = true;
    if (!SetMode(Mode::kGESTURE, 0))
    {
      result = false;
    }
    return result;
  }
  bool ReadGestureMode() override
  {
    uint16_t value = gesture.memory.gesture_configuration_3_4;
    return (value & 0x0100);
  }
  bool CheckIfGestureOccured() override
  {
    return (gesture.memory.gesture_status & 0b1);
  }
  void ReadGestureFIFO(uint8_t * data, uint8_t fifolevel) override
  {
    int amount_of_data_to_read = (4 * fifolevel);
    intptr_t address =
        reinterpret_cast<intptr_t>(&gesture.memory.gesture_fifo_data[0]);
    gesture.Read(address, amount_of_data_to_read, data);
  }
  uint8_t GetGestureFIFOLevel() override
  {
    uint8_t overflow_value = 0;
    uint8_t value          = 0;
    overflow_value         = gesture.memory.gesture_status;
    if (overflow_value & 0b10)  // if overflow, clear FIFO data
    {
      LOG_INFO("overflow");
      value = 32;
    }
    else
    {
      value = gesture.memory.gesture_fifo_level;
    }
    return value;
  }
  void ProcessGestureData(uint8_t level) override
  {
    // Read Gesture FIFOs (U/D/L/R)
    // Store raw gesture data in a 4x32 uint8_t array
    for (int i = 0; i < (4 * level); i += 4)
    {
      if (!points.full())
      {
        // Check which photodiode pair sensed more IR light
        if (abs((gfifo_data_[i] - gfifo_data_[i + 1])) >
            abs((gfifo_data_[i + 2] - gfifo_data_[i + 3])))
        {
          // If a photodiode has a smaller value,
          // then that photodiode was covered and movement detected
          if ((gfifo_data_[i] - gfifo_data_[i + 1]) < kUpSensitivity)  // Up
          {
            points.push_back(Direction::kUp);
          }
          else if ((gfifo_data_[i] - gfifo_data_[i + 1]) >
                   kDownSensitivity)  // Down
          {
            points.push_back(Direction::kDown);
          }
        }
        else
        {
          // If a photodiode has a smaller value,
          // then that photodiode was covered and movement detected
          if ((gfifo_data_[i + 2] - gfifo_data_[i + 3]) <
              kLeftSensitivity)  // Left
          {
            points.push_back(Direction::kLeft);
          }
          else if ((gfifo_data_[i + 2] - gfifo_data_[i + 3]) >
                   kRightSensitivity)  // Right
          {
            points.push_back(Direction::kRight);
          }
        }
      }
      // Check the sum of all photodiodes
      if ((gfifo_data_[i] + gfifo_data_[i + 1] + gfifo_data_[i + 2] +
           gfifo_data_[i + 3]) > kNearSensitivity)  // Near
      {
        near_count_++;
      }
      else if ((gfifo_data_[i] + gfifo_data_[i + 1] + gfifo_data_[i + 2] +
                gfifo_data_[i + 3]) < kFarSensitivity)  // Far
      {
        far_count_++;
      }
    }
  }
  Gesture DecodeGestureData() override
  {
    Gesture result = kError;
    bool found     = false;

    if (far_count_ > 5)
    {
      result = kFAR;
      found  = true;
    }
    else if (near_count_ > 5)
    {
      result = kNEAR;
      found  = true;
    }

    for (uint8_t i = 0; i < points.size() - 1 && !found; i++)
    {
      // Check initial point, then check next point
      // to figure out the direction of the swipe
      switch (points[i])
      {
        case Direction::kUp:
          if (points[i + 1] == Direction::kDown)
          {
            result = kSwipeDOWN;
            found  = true;
          }
          break;
        case Direction::kDown:
          if (points[i + 1] == Direction::kUp)
          {
            result = kSwipeUP;
            found  = true;
          }
          break;
        case Direction::kLeft:
          if (points[i + 1] == Direction::kRight)
          {
            result = kSwipeRIGHT;
            found  = true;
          }
          break;
        case Direction::kRight:
          if (points[i + 1] == Direction::kLeft)
          {
            result = kSwipeLEFT;
            found  = true;
          }
          break;
          // If no valid point, then check near/far counts
        default: break;
      }
    }
    return result;
  }
  Gesture GetGesture() override
  {
    Gesture result            = kError;
    uint8_t gesture_interrupt = 0;

    if (CheckIfGestureOccured())
    {
      // check if gesture ended
      if ((ReadGestureMode() == 0))
      {
        uint8_t level = 0;
        level         = (GetGestureFIFOLevel());
        ReadGestureFIFO(gfifo_data_, level);

        // reset processing variables
        near_count_ = 0;
        far_count_  = 0;
        points.clear();

        gesture_interrupt = (gesture.memory.device_status & 0x04);

        if (gesture_interrupt == 0)
        {
          ProcessGestureData(level);
          result = DecodeGestureData();
        }
      }
    }
    return result;
  }

 private:
  uint8_t gfifo_data_[kMaxFifoSize];
  uint8_t far_count_;
  uint8_t near_count_;
};
