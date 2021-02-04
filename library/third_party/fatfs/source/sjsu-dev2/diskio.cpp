#include <diskio.h>
#include <ff.h>
#include <ffconf.h>

#include <array>
#include <cstdint>
#include <tuple>

#include "config.hpp"
#include "peripherals/storage.hpp"
#include "peripherals/inactive.hpp"
#include "utility/log.hpp"
#include "utility/debug.hpp"
#include "utility/error_handling.hpp"

namespace
{
sjsu::Storage & empty_storage = sjsu::GetInactive<sjsu::Storage>();
std::array<sjsu::Storage *, config::kFatDriveCount> drive;
}  // namespace

namespace sjsu
{
void RegisterFatFsDrive(Storage * storage, uint8_t drive_number)
{
  if (drive_number >= drive.size())
  {
    throw Exception(
        std::errc::invalid_argument,
        "Drive number supplied is beyond the number of storage drives that "
        "can be registered as phyiscal drivers for FatFS. The limit is defined "
        "by config::kFatDriveCount. Increase that config value in order to add "
        "more drives.");
  }

  // Store the address of the storage media into the drive array
  drive[drive_number] = storage;
}
}  // namespace sjsu

/// @param drive_number - Physical drive number to identify the drive
// NOLINTNEXTLINE
extern "C" DSTATUS disk_status(BYTE drive_number)
{
  if (drive_number >= drive.size())
  {
    return RES_PARERR;
  }

  // Get a reference for the storage drive
  auto & storage = drive[drive_number];

  // If the storage media is not present, return NO_DISK
  if (!storage->IsMediaPresent())
  {
    return STA_NODISK;
  }

  // If the media was never initialized by calling disk_initialize, which occurs
  // after an f_mount() call, then return STA_NOINIT.
  if (storage->GetState() != sjsu::State::kInitialized)
  {
    return STA_NOINIT;
  }

  return RES_OK;
}

// NOLINTNEXTLINE
extern "C" DSTATUS disk_initialize(BYTE drive_number)
{
  if (drive_number >= drive.size())
  {
    return RES_PARERR;
  }

  // Get a reference for the storage drive
  auto & storage = drive[drive_number];

  // Attempt to enable media and on failure return STA_NOINIT
  storage->Initialize();

  return RES_OK;
}

std::tuple<uint32_t, uint32_t> CorrectedLocation(uint32_t block_size,
                                                 uint32_t sector,
                                                 uint32_t count)
{
  uint32_t address = sector;

  // Convert the sector number to a byte address
  address = address * FF_MIN_SS;

  // Divide block address by the size of each block for this media to get the
  // correct block address for this media.
  //
  // Final Equation:
  //
  //     address = (sector_number * bytes_per_sector) / storage_size_per_block
  //
  address = address / block_size;

  size_t length = count;

  // convert length into number of bytes to read.
  // Storage devices can read into buffers that are not aligned with
  length = static_cast<size_t>(length * FF_MIN_SS);

  return std::make_tuple(address, length);
}

// NOLINTNEXTLINE
extern "C" DRESULT disk_read(BYTE drive_number,
                             BYTE * buffer,
                             DWORD sector,
                             UINT count)
{
  if (drive_number >= drive.size())
  {
    return RES_PARERR;
  }

  // Get a reference for the storage drive
  auto & storage = drive[drive_number];

  // Get the number of bytes per block for this media.
  units::data::byte_t block_size = storage->GetBlockSize();

  auto location = CorrectedLocation(block_size.to<uint32_t>(), sector, count);
  uint32_t location_block = std::get<0>(location);
  uint32_t location_count = std::get<1>(location);

  // Read from blocks
  storage->Read(location_block,
                      std::span<uint8_t>(buffer, location_count));

  return RES_OK;
}

// NOLINTNEXTLINE
extern "C" DRESULT disk_write(BYTE drive_number,
                              const BYTE * buffer,
                              DWORD sector,
                              UINT count)
{
  if (drive_number >= drive.size())
  {
    return RES_PARERR;
  }

  // Get a reference for the storage drive
  auto & storage = drive[drive_number];

  // Get the number of bytes per block for this media.
  units::data::byte_t block_size = storage->GetBlockSize();

  auto location = CorrectedLocation(block_size.to<uint32_t>(), sector, count);
  uint32_t location_block = std::get<0>(location);
  uint32_t location_count = std::get<1>(location);

  // Erase-before-write for media that requires this
  storage->Erase(location_block, location_count);

  // Write to the block
  storage->Write(location_block,
                       std::span<const uint8_t>(buffer, location_count));

  return RES_OK;
}

// NOLINTNEXTLINE
extern "C" DRESULT disk_ioctl([[maybe_unused]] BYTE drive_number,
                              [[maybe_unused]] BYTE command,
                              [[maybe_unused]] void * buffer)
{
  return RES_PARERR;
}
