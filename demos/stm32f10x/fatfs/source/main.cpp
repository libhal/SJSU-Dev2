#include "L0_Platform/startup.hpp"
#include "L1_Peripheral/stm32f10x/gpio.hpp"
#include "L1_Peripheral/stm32f10x/system_controller.hpp"
#include "L2_HAL/memory/sd.hpp"
#include "L3_Application/file_io/fatfs.hpp"
#include "utility/debug.hpp"
#include "utility/log.hpp"
#include "utility/time.hpp"

class BitBangSpi : public sjsu::Spi
{
 public:
  BitBangSpi(sjsu::Gpio & sck, sjsu::Gpio & mosi, sjsu::Gpio & miso)
      : sck_(sck), mosi_(mosi), miso_(miso)
  {
  }

  sjsu::Status Initialize() const override
  {
    mosi_.SetAsOutput();
    miso_.SetAsInput();
    sck_.SetAsOutput();
    sck_.SetLow();
    mosi_.SetLow();
    miso_.GetPin().PullUp();
    return sjsu::Status::kSuccess;
  }

  uint16_t Transfer(uint16_t data) const override
  {
    uint16_t result = 0;
    for (int i = 0; i < 8; i++)
    {
      // Set MOSI pin to the correct bit
      mosi_.Set(sjsu::Gpio::State{ sjsu::bit::Read(data, 7 - i) });

      // Clock the SCK pin
      sck_.SetHigh();

      // Sample MISO pin
      result = static_cast<uint16_t>(result | miso_.Read() << (7 - i));

      // Pull clock pin low
      sck_.SetLow();
    }

    sck_.SetLow();
    return result;
  }

  void SetDataSize(DataSize size) const override
  {
    size_ = size;
  }

  void SetClock(units::frequency::hertz_t frequency,
                bool = false,
                bool = false) const override
  {
    if (frequency < 1_MHz)
    {
      delay_ = true;
    }
    else
    {
      delay_ = false;
    }
  }

 private:
  sjsu::Gpio & sck_;
  sjsu::Gpio & mosi_;
  sjsu::Gpio & miso_;
  mutable DataSize size_;
  mutable bool delay_ = false;
};

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
  sjsu::LogInfo("Starting FatFS Example...");

  sjsu::stm32f10x::SetMaximumClockSpeedUsingInternalOscillator();

  // Construct the SD Card object
  sjsu::stm32f10x::Gpio miso('A', 6);
  sjsu::stm32f10x::Gpio mosi('A', 7);
  sjsu::stm32f10x::Gpio sck('A', 5);

  BitBangSpi spi(sck, mosi, miso);
  sjsu::stm32f10x::Gpio sd_chip_select('A', 4);

  sjsu::Sd card(spi, sd_chip_select, sjsu::GetInactive<sjsu::Gpio>());

  // Register SD Card as driver number 0.
  // This must be done before using any of the FatFS APIs.
  auto success = sjsu::RegisterFatFsDrive(&card);
  if (!success)
  {
    // sjsu::LogError("Failed to add SD Card to list of FAT FS drives.");
    return -1;
  }

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
