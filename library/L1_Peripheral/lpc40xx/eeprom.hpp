#pragma once

#include <chrono>

#include "L0_Platform/lpc40xx/LPC40xx.h"
#include "L1_Peripheral/lpc40xx/system_controller.hpp"
#include "L1_Peripheral/storage.hpp"
#include "utility/bit.hpp"
#include "utility/status.hpp"

namespace sjsu
{
namespace lpc40xx
{
/// Implementation of the EEPROM interface for the LPC40xx family of
/// microcontrollers.
class Eeprom final : public sjsu::Storage
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
  struct Status  // NOLINT
  {
    /// Mask to get value of programming status bit
    static constexpr bit::Mask kProgramStatusMask =
        bit::CreateMaskFromRange(28);
    /// Mask to get value of read/write status bit
    static constexpr bit::Mask kReadWriteStatusMask =
        bit::CreateMaskFromRange(26);
  };

  /// EEPROM Command codes for reading from, writing to, and programming the
  /// device
  enum command_codes
  {
    kRead32Bits   = 0b010,
    kWrite32Bits  = 0b101,
    kEraseProgram = 0b110
  };

  /// Max timeout for program/write operations in milliseconds
  static constexpr std::chrono::milliseconds kMaxTimeout = 20ms;

  Type GetMemoryType() override
  {
    return Type::kEeprom;
  }

  /// Initializing the EEPROM requires setting the wait state register, setting
  /// the clock divider register, and ensuring that the device is powered on.
  sjsu::Status Initialize() override
  {
    const float kSystemClock = static_cast<float>(
        sjsu::SystemController::GetPlatformController().GetSystemFrequency());
    // The EEPROM runs at 375 kHz
    constexpr float kEepromClk  = 375'000;
    constexpr float kNanosecond = 1E-9f;

    // The EEPROM is turned on by default, but in case it was somehow
    // turned off, we turn it on by writing 0 to the PWRDWN register
    Enable();

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
    return sjsu::Status::kSuccess;
  }

  /// EEPROM is apart of the lpc40xx silicon so it is always present.
  bool IsMediaPresent() override
  {
    return true;
  }

  sjsu::Status Enable() override
  {
    eeprom_register->PWRDWN = 0;
    return sjsu::Status::kSuccess;
  }

  sjsu::Status Disable() override
  {
    eeprom_register->PWRDWN = 1;
    return sjsu::Status::kSuccess;
  }

  bool IsReadOnly() override
  {
    return false;
  }

  units::data::byte_t GetCapacity() override
  {
    return 4_kB;
  }

  units::data::byte_t GetBlockSize() override
  {
    return 4_B;
  }

  sjsu::Status Erase(uint32_t, size_t) override
  {
    return sjsu::Status::kSuccess;
  }

  sjsu::Status Write(uint32_t address, const void * data, size_t size) override
  {
    constexpr bit::Mask kLower6Bits = bit::CreateMaskFromRange(0, 5);
    constexpr bit::Mask kUpper6Bits = bit::CreateMaskFromRange(6, 11);

    address = bit::Clear(address, kAddressMask);

    // Because the EEPROM uses 32-bit communication, write_data will be casted
    // into a uint32_t *
    const uint32_t * write_data = reinterpret_cast<const uint32_t *>(data);

    // The first 6 bits in the address (MSB) dictate which page is being written
    // to in the EEPROM, and the last 6 bits (LSB) dictate offset in the page
    uint32_t page_count  = bit::Extract(address, kUpper6Bits);
    uint32_t page_offset = bit::Extract(address, kLower6Bits);

    uint16_t eeprom_address;

    for (size_t i = 0; (i * 4) < size; i++)
    {
      // Page offset is incremented by 4 because we're writing 32 bits
      constexpr uint8_t kOffsetInterval = 4;

      eeprom_address = static_cast<uint16_t>((page_count << 6) + page_offset);

      eeprom_register->ADDR  = eeprom_address;
      eeprom_register->CMD   = kWrite32Bits;
      eeprom_register->WDATA = write_data[i];

      // Poll status register bit to see when writing is finished
      auto check_register = []() {
        return !(bit::Read(eeprom_register->INT_STATUS,
                           Status::kReadWriteStatusMask));
      };

      Wait(kMaxTimeout, check_register);

      // Clear write interrupt
      eeprom_register->INT_CLR_STATUS =
          (bit::Set(0, Status::kReadWriteStatusMask));

      page_offset += kOffsetInterval;

      // If the 64 byte page buffer fills up, then it must be programmed to the
      // programmed to the EEPROM before continuing.
      if (page_offset > 64)
      {
        Program(eeprom_address);
        page_count++;
        page_offset = 0;
      }
    }

    // Program final information so that it's stored in the EEPROM
    Program(eeprom_address);

    return sjsu::Status::kSuccess;
  }

  sjsu::Status Read(uint32_t address, void * data, size_t size) override
  {
    address = bit::Insert(address, 0b00, kAddressMask);

    // Because the EEPROM uses 32-bit communication, read_data will be casted
    // into a uint32_t *
    uint32_t * read_data = reinterpret_cast<uint32_t *>(data);

    for (uint16_t index = 0; (index * 4) < size; index++)
    {
      eeprom_register->ADDR = address + (index * 4);
      eeprom_register->CMD  = kRead32Bits;
      read_data[index]      = eeprom_register->RDATA;
    }
    return sjsu::Status::kSuccess;
  }

 private:
  /// The EEPROM is accessed through a 64 byte page buffer, and after it fills
  /// up, it must be programmed to the EEPROM. This function handles that.
  void Program(uint32_t address)
  {
    eeprom_register->ADDR = address;
    eeprom_register->CMD  = kEraseProgram;

    // Poll status register bit to see when writing is finished
    auto check_register = []() {
      return !(
          bit::Read(eeprom_register->INT_STATUS, Status::kProgramStatusMask));
    };

    Wait(kMaxTimeout, check_register);

    // Clear program interrupt
    eeprom_register->INT_CLR_STATUS =
        (bit::Set(0, Status::kReadWriteStatusMask));
  }
};
}  // namespace lpc40xx
}  // namespace sjsu
