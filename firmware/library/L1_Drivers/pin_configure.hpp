// PinConfigure abstracts the process of changing the mode and attributes of a
// pin on the LPC40xx series of chips.
//   Usage:
//      PinConfigure P0_0(0, 0);
//      P0_0.SetAsActiveLow();
//      P0_0.SetPinMode(PinConfigureInterface::PinMode::kPullUp);
#pragma once

#include <cstdio>

#include "L0_LowLevel/LPC40xx.h"
#include "L2_Utilities/macros.hpp"

class PinConfigureInterface
{
   public:
    // Source: "UM10562 LPC408x/407x User manual" table 83 page 132
    enum PinMode : uint8_t
    {
        kInactive = 0,
        kPullDown,
        kPullUp,
        kRepeater
    };
    virtual void SetPinFunction(uint8_t function)                       = 0;
    virtual void SetPinMode(PinConfigureInterface::PinMode mode)        = 0;
    virtual void EnableHysteresis(bool enable_hysteresis = true)        = 0;
    virtual void SetAsActiveLow(bool set_as_active_low = true)          = 0;
    virtual void SetAsAnalogMode(bool set_as_analog = true)             = 0;
    virtual void EnableDigitalFilter(bool enable_digital_filter = true) = 0;
    virtual void EnableFastMode(bool enable_fast_mode = true)           = 0;
    virtual void EnableI2cHighSpeedMode(
        bool enable_i2c_high_speed_mode = true) = 0;
    virtual void EnableI2cHighCurrentDrive(
        bool enable_i2c_high_current_drive = true)             = 0;
    virtual void SetAsOpenDrain(bool set_as_open_drain = true) = 0;
    virtual void EnableDac(bool enable_dac = true)             = 0;
};

class PinConfigure : public PinConfigureInterface
{
   public:
    // Source: "UM10562 LPC408x/407x User manual" table 83 page 132
    enum PinBitMap : uint8_t
    {
        kFunction            = 0,
        kMode                = 3,
        kHysteresis          = 5,
        kInputInvert         = 6,
        kAnalogDigitalMode   = 7,
        kDigitalFilter       = 8,
        kI2cHighSpeed        = 8,
        kSlew                = 9,
        kI2cHighCurrentDrive = 9,
        kOpenDrain           = 10,
        kDacEnable           = 16
    };

    SJ2_PACKED(struct) PinMap_t
    {
        using pin_config_register_t = uint32_t;
        pin_config_register_t _register[6][32];
    };

    static PinMap_t * pin_map;
    // Compile time validating PinConfigure factory.
    // Will test the port and pin variables to make sure they are within bounds
    // of the pin_config_register.
    template <unsigned port, unsigned pin>
    static constexpr PinConfigure CreatePinConfigure()
    {
        static_assert(port <= 5, "Port must be between 0 and 5");
        static_assert(pin <= 31, "Pin must be between 0 and 31");
        static_assert(port < 5 || (port == 5 && pin <= 4),
                      "For port 5, the pin number must be equal to or below 4");
        return PinConfigure(port, pin);
    }
    // Pin P5.4 is not featured on the LPC4078, so manipulating its bits has
    // no effect.
    static constexpr PinConfigure CreateInactivePin()
    {
        return PinConfigure(5, 4);
    }

    constexpr PinConfigure(const uint8_t _port, const uint8_t _pin)
        : port(_port), pin(_pin)
    {
    }
    void SetPinFunction(uint8_t function) final override
    {
        pin_map->_register[port][pin] =
            BitPlace(pin_map->_register[port][pin], PinBitMap::kFunction,
                     function & 0b111, 3);
    }
    void SetPinMode(PinConfigureInterface::PinMode mode) final override
    {
        pin_map->_register[port][pin] = BitPlace(
            pin_map->_register[port][pin], PinBitMap::kMode, mode & 0b11, 2);
    }
    void EnableHysteresis(bool enable_hysteresis = true) final override
    {
        pin_map->_register[port][pin] =
            BitPlace(pin_map->_register[port][pin], PinBitMap::kHysteresis,
                     enable_hysteresis, 1);
    }
    void SetAsActiveLow(bool set_as_active_low = true) final override
    {
        pin_map->_register[port][pin] =
            BitPlace(pin_map->_register[port][pin], PinBitMap::kInputInvert,
                     set_as_active_low, 1);
    }
    // Set bit to 0 to enable analog mode
    void SetAsAnalogMode(bool set_as_analog = true) final override
    {
        pin_map->_register[port][pin] =
            BitPlace(pin_map->_register[port][pin],
                     PinBitMap::kAnalogDigitalMode, !set_as_analog, 1);
    }
    // Enable by setting bit to 0 to enable digital filter.
    void EnableDigitalFilter(bool enable_digital_filter = true) final override
    {
        pin_map->_register[port][pin] =
            BitPlace(pin_map->_register[port][pin], PinBitMap::kDigitalFilter,
                     !enable_digital_filter, 1);
    }
    void EnableFastMode(bool enable_fast_mode = true) final override
    {
        pin_map->_register[port][pin] =
            BitPlace(pin_map->_register[port][pin], PinBitMap::kSlew,
                     enable_fast_mode, 1);
    }
    // Enable by setting bit to 0 for i2c high speed mode
    void EnableI2cHighSpeedMode(
        bool enable_i2c_high_speed_mode = true) final override
    {
        pin_map->_register[port][pin] =
            BitPlace(pin_map->_register[port][pin], PinBitMap::kI2cHighSpeed,
                     !enable_i2c_high_speed_mode, 1);
    }
    void EnableI2cHighCurrentDrive(
        bool enable_i2c_high_current_drive = true) final override
    {
        pin_map->_register[port][pin] = BitPlace(
            pin_map->_register[port][pin], PinBitMap::kI2cHighCurrentDrive,
            enable_i2c_high_current_drive, 1);
    }
    void SetAsOpenDrain(bool set_as_open_drain = true) final override
    {
        pin_map->_register[port][pin] =
            BitPlace(pin_map->_register[port][pin], PinBitMap::kOpenDrain,
                     set_as_open_drain, 1);
    }
    void EnableDac(bool enable_dac = true) final override
    {
        pin_map->_register[port][pin] =
            BitPlace(pin_map->_register[port][pin], PinBitMap::kDacEnable,
                     enable_dac, 1);
    }
    inline uint32_t BitPlace(uint32_t target, uint32_t position, uint32_t value,
                             uint32_t value_width)
    {
        // Generate mask with all 1s
        uint32_t mask = 0xFFFFFFFF >> (32 - value_width);
        target &= ~(mask << position);
        target |= (value & mask) << position;
        return target;
    }
    uint8_t getPort()
    {
        return port;
    }
    uint8_t setPin()
    {
        return pin;
    }

   protected:
    const uint8_t port;
    const uint8_t pin;
};
