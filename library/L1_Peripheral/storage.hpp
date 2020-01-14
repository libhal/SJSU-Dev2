#pragma once

#include <cstdint>
#include <cstddef>

#include "utility/units.hpp"
#include "utility/status.hpp"

namespace sjsu
{
/// Abstract interface for persistent memory storage systems.
class Storage
{
 public:
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

  /// @return the type of memory this driver controls. Can be called without
  /// calling Initialize() first.
  virtual Type GetMemoryType() = 0;

  /// Initialize all peripherals required to make communicate with the storage
  /// media possible.
  /// MUST be called before calling any method in this interface with the
  /// exception of `GetMemoryType()`
  virtual sjsu::Status Initialize() = 0;

  /// @return true if the storage is present. For cases where the memory cannot
  /// be removed or is physically located within a device, this should always
  /// return true.
  /// @return false if storage media is not present.
  virtual bool IsMediaPresent() = 0;

  /// Should prepare and configure the storage media for communication such as
  /// Write(), Read(), Erase()
  virtual sjsu::Status Enable() = 0;

  /// Should shutdown the device.
  virtual sjsu::Status Disable() = 0;

  /// @return true if device is not writable.
  virtual bool IsReadOnly() = 0;

  /// @return the maximum capacity of this storage media. This includes areas
  /// that have already been written to. This does not include sections of the
  /// memory that are not accessible. For example, if the first 2kB of the
  /// memory cannot be accessed via this driver, then it should not be
  /// considered as apart of the capacity.
  virtual units::data::byte_t GetCapacity() = 0;

  /// @return the number of bytes per block.
  virtual units::data::byte_t GetBlockSize() = 0;

  /// Must be called before a `Write()` operation. Erases the contents of the
  /// storage media in the location specified, the number of bytes given.
  ///
  /// @param address - Address location to erase
  /// @param size - the number of bytes to erase
  /// @return Status of if the operation was successful, otherwise, returns an
  /// appropriate status signal.
  virtual sjsu::Status Erase(uint32_t address, size_t size) = 0;

  /// Write data to the storage media in the location specified. Underlying
  /// implementation must handle things such as single byte write operations,
  /// but will not guarantee that the operation is efficient. Try and keep write
  /// operation sizes address locations aligned with the result of
  /// `GetBlockSize()`.
  /// Make sure to call Erase() on the memory location you plan to write to
  /// before writing, otherwise, the data may be corrupted or unchanged.
  ///
  /// @param address - address to write data to.
  /// @param data - data to be stored in the location addressed above.
  /// @param size - the number of bytes to write into memory.
  /// @return Status of if the operation was successful, otherwise, returns an
  /// appropriate status signal.
  virtual sjsu::Status Write(uint32_t address,
                             const void * data,
                             size_t size) = 0;

  /// Read data from the storage media in the location specified. Underlying
  /// implementation must handle things such as single byte read operations
  /// but will not guarantee that the operation is efficient. Try and keep read
  /// operation sizes address locations aligned with the result of
  /// `GetBlockSize()`.
  ///
  /// @param address - address to read data from.
  /// @param data - pointer to location to read data from storage into.
  /// @param size - the number of bytes to read from memory.
  /// @return Status of if the operation was successful, otherwise, return an
  /// appropriate status signal.
  virtual sjsu::Status Read(uint32_t address, void * data, size_t size) = 0;
};
}  // namespace sjsu
