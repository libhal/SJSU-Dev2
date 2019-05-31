//-----------------------------------------------------------------------
// Low level disk I/O module skeleton for FatFs
//-----------------------------------------------------------------------
// Implement low level
//-----------------------------------------------------------------------

#include <diskio.h> /* Declarations of disk functions */
#include <ff.h>     /* Obtains integer types */

// NOLINTNEXTLINE
extern "C" DSTATUS disk_status([[maybe_unused]] BYTE drive_number)
{
  // Write disk status code here.
  // Example: for an SD card, one could check the Card Detect or CD line to see
  // if it has been asserted.
  return STA_NODISK;
}

// NOLINTNEXTLINE
extern "C" DSTATUS disk_initialize([[maybe_unused]] BYTE drive_number)
{
  // Write disk initialization code here
  return STA_NOINIT;
}

// NOLINTNEXTLINE
extern "C" DRESULT disk_read([[maybe_unused]] BYTE drive_number, BYTE * buffer,
                             DWORD sector, UINT count)
{
  // Write disk read here
  return RES_OK;
}

// NOLINTNEXTLINE
extern "C" DRESULT disk_write([[maybe_unused]] BYTE drive_number,
                              const BYTE * buffer, DWORD sector, UINT count)
{
  // Write disk write here
  return RES_OK;
}

// NOLINTNEXTLINE
extern "C" DRESULT disk_ioctl([[maybe_unused]] BYTE drive_number,
                              [[maybe_unused]] BYTE command,
                              [[maybe_unused]] void * buffer)
{
  // Write disk IOCTL here
  // See elm chan's FatFs documentation for what each command represents
  return RES_PARERR;
}
