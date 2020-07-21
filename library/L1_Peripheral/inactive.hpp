#pragma once

#include <type_traits>
#include <functional>

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
#include "L1_Peripheral/storage.hpp"
#include "L1_Peripheral/system_controller.hpp"
#include "L1_Peripheral/system_timer.hpp"
#include "L1_Peripheral/timer.hpp"
#include "L1_Peripheral/uart.hpp"
#include "utility/units.hpp"

namespace sjsu
{
/// @defgroup inactives_module Inactive Peripherals
/// @brief The GetInactive() returns an implementation of an L1 peripheral that
/// does nothing. We call these peripherals the inactives. They are meant to do
/// nothing, or return trivial values when created and called. This is meant in
/// cases where a particular module requires a particular peripheral to operate,
/// but you want to substitute it out with a dummy implementation.
/// @{

/// Templated struct with a boolean value field of false for any types that are
/// not specialized like the list below. Used for compile time check usage of
/// GetInactive().
template <typename T>
struct UnsupportedInactivePeripheral_t : std::false_type
{
};

/// Default template behaviour for an attempt to create an inactive sjsu L1
/// interface for an interface that is not yet supported. Will generate a custom
/// compile time error message.
template <typename T>
inline T & GetInactive()
{
  static_assert(UnsupportedInactivePeripheral_t<T>::value,
                "There does not exist an inactive variant of this peripheral");
}

/// Template specialization that generates an inactive sjsu::Pin.
template <>
inline sjsu::Pin & GetInactive<sjsu::Pin>()
{
  class InactivePin : public sjsu::Pin
  {
   public:
    InactivePin() : sjsu::Pin(0, 0) {}
    Returns<void> Initialize() const override
    {
      return {};
    }
    Returns<void> SetPinFunction(uint8_t) const override
    {
      return {};
    }
    Returns<void> SetPull(Resistor) const override
    {
      return {};
    }
    Returns<void> SetAsOpenDrain(bool) const override
    {
      return {};
    }
    Returns<void> SetAsAnalogMode(bool) const override
    {
      return {};
    }
  };

  static InactivePin inactive;
  return inactive;
}

/// Template specialization that generates an inactive sjsu::Adc.
template <>
inline sjsu::Adc & GetInactive<sjsu::Adc>()
{
  class InactiveAdc : public sjsu::Adc
  {
   public:
    sjsu::Status Initialize() const override
    {
      return sjsu::Status::kNotImplemented;
    }
    uint32_t Read() const override
    {
      return 0;
    }
    uint8_t GetActiveBits() const override
    {
      return 12;
    }
    units::voltage::microvolt_t ReferenceVoltage() const override
    {
      return 0_V;
    }
  };

  static InactiveAdc inactive;
  return inactive;
}

/// Template specialization that generates an inactive sjsu::Dac.
template <>
inline sjsu::Dac & GetInactive<sjsu::Dac>()
{
  class InactiveDac : public sjsu::Dac
  {
   public:
    Returns<void> Initialize() const override
    {
      return {};
    }
    Returns<void> Write(uint32_t) const override
    {
      return {};
    }
    Returns<void> SetVoltage(float) const override
    {
      return {};
    }
    uint8_t GetActiveBits() const override
    {
      return 12;
    }
  };

  static InactiveDac inactive;
  return inactive;
}

/// Template specialization that generates an inactive sjsu::Gpio.
template <>
inline sjsu::Gpio & GetInactive<sjsu::Gpio>()
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
    void AttachInterrupt(InterruptCallback, Edge) override {}
    void DetachInterrupt() const override {}
  };

  static InactiveGpio inactive;
  return inactive;
}

/// Template specialization that generates an inactive sjsu::I2c.
template <>
inline sjsu::I2c & GetInactive<sjsu::I2c>()
{
  class InactiveI2c : public sjsu::I2c
  {
   public:
    Returns<void> Initialize() const override
    {
      return {};
    }
    Returns<void> Transaction(Transaction_t) const override
    {
      return {};
    }
  };

  static InactiveI2c inactive;
  return inactive;
}

