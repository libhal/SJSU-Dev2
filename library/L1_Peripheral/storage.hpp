#pragma once

#include <cstddef>
#include <cstdint>
#include <span>
#include <string>

#include "L1_Peripheral/inactive.hpp"
#include "module.hpp"
#include "utility/error_handling.hpp"
#include "utility/units.hpp"

namespace sjsu
{
/// Abstract interface for persistent memory storage systems.
///
/// @ingroup l1_peripheral
class Storage : public Module
{
 public:
  // ===========================================================================
  // Interface Definitions
  // ===========================================================================

  /// Defines the types of storage media.
  enum class Type
  {
    /// Simple storage implemented in RAM
    kRam,
    /// Raw NOR flash, without a sophisticated controller
    kNor,
    /// Raw NAND flash, without a sophisticated controller
    kNand,
    /// SD card
    kSD,
    /// eMMC managed NAND flash memory
    kEMMC,
    /// Electrically Erasable Programmable Read-Only Memory
    kEeprom,
    /// Non-volatile RAM
    kNvRam,
    /// Ferromagnetic RAM
    kFRam,
  };

  // ===========================================================================
  // Interface Methods
  // ===========================================================================

  // ---------------------------------------------------------------------------
  // Configuration Methods
  // ---------------------------------------------------------------------------

  // ---------------------------------------------------------------------------
  // Usage Methods
  // ---------------------------------------------------------------------------

  /// @return the type of memory this driver controls. Can be called without
  ///         calling Initialize() first.
  virtual Type GetMemoryType() = 0;

  /// @return true if the storage is present. For cases where the memory cannot
  ///         be removed or is physically located within a device, this should
  ///         always return true.
  /// @return false if storage media is not present.
  virtual bool IsMediaPresent() = 0;

  /// @return true if device is not writable.
  virtual bool IsReadOnly() = 0;

  /// @return the maximum capacity of this storage media. This includes areas
  ///         that have already been written to. This does not include sections
  ///         of the memory that are not accessible. For example, if the first
  ///         2kB of the memory cannot be accessed via this driver, then it
  ///         should not be considered as apart of the capacity.
  virtual units::data::byte_t GetCapacity() = 0;

  /// @return the number of bytes per block.
  virtual units::data::byte_t GetBlockSize() = 0;

  /// Must be called before a `Write()` operation. Erases the contents of the
  /// storage media in the location specified, the number of bytes given. Some
  /// storage media do not require an erasure before a write to be performed. In
  /// those cases, the implementation should simply do nothing on erase.
  ///
  /// @param block_address - starting block to erase.
  /// @param blocks_count - the number of bytes to erase.
  /// @return Status of if the operation was successful, otherwise, returns an
  ///         appropriate status signal.
  virtual void Erase(uint32_t block_address, size_t blocks_count) = 0;

  /// Write data to the storage media in the location block specified. If the
  /// block size for this media is not 1 byte, then single byte is not allowed
  /// and transactions must be performed at a block level.
  ///
  /// Be sure to call Erase() on the memory location you plan to write to before
  /// writing, otherwise, the data may be corrupted or unchanged.
  ///
  /// @param block_address - starting block to write to
  /// @param data - buffer of data to be stored in the location addressed.
  virtual void Write(uint32_t block_address, std::span<const uint8_t> data) = 0;

  /// Read data from the storage media in the location specified.
  ///
  /// @param block_address - starting block to read from.
  /// @param data - buffer to hold the data stored in the location address.
  virtual void Read(uint32_t block_address, std::span<uint8_t> data) = 0;

  // ===========================================================================
  // Utility Methods
  // ===========================================================================

  /// Helper function that overloads the Write function to allow usage of the
  /// std::string_view container.
  ///
  /// @param block_address - starting block to write to
  /// @param data - string view of characters to stored in the location.
  void Write(uint32_t block_address, std::string_view data)
  {
    const uint8_t * pointer = reinterpret_cast<const uint8_t *>(data.data());

    Write(block_address, std::span(pointer, data.size()));
  }
};

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

    void ModuleInitialize() override {}
    void ModuleEnable(bool = true) override {}

    bool IsMediaPresent() override
    {
      return true;
    }
    bool IsReadOnly() override
    {
      return true;
    }
    units::data::byte_t GetCapacity() override
    {
      return 0_B;
    }
    units::data::byte_t GetBlockSize() override
    {
      return 0_B;
    }
    void Erase(uint32_t, size_t) override {}
    void Write(uint32_t, std::span<const uint8_t>) override {}
    void Read(uint32_t, std::span<uint8_t>) override {}
  };

  static InactiveStorage inactive;
  return inactive;
}
}  // namespace sjsu
