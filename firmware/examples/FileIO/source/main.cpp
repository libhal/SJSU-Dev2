#include <project_config.hpp>

#include <cstdint>

#include "third_party/fatfs/source/ff.h"
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

int main(void)
{
  LOG_INFO("Starting FileIO Application...");
  // Read a text file and display it
  /* Register work area to the default drive */
  LOG_INFO("Mounting filesystem...");
  FATFS fat_fs;
  f_mount(&fat_fs, "", 0);
  LOG_INFO("Filesystem mounted!");

  LOG_INFO("Scanning files in SD card...");
  char path[1024] = "/";
  ScanFiles(path, sizeof(path));
  LOG_INFO("SD card file scan complete!");

  LOG_INFO("FileIO Application Complete!");
  Halt();
  return 0;
}
