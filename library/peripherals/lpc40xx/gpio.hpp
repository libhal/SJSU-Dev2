#pragma once

#include <cstdint>
#include <functional>

#include "platforms/targets/lpc17xx/LPC17xx.h"
#include "platforms/targets/lpc40xx/LPC40xx.h"
#include "peripherals/cortex/interrupt.hpp"
#include "peripherals/gpio.hpp"
#include "peripherals/inactive.hpp"
#include "peripherals/interrupt.hpp"
#include "peripherals/lpc17xx/pin.hpp"
#include "peripherals/lpc40xx/pin.hpp"
#include "utility/build_info.hpp"

namespace sjsu
{
namespace lpc40xx
{
/// GPIO implementation for the lpc40xx platform
class Gpio final : public sjsu::Gpio
{
 public:
  /// Specifies number of ports and pins that can be used with gpio interrupts.
  static constexpr uint8_t kPinCount = 32;

  /// The number of ports that generate gpio interrupts.
  static constexpr uint8_t kInterruptPorts = 2;

  /// Lookup table that holds developer gpio interrupt handlers.
  inline static InterruptCallback handlers[kInterruptPorts][kPinCount];

  /// This structure makes the access of gpio interrupt registers more
  /// accessible and efficient
  struct GpioInterruptRegisterMap_t
  {
    //! @cond Doxygen_Suppress
    volatile uint32_t * rising_status  = nullptr;
    volatile uint32_t * falling_status = nullptr;
    volatile uint32_t * clear          = nullptr;
    volatile uint32_t * rising_enable  = nullptr;
    volatile uint32_t * falling_enable = nullptr;
    //! @endcond
  };

  /// Returns an address of a LPC_GPIO_TypeDef pointer based on the platform you
  /// are running on. This function will only contain the code required for the
  /// specific platform and no more.
  ///
  /// @param port_index - which gpio port to get a pointer of.
  /// @return returns a pointer to a LPC_GPIO_TypeDef pointer internal to this
  /// function. The purpose for the double pointer nature is to allow test code
  /// to re-assign the lookup table contents simply by running this function and
  /// assigning the returned result to what ever block of memory you desire.
  static lpc40xx::LPC_GPIO_TypeDef ** GpioRegister(int port_index)
  {
    if constexpr (IsPlatform(sjsu::build::Platform::lpc40xx))
    {
      static lpc40xx::LPC_GPIO_TypeDef * port[] = {
        lpc40xx::LPC_GPIO0, lpc40xx::LPC_GPIO1, lpc40xx::LPC_GPIO2,
        lpc40xx::LPC_GPIO3, lpc40xx::LPC_GPIO4, lpc40xx::LPC_GPIO5,
      };
      return &port[port_index];
    }
    else if constexpr (IsPlatform(sjsu::build::Platform::lpc17xx))
    {
      static lpc40xx::LPC_GPIO_TypeDef * port[] = {
        reinterpret_cast<lpc40xx::LPC_GPIO_TypeDef *>(lpc17xx::LPC_GPIO0),
        reinterpret_cast<lpc40xx::LPC_GPIO_TypeDef *>(lpc17xx::LPC_GPIO1),
        reinterpret_cast<lpc40xx::LPC_GPIO_TypeDef *>(lpc17xx::LPC_GPIO2),
        reinterpret_cast<lpc40xx::LPC_GPIO_TypeDef *>(lpc17xx::LPC_GPIO3),
        reinterpret_cast<lpc40xx::LPC_GPIO_TypeDef *>(lpc17xx::LPC_GPIO4),
      };
      return &port[port_index];
    }
    else  // Used for unit testing
    {
      static lpc40xx::LPC_GPIO_TypeDef * port[6];
      return &port[port_index];
    }
  }

