#include "ff.h"     /* Obtains integer types */
#include "diskio.h" /* Declarations of disk functions */
#include "utility/log.hpp"

extern "C" [[gnu::weak]] DSTATUS disk_status([[maybe_unused]] BYTE pdrv)
{
  sjsu::LogInfo("FatFS not currently supported on this platform");
  sjsu::Halt();
  return STA_NOINIT;
}

extern "C" [[gnu::weak]] DSTATUS disk_initialize([
    [maybe_unused]] BYTE drive_number)
{
  sjsu::LogInfo("FatFS not currently supported on this platform");
  sjsu::Halt();
  return RES_PARERR;
}

extern "C" [[gnu::weak]] DRESULT disk_read([[maybe_unused]] BYTE drive_number,
                                           [[maybe_unused]] BYTE * buffer,
                                           [[maybe_unused]] DWORD sector,
                                           [[maybe_unused]] UINT count)
{
  sjsu::LogInfo("FatFS not currently supported on this platform");
  sjsu::Halt();
  return RES_PARERR;
}

extern "C" [[gnu::weak]] DRESULT disk_write(
    [[maybe_unused]] BYTE drive_number,
    [[maybe_unused]] const BYTE * buffer,
    [[maybe_unused]] DWORD sector,
    [[maybe_unused]] UINT count)
{
  sjsu::LogInfo("FatFS not currently supported on this platform");
  sjsu::Halt();
  return RES_PARERR;
}

extern "C" [[gnu::weak]] DRESULT disk_ioctl([[maybe_unused]] BYTE drive_number,
                                            [[maybe_unused]] BYTE command,
                                            [[maybe_unused]] void * buffer)
{
  sjsu::LogInfo("FatFS not currently supported on this platform");
  sjsu::Halt();
  return RES_PARERR;
}
