#pragma once

#include <ff.h>

#include "peripherals/storage.hpp"
#include "utility/error_handling.hpp"

namespace sjsu
{
/// Register a storage device to the list of devices that can be controlled by
/// fatfs.
///
/// NOTE: Driver 0 is special in that you do not need to specify the drive
/// number in the file path.
///
/// For example:
///
///     Drive Number 0: f_open("/path/to/file", ...);
///     Drive Number 1: f_open("1:/path/to/file", ...);
///
/// For all other driver numbers a prefix of `<number>:` is required.
///
/// @param storage - reference to the storage device that contains a fatfs
///                  filesystem.
/// @param drive_number - the drive designation number for device.
/// @return int - an error if there is one.
void RegisterFatFsDrive(Storage * storage, uint8_t drive_number = 0);

/// @param result - the fatfs result to convert to a string description.
/// @return a string description of the passed fatfs result.
inline const char * Stringify(FRESULT result)
{
  static const char * k_result_string_table[] = {
    [FR_OK]       = "(0) Succeeded",
    [FR_DISK_ERR] = "(1) A hard error occurred in the low level disk I/O layer",
    [FR_INT_ERR]  = "(2) Assertion failed",
    [FR_NOT_READY]    = "(3) The physical drive cannot work",
    [FR_NO_FILE]      = "(4) Could not find the file",
    [FR_NO_PATH]      = "(5) Could not find the path",
    [FR_INVALID_NAME] = "(6) The path name format is invalid",
    [FR_DENIED] =
        "(7) Access denied due to prohibited access or directory full",
    [FR_EXIST]           = "(8) Access denied due to prohibited access",
    [FR_INVALID_OBJECT]  = "(9) The file/directory object is invalid",
    [FR_WRITE_PROTECTED] = "(10) The physical drive is write protected",
    [FR_INVALID_DRIVE]   = "(11) The logical drive number is invalid",
    [FR_NOT_ENABLED]     = "(12) The volume has no work area",
    [FR_NO_FILESYSTEM]   = "(13) There is no valid FAT volume",
    [FR_MKFS_ABORTED]    = "(14) The f_mkfs() aborted due to any problem",
    [FR_TIMEOUT] =
        "(15) Could not get a grant to access the volume within defined period",
    [FR_LOCKED] =
        "(16) The operation is rejected according to the file sharing policy",
    [FR_NOT_ENOUGH_CORE]     = "(17) LFN working buffer could not be allocated",
    [FR_TOO_MANY_OPEN_FILES] = "(18) Number of open files > FF_FS_LOCK",
    [FR_INVALID_PARAMETER]   = "(19) Given parameter is invalid"
  };

  return (result <= FR_INVALID_PARAMETER) ? k_result_string_table[result]
                                          : "Invalid!";
}
}  // namespace sjsu
