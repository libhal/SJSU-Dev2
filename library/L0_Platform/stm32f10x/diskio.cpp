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

/// @param drive_number - Physical drive number to identify the drive
// NOLINTNEXTLINE
extern "C" DSTATUS disk_status([[maybe_unused]] BYTE drive_number)
{
  return STA_NODISK;
}

// NOLINTNEXTLINE
extern "C" DSTATUS disk_initialize([[maybe_unused]] BYTE drive_number)
{
  return STA_NOINIT;
}

// NOLINTNEXTLINE
extern "C" DRESULT disk_read([[maybe_unused]] BYTE drive_number,
                             [[maybe_unused]] BYTE * buffer,
                             [[maybe_unused]] DWORD sector,
                             [[maybe_unused]] UINT count)
{
  return RES_OK;
}

#if FF_FS_READONLY == 0
// NOLINTNEXTLINE
extern "C" DRESULT disk_write([[maybe_unused]] BYTE drive_number,
                              [[maybe_unused]] const BYTE * buffer,
                              [[maybe_unused]] DWORD sector,
                              [[maybe_unused]] UINT count)
{
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