  /// Returns an address of a gpio interrupt register map object based based on
  /// the platform you are running on. This function will only contain the code
  /// required for the specific platform and no more.
  ///
  /// @param interrupt_index - which interrupt port you want to work with. Only
  ///        0 and 1 are valid.
  /// @return returns a pointer to a GpioInterruptRegisterMap_t object internal
  /// to this function.
  static GpioInterruptRegisterMap_t * InterruptRegister(int interrupt_index)
  {
    if constexpr (IsPlatform(sjsu::build::Platform::lpc40xx))
    {
      /// An array that contains all the port specific gpio interrupt registers.
      static GpioInterruptRegisterMap_t interrupt[kInterruptPorts] = {
        {
            .rising_status  = &(lpc40xx::LPC_GPIOINT->IO0IntStatR),
            .falling_status = &(lpc40xx::LPC_GPIOINT->IO0IntStatF),
            .clear          = &(lpc40xx::LPC_GPIOINT->IO0IntClr),
            .rising_enable  = &(lpc40xx::LPC_GPIOINT->IO0IntEnR),
            .falling_enable = &(lpc40xx::LPC_GPIOINT->IO0IntEnF),
        },
        {
            .rising_status  = &(lpc40xx::LPC_GPIOINT->IO2IntStatR),
            .falling_status = &(lpc40xx::LPC_GPIOINT->IO2IntStatF),
            .clear          = &(lpc40xx::LPC_GPIOINT->IO2IntClr),
            .rising_enable  = &(lpc40xx::LPC_GPIOINT->IO2IntEnR),
            .falling_enable = &(lpc40xx::LPC_GPIOINT->IO2IntEnF),
        },
      };
      return &interrupt[interrupt_index];
    }
    else if constexpr (IsPlatform(sjsu::build::Platform::lpc17xx))
    {
      /// An array that contains all the port specific gpio interrupt registers.
      static GpioInterruptRegisterMap_t interrupt[kInterruptPorts] = {
        {
            .rising_status  = &(lpc17xx::LPC_GPIOINT->IO0IntStatR),
            .falling_status = &(lpc17xx::LPC_GPIOINT->IO0IntStatF),
            .clear          = &(lpc17xx::LPC_GPIOINT->IO0IntClr),
            .rising_enable  = &(lpc17xx::LPC_GPIOINT->IO0IntEnR),
            .falling_enable = &(lpc17xx::LPC_GPIOINT->IO0IntEnF),
        },
        {
            .rising_status  = &(lpc17xx::LPC_GPIOINT->IO2IntStatR),
            .falling_status = &(lpc17xx::LPC_GPIOINT->IO2IntStatF),
            .clear          = &(lpc17xx::LPC_GPIOINT->IO2IntClr),
            .rising_enable  = &(lpc17xx::LPC_GPIOINT->IO2IntEnR),
            .falling_enable = &(lpc17xx::LPC_GPIOINT->IO2IntEnF),
        },
      };
      return &interrupt[interrupt_index];
    }
    else  // Used for unit testing
    {
      static GpioInterruptRegisterMap_t interrupt[kInterruptPorts];
      return &interrupt[interrupt_index];
    }
  }

  /// @return a pointer to the interrupt status register based on the current
  /// building platform.
  static volatile uint32_t * InterruptStatus()
  {
    if constexpr (IsPlatform(sjsu::build::Platform::lpc40xx))
    {
      return &lpc40xx::LPC_GPIOINT->IntStatus;
    }
    else if constexpr (IsPlatform(sjsu::build::Platform::lpc17xx))
    {
      return &lpc40xx::LPC_GPIOINT->IntStatus;
    }
    else  // Used for unit testing
    {
      static volatile uint32_t status;
      return &status;
    }
  }

  /// The gpio interrupt handler that calls the attached interrupt callbacks.
  static void InterruptHandler()
  {
    int triggered_port;
    if constexpr (IsPlatform(sjsu::build::Platform::lpc40xx))
    {
      triggered_port = *InterruptStatus() >> 2;
    }
    else
    {
      triggered_port = *InterruptStatus() >> 2;
    }

    auto * interrupt  = InterruptRegister(triggered_port);
    int status        = *interrupt->rising_status | *interrupt->falling_status;
    int triggered_pin = __builtin_ctz(status);

    handlers[triggered_port][triggered_pin]();
    *interrupt->clear |= (1 << triggered_pin);
  }

  /// For port 0-4, pins 0-31 are available. Port 5 only has pins 0-4 available.
  ///
  /// @param port_number - port number
  /// @param pin_number - pin number
  /// @param pin - pointer to an sjsu::Pin, keep as nullptr to ignore this
  Gpio(uint8_t port_number, uint8_t pin_number, sjsu::Pin * pin = nullptr)
      : lpc17xx_pin_(port_number, pin_number),
        lpc40xx_pin_(port_number, pin_number),
        pin_obj_(nullptr),
        gpio_port_(nullptr),
        pin_(pin_number),
        interrupt_index_(0)
  {
    // Assign pin object pointer
    if (pin == nullptr)
    {
      if constexpr (IsPlatform(sjsu::build::Platform::lpc40xx))
      {
        pin_obj_ = &lpc40xx_pin_;
      }
      else if constexpr (IsPlatform(sjsu::build::Platform::lpc17xx))
      {
        pin_obj_ = &lpc17xx_pin_;
      }
      else
      {
        pin_obj_ = &sjsu::GetInactive<sjsu::Pin>();
      }
    }
    else
    {
      pin_obj_ = pin;
    }

    // Assign interrupt_index_ pointer
    if (port_number == 2)
    {
      interrupt_index_ = 1;
    }
    else if (port_number == 0)
    {
      interrupt_index_ = 0;
    }
    else
    {
      interrupt_index_ = kInterruptPorts;
    }

    // Assign gpio port
    gpio_port_ = *GpioRegister(port_number);
  }

