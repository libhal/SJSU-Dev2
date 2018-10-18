// SJTwo Hyperload Version 1.0
#include <project_config.hpp>

#include <cmath>
#include <cstdarg>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "config.hpp"
#include "L0_LowLevel/delay.hpp"
#include "L0_LowLevel/interrupt.hpp"
#include "L0_LowLevel/LPC40xx.h"
#include "L0_LowLevel/startup.hpp"
#include "L0_LowLevel/uart0.hpp"
#include "L0_LowLevel/uart2.hpp"
#include "L2_Utilities/debug.hpp"
#include "L2_Utilities/macros.hpp"
#include "L3_HAL/onboard_led.hpp"

#if !defined(BOOTLOADER) && !defined(CLANG_TIDY)
#error Hyperload must be built as a 'bootloader' and not as an application or \
       test. Please build this software using 'make bootloader'
#endif

// NOLINTNEXTLINE(readability-identifier-naming)
int printf2(const char * format, ...)
{
  constexpr size_t kPrintfBufferSize = 256;
  char buffer[kPrintfBufferSize];
  va_list args;
  va_start(args, format);
  int length = vsnprintf(buffer, kPrintfBufferSize, format, args);
  va_end(args);

  uart2::Puts(buffer);
  return length;
}

constexpr char kHyperloadFinished   = '*';
constexpr char kHyperloadReady      = '!';
constexpr char kChecksumError       = '@';
constexpr char kOtherError          = '^';
constexpr uint32_t kBlockSize       = 0x1000;                         // 4096
constexpr uint32_t kBlocksPerSector = 0x8;                            // 4096
constexpr uint32_t kSectorSize      = kBlockSize * kBlocksPerSector;  // 32 kB
constexpr uint32_t kSizeOfSector    = 0x8000;
constexpr uint32_t kStartSector     = 16;
constexpr uint32_t kLastSector      = 29;
constexpr uint32_t kApplicationStartAddress = kStartSector * kBlockSize;
constexpr uint32_t kFlashDelay              = 10;

enum IapCommands : uint8_t
{
  kPrepareFlash           = 50,
  kCopyRamToFlash         = 51,
  kEraseSector            = 52,
  kBlankCheckSector       = 53,
  kReadPartId             = 54,
  kReadBootCode           = 55,
  kReadDeviceSerialNumber = 58,
  kCompare                = 56,
  kReinvokeIsp            = 57
};

enum class IapResult : uint32_t
{
  kCmdSuccess        = 0,
  kInvalidCommand    = 1,
  kSrcAddrError      = 2,
  kDstAddrError      = 3,
  kSrcAddrNotMapped  = 4,
  kDstAddrNotMapped  = 5,
  kCountError        = 6,
  kInvalidSector     = 7,
  kSectorNotBlank    = 8,
  kSectorNotPrepared = 9,
  kCompareError      = 10,
  kBusy              = 11
};

const char * const kIapResultString[] = {
  "kCmdSuccess",       "kInvalidCommand",    "kSrcAddrError", "kDstAddrError",
  "kSrcAddrNotMapped", "kDstAddrNotMapped",  "kCountError",   "kInvalidSector",
  "kSectorNotBlank",   "kSectorNotPrepared", "kCompareError", "kBusy"
};

SJ2_PACKED(struct) IapCommand_t
{
  uint32_t command;
  intptr_t parameters[4];
};

SJ2_PACKED(struct) IapStatus_t
{
  IapResult result;
  intptr_t parameters[4];
};

SJ2_PACKED(struct) Block_t
{
  uint8_t data[kBlockSize];
};

SJ2_PACKED(struct) Sector_t
{
  Block_t block[kBlocksPerSector];
};

SJ2_PACKED(struct) FlashMemory_t
{
  // [sector][block][block_bytes]
  Block_t bootloader[kStartSector];
  // [sector][block][block_bytes]
  Sector_t application[kLastSector - kStartSector];
};

using IapFunction = void (*)(IapCommand_t *, IapStatus_t *);

uint8_t WriteUartToBlock(Block_t * block);
bool WriteUartToRamSector(Sector_t * sector);
uint32_t BlockToSector(uint32_t block);
IapResult PrepareSector(uint32_t start, uint32_t end);
IapResult EraseSector(uint32_t start, uint32_t end);
IapResult FlashBlock(Block_t * block, uint32_t sector_number,
                     uint32_t block_number);
