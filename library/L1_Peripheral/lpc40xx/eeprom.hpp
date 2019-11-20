#pragma once

#include <chrono>
#include "L1_Peripheral/eeprom.hpp"

#include "L0_Platform/lpc40xx/LPC40xx.h"
#include "L1_Peripheral/lpc40xx/system_controller.hpp"

#include "utility/bit.hpp"

namespace sjsu
{
namespace lpc40xx
{
/// Implementation of the EEPROM interface for the LPC40xx family of
/// microcontrollers.
class Eeprom final : public sjsu::Eeprom
{
 public:
  /// Pointer to the LPC EEPROM peripheral in memory
  inline static LPC_EEPROM_TypeDef * eeprom_register = LPC_EEPROM;

  /// This driver only supports reading and writing to the EEPROM in 32-bit
  /// mode, which means addresses must be multiples of 4 to avoid hard faults.
  /// To enforce this, all addresses used to read/write to the EEPROM have to
  /// be masked.
  static constexpr bit::Mask kAddressMask = bit::CreateMaskFromRange(0, 1);

  /// Masks for the program status bits and read/write status bits
  struct Status {  // NOLINT
    /// Mask to get value of programming status bit
    static constexpr bit::Mask kProgramStatusMask =
        bit::CreateMaskFromRange(28);
    /// Mask to get value of read/write status bit
    static constexpr bit::Mask kReadWriteStatusMask =
        bit::CreateMaskFromRange(26);
  };

  /// EEPROM Command codes for reading from, writing to, and programming the
  /// device
  enum command_codes {
    kRead32Bits    = 0b010,
    kWrite32Bits   = 0b101,
    kEraseProgram  = 0b110
  };

  /// Max timeout for program/write operations in milliseconds
  static constexpr std::chrono::milliseconds kMaxTimeout = 20ms;

  /// Getting actual SystemController Object
  static constexpr sjsu::lpc40xx::SystemController kLpc40xxSystemController =
      sjsu::lpc40xx::SystemController();

  /// Constructor has an optional SystemController parameter which can be
  /// specified for testing purposes. Otherwise, the actual SystemController
  /// object is used (kLpc40xxSystemController)
  explicit constexpr Eeprom(const sjsu::SystemController & system_controller =
                             kLpc40xxSystemController)
      : system_controller_(system_controller)
  {
  }

  /// Initializing the EEPROM requires setting the wait state register, setting
  /// the clock divider register, and ensuring that the device is powered on.
  void Initialize() const override
  {
    const float kSystemClock    = static_cast<float>
                                    (system_controller_.GetSystemFrequency());
    // The EEPROM runs at 375 kHz
    constexpr float kEepromClk  = 375'000;
    constexpr float kNanosecond = 1E-9f;

    // The EEPROM is turned on by default, but in case it was somehow
    // turned off, we turn it on by writing 0 to the PWRDWN register
    eeprom_register->PWRDWN = 0;

    // Initialize EEPROM wait state register with number of wait states
    // for each of its internal phases
    // Phase 3 (15 ns)
    eeprom_register->WSTATE |=
        static_cast<uint8_t>((15 * kNanosecond * kSystemClock) + 1);
    // Phase 2 (55 ns)
    eeprom_register->WSTATE |=
        (static_cast<uint8_t>((55 * kNanosecond * kSystemClock) + 1)) << 8;
    // Phase 1 (35 ns)
    eeprom_register->WSTATE |=
        (static_cast<uint8_t>((35 * kNanosecond * kSystemClock) + 1)) << 16;

    // Initialize EEPROM clock
    eeprom_register->CLKDIV = static_cast<uint8_t>(kSystemClock / kEepromClk);
  }

  /// This function will write however much data to the EEPROM that the user
  /// specifies. The maximum size of the EEPROM is 64 KB.
  ///
  /// @param wdata        - array of data to be written to EEPROM
  /// @param full_address - address where data will start being written to
  /// @param count        - number of bytes that have to be transferred
  void Write(const uint8_t * wdata, uint32_t full_address,
             size_t count) const override
  {
    constexpr bit::Mask kLower6Bits = bit::CreateMaskFromRange(0, 5);
    constexpr bit::Mask kUpper6Bits = bit::CreateMaskFromRange(6, 11);

    full_address = bit::Clear(full_address, kAddressMask);

    // Because the EEPROM uses 32-bit communication, write_data will be casted
    // into a uint32_t *
    const uint32_t * write_data = reinterpret_cast<const uint32_t *>(wdata);

    // The first 6 bits in the address (MSB) dictate which page is being written
    // to in the EEPROM, and the last 6 bits (LSB) dictate offset in the page
    uint8_t page_count  = bit::Read(full_address, kUpper6Bits);
    uint8_t page_offset = bit::Read(full_address, kLower6Bits);

    uint16_t address;

    for (size_t i = 0; i*4 < count; i++)
    {
      // Page offset is incremented by 4 because we're writing 32 bits
      constexpr uint8_t kOffsetInterval = 4;

      address = static_cast<uint16_t>((page_count << 6) + page_offset);

      eeprom_register->ADDR  = address;
      eeprom_register->CMD   = kWrite32Bits;
      eeprom_register->WDATA = write_data[i];

      // Poll status register bit to see when writing is finished
      auto check_register = [] ()
      {
        return !(bit::Read(eeprom_register->INT_STATUS,
                           Status::kReadWriteStatusMask));
      };

      Wait(kMaxTimeout, check_register);

      // Clear write interrupt
      eeprom_register->INT_CLR_STATUS =
              (bit::Set(0, Status::kReadWriteStatusMask));

      page_offset = static_cast<uint8_t>(page_offset + kOffsetInterval);

      // If the 64 byte page buffer fills up, then it must be programmed to the
      // programmed to the EEPROM before continuing.
      if (page_offset > 64)
      {
        Program(address);
        page_count++;
        page_offset = 0;
      }
    }

    // Program final information so that it's stored in the EEPROM
    Program(address);
  }

  /// The EEPROM is accessed through a 64 byte page buffer, and after it fills
  /// up, it must be programmed to the EEPROM. This function handles that.
  void Program(uint32_t address) const
  {
    eeprom_register->ADDR = address;
    eeprom_register->CMD  = kEraseProgram;

    // Poll status register bit to see when writing is finished
    auto check_register = [] ()
    {
      return !(bit::Read(eeprom_register->INT_STATUS,
                         Status::kProgramStatusMask));
    };

    Wait(kMaxTimeout, check_register);

    // Clear program interrupt
    eeprom_register->INT_CLR_STATUS =
            (bit::Set(0, Status::kReadWriteStatusMask));
  }

  /// This function will return however much 32-bit data from the EEPROM
  /// starting at address and continuing on for how large count is.
  ///
  /// @param rdata   - array that read data will be stored in
  /// @param address - address where data will start being read from
  /// @param count   - number of bytes that have to be read
  void Read(uint8_t * rdata, uint32_t address, size_t count) const override
  {
    address = bit::Clear(address, kAddressMask);

    // Because the EEPROM uses 32-bit communication, read_data will be casted
    // into a uint32_t *
    uint32_t * read_data = reinterpret_cast<uint32_t *>(rdata);

    for (uint16_t index = 0; (index * 4) < count; index++)
    {
      eeprom_register->ADDR = address + (index * 4);
      eeprom_register->CMD  = kRead32Bits;
      read_data[index]      = eeprom_register->RDATA;
    }
  }

 private:
  const sjsu::SystemController & system_controller_;
};
}  // namespace lpc40xx
}  // namespace sjsu
