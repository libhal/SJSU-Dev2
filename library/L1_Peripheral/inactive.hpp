#pragma once

#include <type_traits>

#include "config.hpp"
#include "L1_Peripheral/adc.hpp"
#include "L1_Peripheral/can.hpp"
#include "L1_Peripheral/dac.hpp"
#include "L1_Peripheral/gpio.hpp"
#include "L1_Peripheral/i2c.hpp"
#include "L1_Peripheral/inactive.hpp"
#include "L1_Peripheral/pin.hpp"
#include "L1_Peripheral/pwm.hpp"
#include "L1_Peripheral/spi.hpp"
#include "L1_Peripheral/system_controller.hpp"
#include "L1_Peripheral/system_timer.hpp"
#include "L1_Peripheral/timer.hpp"
#include "L1_Peripheral/uart.hpp"

namespace sjsu
{
template <typename T>
struct UnsupportedInactivePeripheral_t : std::false_type
{
};

template <typename T>
const T & GetInactive()
{
  static_assert(UnsupportedInactivePeripheral_t<T>::value,
                "There does not exist an inactive variant of this peripheral");
}

// template <typename T>
// const T & GetInactive() = delete;

template <>
const sjsu::Pin & GetInactive<sjsu::Pin>()
{
  class InactivePin : public sjsu::Pin
  {
   public:
    InactivePin() : sjsu::Pin(0, 0) {}
    void SetPinFunction(uint8_t) const override {}
    void SetPull(Resistor) const override {}
    void SetAsOpenDrain(bool) const override {}
    void SetAsAnalogMode(bool) const override {}
  };

  static InactivePin inactive;
  return inactive;
}

template <>
const sjsu::Adc & GetInactive<sjsu::Adc>()
{
  class InactiveAdc : public sjsu::Adc
  {
   public:
    sjsu::Status Initialize() const override
    {
      return sjsu::Status::kNotImplemented;
    }
    void Conversion() const override {}
    uint32_t Read() const override
    {
      return 0;
    }
    bool HasConversionFinished() const override
    {
      return true;
    }
    uint8_t GetActiveBits() const override
    {
      return 12;
    }
  };

  static InactiveAdc inactive;
  return inactive;
}

template <>
const sjsu::Dac & GetInactive<sjsu::Dac>()
{
  class InactiveDac : public sjsu::Dac
  {
   public:
    Status Initialize() const override
    {
      return Status::kNotImplemented;
    }
    void Write(uint32_t) const override {}
    void SetVoltage(float) const override {}
    uint8_t GetActiveBits() const override
    {
      return 12;
    }
  };

  static InactiveDac inactive;
  return inactive;
}

template <>
const sjsu::Gpio & GetInactive<sjsu::Gpio>()
{
  class InactiveGpio : public sjsu::Gpio
  {
   public:
    void SetDirection(Direction) const override {}
    void Set(State) const override {}
    void Toggle() const override {}
    bool Read() const override
    {
      return false;
    }
    const sjsu::Pin & GetPin() const override
    {
      return GetInactive<sjsu::Pin>();
    }
    void AttachInterrupt(IsrPointer, Edge) const override {}
    void DetachInterrupt() const override {}
  };

  static InactiveGpio inactive;
  return inactive;
}

template <>
const sjsu::I2c & GetInactive<sjsu::I2c>()
{
  class InactiveI2c : public sjsu::I2c
  {
   public:
    Status Initialize() const override
    {
      return Status::kNotImplemented;
    }
    Status Transaction(Transaction_t) const override
    {
      return Status::kNotImplemented;
    }
  };

  static InactiveI2c inactive;
  return inactive;
}

template <>
const sjsu::Pwm & GetInactive<sjsu::Pwm>()
{
  class InactivePwm : public sjsu::Pwm
  {
   public:
    Status Initialize(uint32_t) const override
    {
      return Status::kNotImplemented;
    }
    void SetDutyCycle(float) const override {}
    float GetDutyCycle() const override
    {
      return 0.0;
    }
    void SetFrequency(uint32_t) const override {}
  };

  static InactivePwm inactive;
  return inactive;
}

template <>
const sjsu::Spi & GetInactive<sjsu::Spi>()
{
  class InactiveSpi : public sjsu::Spi
  {
   public:
    Status Initialize() const override
    {
      return Status::kNotImplemented;
    }
    uint16_t Transfer(uint16_t) const override
    {
      return 0xFF;
    }
    void SetDataSize(DataSize) const override {}
    void SetClock(uint32_t, bool, bool) const override {}
  };

  static InactiveSpi inactive;
  return inactive;
}

template <>
const sjsu::SystemController & GetInactive<sjsu::SystemController>()
{
  class InactiveSystemController : public sjsu::SystemController
  {
   public:
    uint32_t SetSystemClockFrequency(uint8_t) const override
    {
      return 0;
    }
    uint32_t GetPeripheralClockDivider(const PeripheralID &) const override
    {
      return 0;
    }
    bool IsPeripheralPoweredUp(const PeripheralID &) const override
    {
      return false;
    }
    void SetPeripheralClockDivider(const PeripheralID &, uint8_t) const override
    {
    }
    void PowerUpPeripheral(const PeripheralID &) const override {}
    void PowerDownPeripheral(const PeripheralID &) const override {}
    // NOTE: We only this method for SystemTimer to work.
    uint32_t GetSystemFrequency() const override
    {
      return config::kSystemClockRate;
    }
  };

  static InactiveSystemController inactive;
  return inactive;
}

template <>
const sjsu::SystemTimer & GetInactive<sjsu::SystemTimer>()
{
  class InactiveSystemTimer : public sjsu::SystemTimer
  {
   public:
    void SetInterrupt(IsrPointer) const override {}
    Status StartTimer() const override
    {
      return Status::kNotImplemented;
    }
    uint32_t SetTickFrequency(uint32_t) const override
    {
      return 0;
    }
  };

  static InactiveSystemTimer inactive;
  return inactive;
}

template <>
const sjsu::Timer & GetInactive<sjsu::Timer>()
{
  class InactiveTimer : public sjsu::Timer
  {
   public:
    Status Initialize(uint32_t, IsrPointer, int32_t) const override
    {
      return Status::kNotImplemented;
    }
    void SetMatchBehavior(uint32_t, MatchAction, uint8_t) const override {}
    uint32_t GetCount() const override
    {
      return 0;
    }
    uint8_t GetAvailableMatchRegisters() const override
    {
      return 3;
    }
  };

  static InactiveTimer inactive;
  return inactive;
}

template <>
const sjsu::Uart & GetInactive<sjsu::Uart>()
{
  class InactiveUart : public sjsu::Uart
  {
   public:
    Status Initialize(uint32_t) const override
    {
      return Status::kNotImplemented;
    }
    bool SetBaudRate(uint32_t) const override
    {
      return true;
    }
    void Write(const uint8_t *, size_t) const override {}
    Status Read(uint8_t *, size_t, uint32_t) const override
    {
      return Status::kNotImplemented;
    }
    bool HasData() const override
    {
      return false;
    }
  };

  static InactiveUart inactive;
  return inactive;
}
}  // namespace sjsu