IapResult VerifySector(Sector_t * ram_sector, uint32_t sector_number);
IapResult BlankCheckSector(uint32_t start, uint32_t end);
void EraseWithVerifySector(uint32_t sector_number);
IapResult FlashSector(Sector_t * ram_sector, uint32_t sector_number,
                      uint32_t blocks_filled_in_sector = kBlocksPerSector);

namespace hyperload
{
struct Version_t
{
  uint8_t major;
  uint8_t minor;
};
const float kStandardBaudRates[] = {
  4800,
  9600,
  19200,
  38400,
  57600,
  115200,
  230400,
  // 460800,
  // 500000,
  576000,
  921600,
  1000000,
  1152000,
  1500000,
  2000000,
  2500000,
  3000000,
};

float FindNearestBaudRate(float baud_rate)
{
  float result = 38400;
  for (size_t i = 0; i < SJ2_ARRAY_LENGTH(kStandardBaudRates); i++)
  {
    if (0.9f * kStandardBaudRates[i] <= baud_rate &&
        baud_rate <= 1.1f * kStandardBaudRates[i])
    {
      result = kStandardBaudRates[i];
      break;
    }
  }
  return result;
}
}  // namespace hyperload

const hyperload::Version_t kHyperload = { 1, 1 };
FlashMemory_t * flash                 = reinterpret_cast<FlashMemory_t *>(0x0);
IapFunction iap = reinterpret_cast<IapFunction>(0x1FFF1FF1);

void SetFlashAcceleratorSpeed(int32_t clocks_per_flash_access)
{
  if (clocks_per_flash_access > 6)
  {
    clocks_per_flash_access = 6;
  }
  else if (clocks_per_flash_access <= 0)
  {
    clocks_per_flash_access = 1;
  }
  clocks_per_flash_access -= 1;
  // Set flash memory access clock rate to 6 clocks per access
  LPC_SC->FLASHCFG =
      (LPC_SC->FLASHCFG & ~(0b1111 << 12)) | (clocks_per_flash_access << 12);
}

int main(void)
{
  Gpio button0(1, 0);
  button0.SetMode(Pin::Mode::kPullDown);
  button0.SetAsInput();
  OnBoardLed leds;
  leds.Initialize();
  leds.SetAll(0);
  uart2::Init(115200);
  printf2("Bootloader Debug Port Initialized!\n");
  // Flush any initial bytes
  uart0::GetChar(500);
  uart0::PutChar(0xFF);
  // Hyperload will send 0x55 to notify that it is alive!
  if (0x55 == uart0::GetChar(500))
  {
    SetFlashAcceleratorSpeed(6);
    // Notify Hyperload that we're alive too!
    uart0::PutChar(0xAA);
    // Get new baud rate control word
    union BaudRateControlWord {
      uint8_t array[4];
      uint32_t word;
    };
    BaudRateControlWord control;
    control.array[0] = uart0::GetChar(500);
    control.array[1] = uart0::GetChar(500);
    control.array[2] = uart0::GetChar(500);
    control.array[3] = uart0::GetChar(500);
    // Echo it back to verify
    uart0::PutChar(control.array[0]);
    // Hyperload Frequency should be set to 48,000,000 for this to work
    // correctly It calculates the baud rate by: 48Mhz/(16//BAUD) - 1 = CW
    // (control word) So we solve for BAUD:  BAUD = (48/(CW + 1))/16
    float control_word_f = static_cast<float>(control.word);
    float approx_baud =
        (config::kSystemClockRate / (control_word_f + 1.0f)) / 16.0f;
    uint32_t baud_rate =
        static_cast<uint32_t>(hyperload::FindNearestBaudRate(approx_baud));
    uart0::Init(baud_rate);
    // Wait for host to change it's baud rate
    Delay(500);
    // Send our CPU information along with data parameters:
    // Name:Blocksize:Bootsize/2:FlashSize
    uart0::Puts("$LPC4078:4096:32768:512\n");
    bool finished = false;
    for (uint32_t sector_number = kStartSector; !finished; sector_number++)
    {
      Sector_t ram_sector;
      finished         = WriteUartToRamSector(&ram_sector);
      IapResult result = FlashSector(&ram_sector, sector_number);
      if (result == IapResult::kCmdSuccess)
      {
        leds.SetAll(0);
        uart2::Puts("Sector Flash Successful!\n");
      }
      else
      {
        uint8_t error = static_cast<uint8_t>(result);
        leds.SetAll(error);
        printf2("Flashing error %s!\n",
                kIapResultString[static_cast<uint32_t>(result)]);
        uart0::PutChar(kOtherError);
      }
    }
    uart2::Puts("Programming Finished!\n");
    uart2::Puts("Sending final acknowledge!\n");
    Delay(500);
    uart0::PutChar(kHyperloadFinished);
  }
  // Change baud rate back to 38400 so that user can continue using a serial
  // monitor for the final bootloader message and application messages.
  uart0::Init(config::kBaudRate);

  IsrPointer * application_vector_table =
      reinterpret_cast<IsrPointer *>(&(flash->application));
  IsrPointer application_entry_isr = application_vector_table[1];

  printf("Hyperload Version (%d.%d)\n", kHyperload.major, kHyperload.minor);
  if (button0.Read())
  {
    constexpr uint32_t kSize16kB = 1 << 13;
    void * vector_address = reinterpret_cast<void *>(application_vector_table);
    printf("Hexdump @ %p \n", vector_address);
    debug::Hexdump(vector_address, kSize16kB);
    while (true)
    {
      continue;
    }
  }
  else if (application_entry_isr == reinterpret_cast<void *>(0xFFFFFFFFUL))
  {
    uart0::Puts("Application Not Found, Halting System ...\n");
    while (true)
    {
      continue;
    }
  }

  printf("Application Reset ISR value = %p\n", application_entry_isr);
  Delay(500);
  button0.SetMode(Pin::Mode::kPullUp);
  leds.SetAll(0);
  // SystemTimerIrq must be disabled, otherwise it will continue to fire,
  // after the application is  executed. This can lead to a lot of problems
  // depending on the how the application is written.
  system_timer.DisableTimer();
  // Moving the interrupt vector table pointer
  SCB->VTOR = reinterpret_cast<intptr_t>(application_vector_table);
  // Jump to application code
  uart0::Puts("Booting Application...\n");
  application_entry_isr();
  return 0;
}

