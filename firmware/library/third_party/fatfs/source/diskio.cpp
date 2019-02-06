/*-----------------------------------------------------------------------*/
/* Low level disk I/O module skeleton for FatFs     (C)ChaN, 2016        */
/*-----------------------------------------------------------------------*/
/* If a working storage control module is available, it should be        */
/* attached to the FatFs via a glue function rather than modifying it.   */
/* This is an example of glue functions to attach various exsisting      */
/* storage control modules to the FatFs module with a defined API.       */
/*-----------------------------------------------------------------------*/

#include "ff.h"     /* Obtains integer types */
#include "diskio.h" /* Declarations of disk functions */
#include "L2_HAL/memory/sd.hpp"

/* Definitions of physical drive number for each drive */
#define DEV_SD 0 /* Example: Map SD Card to physical drive 0 */
namespace
{
Sd sd_card(Sd::DebugSdCard_t{});
bool initialized = false;
}  // namespace

/// @param drive_number - Physical drive number to identify the drive
extern "C" DSTATUS disk_status([[maybe_unused]] BYTE drive_number)
{
  LOG_DEBUG("Getting Disk Status!");
  DSTATUS result = STA_NODISK;
  Gpio card_detect(1, 9);
  card_detect.SetAsInput();
  card_detect.GetPin().SetMode(Pin::Mode::kPullUp);
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

extern "C" DSTATUS disk_initialize([[maybe_unused]] BYTE drive_number)
{
  LOG_DEBUG("DISK INIT!");
  sd_card.Initialize();
  Sd::CardInfo_t card_info;
  DSTATUS status = sd_card.Mount(&card_info) ? 0 : STA_NOINIT;
  if (status == 0)
  {
    initialized = true;
  }
  return status;
}

extern "C" DRESULT disk_read([[maybe_unused]] BYTE drive_number, BYTE * buffer,
                             DWORD sector, UINT count)
{
  LOG_DEBUG("drive_number: %u :: sector: %ld :: count: %u", drive_number,
            sector, count);

  sd_card.ReadBlock(sector, buffer, static_cast<uint32_t>(count));
  return RES_OK;
}

#if FF_FS_READONLY == 0

extern "C" DRESULT disk_write([[maybe_unused]] BYTE drive_number,
                              const BYTE * buffer, DWORD sector, UINT count)
{
  LOG_DEBUG("drive_number: %u :: sector: %ld :: count: %u", drive_number,
            sector, count);
  sd_card.WriteBlock(sector, buffer, static_cast<uint32_t>(count));
  return RES_OK;
}

#endif

extern "C" DRESULT disk_ioctl([[maybe_unused]] BYTE drive_number,
                              [[maybe_unused]] BYTE command,
                              [[maybe_unused]] void * buffer)
{
  return RES_PARERR;
}
