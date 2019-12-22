/*-----------------------------------------------------------------------*/
/* Low level disk I/O module skeleton for FatFs     (C)ChaN, 2016        */
/*-----------------------------------------------------------------------*/
/* If a working storage control module is available, it should be        */
/* attached to the FatFs via a glue function rather than modifying it.   */
/* This is an example of glue functions to attach various existing       */
/* storage control modules to the FatFs module with a defined API.       */
/*-----------------------------------------------------------------------*/

#include <diskio.h> /* Declarations of disk functions */
#include <ff.h>     /* Obtains integer types */

#include "L2_HAL/boards/sjtwo.hpp"

/* Definitions of physical drive number for each drive */
#define DEV_SD 0 /* Example: Map SD Card to physical drive 0 */
namespace
{
bool initialized = false;
}  // namespace

/// @param drive_number - Physical drive number to identify the drive
// NOLINTNEXTLINE
extern "C" DSTATUS disk_status([[maybe_unused]] BYTE drive_number)
{
  LOG_DEBUG("Getting Disk Status!");
  DSTATUS result = STA_NODISK;
  sjsu::lpc40xx::Gpio card_detect(1, 9);
  card_detect.SetAsInput();
  card_detect.GetPin().PullUp();
  // Card detect is active low
  if (!card_detect.Read())
  {
    LOG_DEBUG("Card IS present!");
    result = (initialized) ? 0 : STA_NOINIT;
  }
  else
  {
    LOG_DEBUG("Card is NOT present!");
  }
  return result;
}

// NOLINTNEXTLINE
extern "C" DSTATUS disk_initialize([[maybe_unused]] BYTE drive_number)
{
  LOG_DEBUG("DISK INIT!");
  sjtwo::SdCard().Initialize();
  sjsu::Sd::CardInfo_t card_info;
  DSTATUS status = sjtwo::SdCard().Mount(&card_info) ? 0 : STA_NOINIT;
  if (status == 0)
  {
    initialized = true;
  }
  return status;
}

// NOLINTNEXTLINE
extern "C" DRESULT disk_read([[maybe_unused]] BYTE drive_number, BYTE * buffer,
                             DWORD sector, UINT count)
{
  LOG_DEBUG("drive_number: %u :: sector: %ld :: count: %u", drive_number,
            sector, count);

  sjtwo::SdCard().ReadBlock(sector, buffer, static_cast<uint32_t>(count));
  return RES_OK;
}

#if FF_FS_READONLY == 0
// NOLINTNEXTLINE
extern "C" DRESULT disk_write([[maybe_unused]] BYTE drive_number,
                              const BYTE * buffer, DWORD sector, UINT count)
{
  LOG_DEBUG("drive_number: %u :: sector: %ld :: count: %u", drive_number,
            sector, count);
  sjtwo::SdCard().WriteBlock(sector, buffer, static_cast<uint32_t>(count));
  return RES_OK;
}

#endif
// NOLINTNEXTLINE
extern "C" DRESULT disk_ioctl([[maybe_unused]] BYTE drive_number,
                              [[maybe_unused]] BYTE command,
                              [[maybe_unused]] void * buffer)
{
  return RES_PARERR;
}