// TODO(#177): All of the code below should be moved into the file
// library/L0_LowLevel/lpc_flash.hpp
uint8_t WriteUartToBlock(Block_t * block)
{
  uint32_t checksum = 0;
  for (uint32_t position = 0; position < kBlockSize; position++)
  {
    uint8_t byte          = uart0::GetChar(100);
    block->data[position] = byte;
    checksum += byte;
  }
  return static_cast<uint8_t>(checksum & 0xFF);
}

bool WriteUartToRamSector(Sector_t * sector)
{
  bool finished           = false;
  uint32_t blocks_written = 0;
  // Blank RAM sector to all 1s
  memset(sector, 0xFF, sizeof(*sector));
  printf2("Writing to Ram Sector...\n");
  uart0::PutChar(kHyperloadReady);
  while (blocks_written < kBlocksPerSector)
  {
    uint8_t block_number_msb = uart0::GetChar(1000);
    uint8_t block_number_lsb = uart0::GetChar(100);
    uint32_t block_number    = (block_number_msb << 8) | block_number_lsb;
    if (0xFFFF == block_number)
    {
      uart2::Puts("End Of Blocks\n");
      finished = true;
      break;
    }
    else
    {
      uint32_t partition        = block_number % kBlocksPerSector;
      uint8_t checksum          = WriteUartToBlock(&sector->block[partition]);
      uint8_t expected_checksum = uart0::GetChar(1000);
      if (checksum != expected_checksum)
      {
        uart0::PutChar(kChecksumError);
      }
      else
      {
        blocks_written++;
        if (blocks_written < 8)
        {
          uart0::PutChar(kHyperloadReady);
        }
      }
    }
  }
  return finished;
}

IapResult PrepareSector(uint32_t start, uint32_t end)
{
  IapCommand_t command  = { 0, { 0 } };
  IapStatus_t status    = { IapResult(0), { 0 } };
  command.command       = IapCommands::kPrepareFlash;
  command.parameters[0] = start;
  command.parameters[1] = end;
  iap(&command, &status);
  return status.result;
}