  void ModuleInitialize() override
  {
    /// Pin function is zero fall pins on the LPC40xx and LPC17xx.
    constexpr uint8_t kGpioFunction = 0;
    pin_obj_->settings.function     = kGpioFunction;
    pin_obj_->Initialize();
  }

  void SetDirection(Direction direction) override
  {
    if (direction == Direction::kInput)
    {
      gpio_port_->DIR = bit::Clear(gpio_port_->DIR, pin_);
    }
    else
    {
      gpio_port_->DIR = bit::Set(gpio_port_->DIR, pin_);
    }
  }

  void Set(State output = kHigh) override
  {
    if (output == State::kHigh)
    {
      gpio_port_->SET = (1 << pin_);
    }
    else
    {
      gpio_port_->CLR = (1 << pin_);
    }
  }
  void Toggle() override
  {
    gpio_port_->PIN ^= (1 << pin_);
  }
  bool Read() override
  {
    return bit::Read(gpio_port_->PIN, pin_);
  }
  sjsu::Pin & GetPin() override
  {
    return *pin_obj_;
  }

  /// Assign the developer's ISR and sets the selected edge that the gpio
  /// interrupt will be triggered on.
  void AttachInterrupt(InterruptCallback callback, Edge edge) override
  {
    if (!IsInterruptPort())
    {
      throw Exception(std::errc::invalid_argument,
                      "Only port 0 and port 2 can be used as an interrupt.");
    }

    if constexpr (IsPlatform(sjsu::build::Platform::lpc17xx))
    {
      sjsu::InterruptController::GetPlatformController().Enable({
          .interrupt_request_number = lpc17xx::EINT3_IRQn,
          .interrupt_handler        = InterruptHandler,
      });
    }
    else  // For LPC40xx and host tests
    {
      sjsu::InterruptController::GetPlatformController().Enable({
          .interrupt_request_number = lpc40xx::GPIO_IRQn,
          .interrupt_handler        = InterruptHandler,
      });
    }

    handlers[interrupt_index_][pin_] = callback;

    auto * interrupt = LocalInterruptRegister();
    if (edge == Edge::kBoth || edge == Edge::kRising)
    {
      *interrupt->rising_enable = bit::Set(*interrupt->rising_enable, pin_);
    }
    if (edge == Edge::kBoth || edge == Edge::kFalling)
    {
      *interrupt->falling_enable = bit::Set(*interrupt->falling_enable, pin_);
    }
  }

  /// Removes the developer's ISR and clears the selected edge of the gpio
  /// interrupt from being triggered.
  void DetachInterrupt() override
  {
    if (IsInterruptPort())
    {
      handlers[interrupt_index_][pin_] = nullptr;

      auto * interrupt           = LocalInterruptRegister();
      *interrupt->rising_enable  = bit::Clear(*interrupt->rising_enable, pin_);
      *interrupt->falling_enable = bit::Clear(*interrupt->falling_enable, pin_);
    }
  }

  ~Gpio()
  {
    DetachInterrupt();
  }

 private:
  /// Checks if the selected gpio port is valid for external interrupts.
  bool IsInterruptPort() const
  {
    return !(interrupt_index_ == kInterruptPorts);
  }

  GpioInterruptRegisterMap_t * LocalInterruptRegister() const
  {
    return InterruptRegister(interrupt_index_);
  }

  sjsu::lpc17xx::Pin lpc17xx_pin_;
  sjsu::lpc40xx::Pin lpc40xx_pin_;
  sjsu::Pin * pin_obj_;

  lpc40xx::LPC_GPIO_TypeDef * gpio_port_;
  uint8_t pin_;
  uint8_t interrupt_index_;
};

template <int port, int pin_number>
inline Gpio & GetGpio()
{
  static_assert(
      (port <= 4 && pin_number <= 31) || (port == 5 && pin_number < 4),
      "For ports between 0 and 4, the pin number must be between 0 and 31. For "
      "port 5, the pin number must be equal to or below 4");

  static Gpio gpio(port, pin_number);
  return gpio;
}

}  // namespace lpc40xx
}  // namespace sjsu