/// Template specialization that generates an inactive sjsu::Pwm.
template <>
inline sjsu::Pwm & GetInactive<sjsu::Pwm>()
{
  class InactivePwm : public sjsu::Pwm
  {
   public:
    Status Initialize(units::frequency::hertz_t) const override
    {
      return Status::kNotImplemented;
    }
    void SetDutyCycle(float) const override {}
    float GetDutyCycle() const override
    {
      return 0.0;
    }
    void SetFrequency(units::frequency::hertz_t) const override {}
  };

  static InactivePwm inactive;
  return inactive;
}

/// Template specialization that generates an inactive sjsu::Spi.
template <>
inline sjsu::Spi & GetInactive<sjsu::Spi>()
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
    void SetClock(units::frequency::hertz_t, bool, bool) const override {}
  };

  static InactiveSpi inactive;
  return inactive;
}

/// Template specialization that generates an inactive sjsu::SystemController.
template <>
inline sjsu::SystemController & GetInactive<sjsu::SystemController>()
{
  class InactiveSystemController : public sjsu::SystemController
  {
   public:
    void Initialize() override {}
    void * GetClockConfiguration() override
    {
      return nullptr;
    }
    units::frequency::hertz_t GetClockRate(ResourceID) const override
    {
      return 0_Hz;
    }
    bool IsPeripheralPoweredUp(ResourceID) const override
    {
      return false;
    }
    void PowerUpPeripheral(ResourceID) const override {}
    void PowerDownPeripheral(ResourceID) const override {}
  };

  static InactiveSystemController inactive;
  return inactive;
}

/// Template specialization that generates an inactive sjsu::SystemController.
template <>
inline sjsu::InterruptController & GetInactive<sjsu::InterruptController>()
{
  class InactiveInterruptController : public sjsu::InterruptController
  {
   public:
    void Initialize(InterruptHandler) override {}
    void Enable(RegistrationInfo_t) override {}
    void Disable(int) override {}
  };

  static InactiveInterruptController inactive;
  return inactive;
}

/// Template specialization that generates an inactive sjsu::SystemTimer.
template <>
inline sjsu::SystemTimer & GetInactive<sjsu::SystemTimer>()
{
  class InactiveSystemTimer : public sjsu::SystemTimer
  {
   public:
    void Initialize() const override {}
    void SetCallback(InterruptCallback) const override {}
    Status StartTimer() const override
    {
      return Status::kNotImplemented;
    }
    int32_t SetTickFrequency(units::frequency::hertz_t) const override
    {
      return 0;
    }
  };

  static InactiveSystemTimer inactive;
  return inactive;
}

/// Template specialization that generates an inactive sjsu::Timer.
template <>
inline sjsu::Timer & GetInactive<sjsu::Timer>()
{
  class InactiveTimer : public sjsu::Timer
  {
   public:
    Status Initialize(units::frequency::hertz_t,
                      InterruptCallback,
                      int32_t) const override
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
    void Start() const override {}
    void Stop() const override {}
    void Reset() const override {}
  };

  static InactiveTimer inactive;
  return inactive;
}

/// Template specialization that generates an inactive sjsu::Uart.
template <>
inline sjsu::Uart & GetInactive<sjsu::Uart>()
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
    void Write(const void *, size_t) const override {}
    size_t Read(void *, size_t) const override
    {
      return 0;
    }
    bool HasData() const override
    {
      return false;
    }
  };

  static InactiveUart inactive;
  return inactive;
}
/// Template specialization that generates an inactive sjsu::Uart.
template <>
inline sjsu::Storage & GetInactive<sjsu::Storage>()
{
  class InactiveStorage : public sjsu::Storage
  {
   public:
    Type GetMemoryType() override
    {
      return Type::kRam;
    }
    Returns<void> Initialize() override
    {
      return {};
    }
    bool IsMediaPresent() override
    {
      return {};
    }
    Returns<void> Enable() override
    {
      return {};
    }
    bool IsReadOnly() override
    {
      return {};
    }
    units::data::byte_t GetCapacity() override
    {
      return 0_B;
    }
    units::data::byte_t GetBlockSize() override
    {
      return 0_B;
    }
    Returns<void> Erase(uint32_t, size_t) override
    {
      return {};
    }
    Returns<void> Write(uint32_t, const void *, size_t) override
    {
      return {};
    }
    Returns<void> Read(uint32_t, void *, size_t) override
    {
      return {};
    }
    Returns<void> Disable() override
    {
      return {};
    }
  };

  static InactiveStorage inactive;
  return inactive;
}
/// @}
}  // namespace sjsu