IapResult EraseSector(uint32_t start, uint32_t end)
{
  IapCommand_t command   = { 0, { 0 } };
  IapStatus_t status     = { IapResult(0), { 0 } };
  IapResult flash_status = PrepareSector(start, end);
  if (flash_status == IapResult::kCmdSuccess)
  {
    command.command       = IapCommands::kEraseSector;
    command.parameters[0] = start;
    command.parameters[1] = end;
    command.parameters[2] = config::kSystemClockRate / 1000;
    iap(&command, &status);
  }
  else
  {
    status.result = flash_status;
  }
  return status.result;
}

IapResult FlashBlock(Block_t * block, uint32_t sector_number,
                     uint32_t block_number)
{
  IapCommand_t command   = { 0, { 0 } };
  IapStatus_t status     = { IapResult(0), { 0 } };
  IapResult flash_status = PrepareSector(sector_number, sector_number);
  uint32_t app_sector    = sector_number - kStartSector;
  Block_t * flash_address =
      &(flash->application[app_sector].block[block_number]);
  if (flash_status == IapResult::kCmdSuccess)
  {
    command.command       = IapCommands::kCopyRamToFlash;
    command.parameters[0] = reinterpret_cast<intptr_t>(flash_address);
    command.parameters[1] = reinterpret_cast<intptr_t>(block);
    command.parameters[2] = kBlockSize;
    command.parameters[3] = config::kSystemClockRate / 1000;
    iap(&command, &status);
    printf2("Flash Attempted! %p %s\n", flash_address,
            kIapResultString[static_cast<uint32_t>(status.result)]);
  }
  else
  {
    printf2("Flash Failed Preperation 0x%lX!\n",
            kIapResultString[static_cast<uint32_t>(flash_status)]);
    status.result = flash_status;
  }
  return status.result;
}

IapResult VerifySector(Sector_t * ram_sector, uint32_t sector_number)
{
  IapCommand_t command    = { 0, { 0 } };
  IapStatus_t status      = { IapResult(0), { 0 } };
  command.command         = IapCommands::kCompare;
  uint32_t app_sector     = sector_number - kStartSector;
  Block_t * flash_address = &(flash->application[app_sector].block[0]);
  command.parameters[0]   = reinterpret_cast<intptr_t>(ram_sector);
  command.parameters[1]   = reinterpret_cast<intptr_t>(flash_address);
  command.parameters[2]   = kSectorSize;
  iap(&command, &status);
  return status.result;
}

IapResult BlankCheckSector(uint32_t start, uint32_t end)
{
  IapCommand_t command  = { 0, { 0 } };
  IapStatus_t status    = { IapResult(0), { 0 } };
  command.command       = IapCommands::kBlankCheckSector;
  command.parameters[0] = start;
  command.parameters[1] = end;
  iap(&command, &status);
  return status.result;
}

void EraseWithVerifySector(uint32_t sector_number)
{
  printf2("Erasing Flash...\n");
  Delay(kFlashDelay);
  IapResult erase_sector_result, black_check_result;
  do
  {
    erase_sector_result = EraseSector(sector_number, sector_number);
    black_check_result  = BlankCheckSector(sector_number, sector_number);
  } while (erase_sector_result != IapResult::kCmdSuccess ||
           black_check_result != IapResult::kCmdSuccess);
  printf2("Flash Erased and Verified!\n");
}

IapResult FlashSector(Sector_t * ram_sector, uint32_t sector_number,
                      uint32_t blocks_filled_in_sector)
{
  printf2("Flashing Sector %d\n", sector_number);
  EraseWithVerifySector(sector_number);
  IapResult flash_verified = IapResult::kBusy;
  while (flash_verified != IapResult::kCmdSuccess)
  {
    for (uint32_t block_number = 0; block_number < blocks_filled_in_sector;
         block_number++)
    {
      IapResult block_flashed_successfully = FlashBlock(
          &ram_sector->block[block_number], sector_number, block_number);

      if (block_flashed_successfully != IapResult::kCmdSuccess)
      {
        uint8_t error = static_cast<uint8_t>(block_flashed_successfully);
        printf2("Flash Failed with Code 0x%X!\n", error);
        EraseWithVerifySector(sector_number);
        block_number = 0;
      }
      // Omitting this delay will cause a brown out due to the power
      // consumed during flash programming.
      // The number of blocks flashed without it would vary from 1 to the
      // full sector's amount.
      Delay(kFlashDelay);
    }
    flash_verified = VerifySector(ram_sector, sector_number);
  }
  printf2("Flash Programming Verified\n");
  return IapResult::kCmdSuccess;
}
