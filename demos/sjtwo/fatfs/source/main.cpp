#include <ff.h>

#include <cstdint>
#include <cstring>

#include "peripherals/lpc40xx/spi.hpp"
#include "peripherals/lpc40xx/gpio.hpp"
#include "devices/memory/sd.hpp"
#include "utility/fatfs/fatfs.hpp"
#include "utility/log.hpp"

FRESULT ScanFiles(char * start_path, size_t path_length)
{
  static FILINFO fno;
  FRESULT res;
  DIR dir;

  res = f_opendir(&dir, start_path); /* Open the directory */
  if (res == FR_OK)
  {
    size_t i;
    while (true)
    {
      res = f_readdir(&dir, &fno); /* Read a directory item */
      // Break on error or end of dir
      if (res != FR_OK || fno.fname[0] == 0)
      {
        break;
      }
      // Check if file is a directory
      if (fno.fattrib & AM_DIR)
      {
        i = strlen(start_path);
        snprintf(&start_path[i], path_length - i, "/%s", fno.fname);
        res = ScanFiles(start_path, path_length); /* Enter the directory */
        if (res != FR_OK)
        {
          break;
        }
        start_path[i] = 0;
        printf("%s/\n", start_path);
      }
      // It is a file.
      else
      {
        printf("%s/%s\n", start_path, fno.fname);
      }
    }
    f_closedir(&dir);
  }
  return res;
}

int main()
{
  sjsu::LogInfo("Starting Fat Filesystem Application...");

  // Before we can use a storage device, we need to create the storage device we
  // want to use. In this case we want to use the SD card on the SJTwo board.
  sjsu::lpc40xx::Spi & spi2 = sjsu::lpc40xx::GetSpi<2>();
  sjsu::lpc40xx::Gpio sd_chip_select(1, 8);
  sjsu::lpc40xx::Gpio sd_card_detect(1, 9);
  sjsu::Sd card(spi2, sd_chip_select, sd_card_detect);

  // In order to use the SD card as a FAT filesystem drive, the storage media
  // needs to be registered. Register SD Card as driver number 0.
  sjsu::RegisterFatFsDrive(&card);
  sjsu::LogInfo("Mounting filesystem...");

  // Must be called before using fat fs.
  FATFS fat_fs;

  // See http://elm-chan.org/fsw/ff/00index_e.html
  // for more information about how to use the elm-chan fatfs APIs.
  auto mount_result = f_mount(&fat_fs, "", 0);

  if (mount_result != 0)
  {
    sjsu::LogError("Failed to mount SD Card");
    return -2;
  }

  sjsu::LogInfo("Scanning files on SD card...");
  std::array<char, 512> path;
  memset(path.data(), 0, path.size());
  path[0] = '/';
  ScanFiles(path.data(), path.size());

  return 0;
}
