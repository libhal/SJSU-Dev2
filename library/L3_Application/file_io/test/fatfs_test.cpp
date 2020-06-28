#include <diskio.h>
#include <ff.h>
#include <ffconf.h>

#include <cstdint>
#include <cstring>

#include "L4_Testing/testing_frameworks.hpp"
#include "L3_Application/file_io/fatfs.hpp"
#include "third_party/fakeit/fakeit.hpp"

// NOLINTNEXTLINE
extern "C" DSTATUS disk_status(BYTE drive_number);

// NOLINTNEXTLINE
extern "C" DSTATUS disk_initialize(BYTE drive_number);

// NOLINTNEXTLINE
extern "C" DRESULT disk_read(BYTE drive_number,
                             BYTE * buffer,
                             DWORD sector,
                             UINT count);

// NOLINTNEXTLINE
extern "C" DRESULT disk_write(BYTE drive_number,
                              const BYTE * buffer,
                              DWORD sector,
                              UINT count);
// NOLINTNEXTLINE
extern "C" DRESULT disk_ioctl(BYTE drive_number, BYTE command, void * buffer);

namespace sjsu
{
TEST_CASE("Testing FAT FS")
{
  using namespace fakeit;

  SECTION("Stringify")
  {
    CHECK(0 == strcmp(Stringify(FR_OK), "(0) Succeeded"));
    CHECK(0 ==
          strcmp(Stringify(FR_DISK_ERR),
                 "(1) A hard error occurred in the low level disk I/O layer"));
    CHECK(0 == strcmp(Stringify(FR_INT_ERR), "(2) Assertion failed"));
    CHECK(0 == strcmp(Stringify(FR_NOT_READY),
                      "(3) The physical drive cannot work"));
    CHECK(0 == strcmp(Stringify(FR_NO_FILE), "(4) Could not find the file"));
    CHECK(0 == strcmp(Stringify(FR_NO_PATH), "(5) Could not find the path"));
    CHECK(0 == strcmp(Stringify(FR_INVALID_NAME),
                      "(6) The path name format is invalid"));
    CHECK(
        0 ==
        strcmp(Stringify(FR_DENIED),
               "(7) Access denied due to prohibited access or directory full"));
    CHECK(0 == strcmp(Stringify(FR_EXIST),
                      "(8) Access denied due to prohibited access"));
    CHECK(0 == strcmp(Stringify(FR_INVALID_OBJECT),
                      "(9) The file/directory object is invalid"));
    CHECK(0 == strcmp(Stringify(FR_WRITE_PROTECTED),
                      "(10) The physical drive is write protected"));
    CHECK(0 == strcmp(Stringify(FR_INVALID_DRIVE),
                      "(11) The logical drive number is invalid"));
    CHECK(0 == strcmp(Stringify(FR_NOT_ENABLED),
                      "(12) The volume has no work area"));
    CHECK(0 == strcmp(Stringify(FR_NO_FILESYSTEM),
                      "(13) There is no valid FAT volume"));
    CHECK(0 == strcmp(Stringify(FR_MKFS_ABORTED),
                      "(14) The f_mkfs() aborted due to any problem"));
    CHECK(0 == strcmp(Stringify(FR_TIMEOUT),
                      "(15) Could not get a grant to access the volume within "
                      "defined period"));
    CHECK(0 == strcmp(Stringify(FR_LOCKED),
                      "(16) The operation is rejected according to the file "
                      "sharing policy"));
    CHECK(0 == strcmp(Stringify(FR_NOT_ENOUGH_CORE),
                      "(17) LFN working buffer could not be allocated"));
    CHECK(0 == strcmp(Stringify(FR_TOO_MANY_OPEN_FILES),
                      "(18) Number of open files > FF_FS_LOCK"));
    CHECK(0 == strcmp(Stringify(FR_INVALID_PARAMETER),
                      "(19) Given parameter is invalid"));
  }

  SECTION("disk_status()")
  {
    // Setup
    Mock<sjsu::Storage> mock_storage;
    RegisterFatFsDrive(&mock_storage.get());

    SECTION("Invalid drive number")
    {
      // Exercise + Verify
      CHECK(RES_PARERR == disk_status(5));
    }

    SECTION("Media is not Present")
    {
      // Setup
      When(Method(mock_storage, IsMediaPresent)).AlwaysReturn(false);

      // Exercise + Verify
      CHECK(STA_NODISK == disk_status(0));
    }

    SECTION("Storage is not initialized")
    {
      // Setup
      When(Method(mock_storage, IsMediaPresent)).AlwaysReturn(true);

      // Exercise + Verify
      CHECK(STA_NOINIT == disk_status(0));
    }
  }

  SECTION("disk_initialize()")
  {
    // Setup
    Mock<sjsu::Storage> mock_storage;
    RegisterFatFsDrive(&mock_storage.get());

    SECTION("Invalid drive number")
    {
      // Exercise + Verify
      CHECK(RES_PARERR == disk_initialize(5));
    }

    SECTION("Initialize Failed")
    {
      // Setup
      // Setup: Needed to verify disk_status's return value
      When(Method(mock_storage, IsMediaPresent)).AlwaysReturn(true);
      // Setup: The error returned is arbitrary
      When(Method(mock_storage, Initialize))
          .AlwaysReturn(
              Error(Status::kDeviceNotFound, "Initialize Failed for Testing"));
      When(Method(mock_storage, Enable)).AlwaysReturn({});

      // Exercise + Verify
      CHECK(STA_NOINIT == disk_initialize(0));
      CHECK(STA_NOINIT == disk_status(0));
    }

    SECTION("Initialize Successful + Enable Failed")
    {
      // Setup
      // Setup: Needed to verify disk_status's return value
      When(Method(mock_storage, IsMediaPresent)).AlwaysReturn(true);
      // Setup: The error returned is arbitrary
      When(Method(mock_storage, Initialize)).AlwaysReturn({});
      When(Method(mock_storage, Enable))
          .AlwaysReturn(
              Error(Status::kNotReadyYet, "Enable Failed for Testing"));

      // Exercise + Verify
      CHECK(STA_NOINIT == disk_initialize(0));
      CHECK(STA_NOINIT == disk_status(0));
    }

    SECTION("Initialize Successful + Enable Failed")
    {
      // Setup
      // Setup: Needed to verify disk_status's return value
      When(Method(mock_storage, IsMediaPresent)).AlwaysReturn(true);
      When(Method(mock_storage, Initialize)).AlwaysReturn({});
      When(Method(mock_storage, Enable)).AlwaysReturn({});

      // Exercise + Verify
      CHECK(RES_OK == disk_initialize(0));
      CHECK(RES_OK == disk_status(0));
    }
  }

  SECTION("disk_ioctl()")
  {
    // Setup + Exercise + Verify
    CHECK(RES_PARERR == disk_ioctl(0, 0, nullptr));
  }

  SECTION("RegisterFatFsDrive() Fails when driver number is out of bounds")
  {
    // Setup
    Mock<sjsu::Storage> mock_storage;

    // Exercise
    // Exercise: Take the drive count and add 1 to push it out of bounds.
    auto result =
        RegisterFatFsDrive(&mock_storage.get(), config::kFatDriveCount + 1);

    // Verify
    CHECK(!result.has_value());
  }

  SECTION("disk_write()")
  {
    // Setup
    Mock<sjsu::Storage> mock_storage;
    RegisterFatFsDrive(&mock_storage.get());

    SECTION("Invalid drive number")
    {
      // Exercise + Verify
      CHECK(RES_PARERR == disk_write(5, nullptr, 0, 1));
    }

    SECTION("Check various sector and count values")
    {
      // Setup
      uint8_t payload[] = { 1, 2, 3, 4, 5, 6 };

      When(Method(mock_storage, Write)).AlwaysReturn({});
      When(Method(mock_storage, Erase)).AlwaysReturn({});

      for (uint32_t sector : { 0, 1, 4, 512, 1024, 65536 })
      {
        for (uint32_t count : { 1, 2, 4, 8, 32, 64, 128 })
        {
          for (uint32_t block_size : { 1, 2, 4, 8, 512 })
          {
            // Setup
            INFO("Failure on sector: " << sector << ", count: " << count
                                       << ", block size: " << block_size);
            const uint32_t kExpectedSector = (sector * FF_MIN_SS) / block_size;
            const uint32_t kLength         = (count * FF_MIN_SS);
            auto result = units::data::byte_t{ static_cast<float>(block_size) };

            When(Method(mock_storage, GetBlockSize)).AlwaysReturn(result);

            // Exercise
            disk_write(0, payload, sector, count);

            // Verify
            Verify(Method(mock_storage, Erase).Using(kExpectedSector, kLength));
            Verify(Method(mock_storage, Write)
                       .Using(kExpectedSector, payload, kLength));
          }
        }
      }
    }

    SECTION("Storage::Erase() Failure")
    {
      // Setup
      uint8_t payload[] = { 1, 2, 3, 4, 5, 6 };
      When(Method(mock_storage, Erase))
          .AlwaysReturn(Error(Status::kBusError, "Erase Failed for Testing"));
      When(Method(mock_storage, Write)).AlwaysReturn({});
      When(Method(mock_storage, GetBlockSize)).AlwaysReturn(512_B);

      uint32_t sector     = 0;
      uint32_t block_size = 512;
      uint32_t count      = 5;

      const uint32_t kExpectedSector = (sector * FF_MIN_SS) / block_size;
      const uint32_t kLength         = (count * FF_MIN_SS);

      // Exercise
      auto disk_write_status = disk_write(0, payload, sector, count);

      // Verify
      CHECK(RES_ERROR == disk_write_status);

      Verify(Method(mock_storage, Erase).Using(kExpectedSector, kLength))
          .Once();
      // Verify: The write command must not be called at all in anyway.
      Verify(Method(mock_storage, Write)).Never();
    }

    SECTION("Storage::Write() Failure")
    {
      // Setup
      uint8_t payload[] = { 1, 2, 3, 4, 5, 6 };
      When(Method(mock_storage, Erase)).AlwaysReturn({});
      When(Method(mock_storage, Write))
          .AlwaysReturn(Error(Status::kBusError, "Write Failed for Testing"));
      When(Method(mock_storage, GetBlockSize)).AlwaysReturn(512_B);

      uint32_t sector     = 5;
      uint32_t block_size = 512;
      uint32_t count      = 2;

      const uint32_t kExpectedSector = (sector * FF_MIN_SS) / block_size;
      const uint32_t kLength         = (count * FF_MIN_SS);

      // Exercise
      auto disk_write_status = disk_write(0, payload, sector, count);

      // Verify
      CHECK(RES_ERROR == disk_write_status);

      Verify(Method(mock_storage, Erase).Using(kExpectedSector, kLength))
          .Once();
      Verify(
          Method(mock_storage, Write).Using(kExpectedSector, payload, kLength))
          .Once();
    }
  }

  SECTION("disk_read()")
  {
    // Setup
    Mock<sjsu::Storage> mock_storage;
    RegisterFatFsDrive(&mock_storage.get());

    SECTION("Invalid drive number")
    {
      // Exercise + Verify
      CHECK(RES_PARERR == disk_read(5, nullptr, 0, 1));
    }

    SECTION("Check various sector and count values")
    {
      // Setup
      uint8_t payload[512];

      When(Method(mock_storage, Read)).AlwaysReturn({});

      for (uint32_t sector : { 0, 1, 4, 512, 1024, 65536 })
      {
        for (uint32_t count : { 1, 2, 4, 8, 32, 64, 128 })
        {
          for (uint32_t block_size : { 1, 2, 4, 8, 512 })
          {
            // Setup
            INFO("Failure on sector: " << sector << ", count: " << count
                                       << ", block size: " << block_size);
            const uint32_t kExpectedSector = (sector * FF_MIN_SS) / block_size;
            const uint32_t kLength         = (count * FF_MIN_SS);
            auto result = units::data::byte_t{ static_cast<float>(block_size) };

            When(Method(mock_storage, GetBlockSize)).AlwaysReturn(result);

            // Exercise
            disk_read(0, payload, sector, count);

            // Verify
            Verify(Method(mock_storage, Read)
                       .Using(kExpectedSector,
                              reinterpret_cast<void *>(payload), kLength));
          }
        }
      }
    }

    SECTION("Storage::Read() Failure")
    {
      // Setup
      uint8_t payload[512];
      When(Method(mock_storage, Read))
          .AlwaysReturn(Error(Status::kBusError, "Read Failed for Testing"));
      When(Method(mock_storage, GetBlockSize)).AlwaysReturn(512_B);

      uint32_t sector     = 5;
      uint32_t block_size = 512;
      uint32_t count      = 2;

      const uint32_t kExpectedSector = (sector * FF_MIN_SS) / block_size;
      const uint32_t kLength         = (count * FF_MIN_SS);

      // Exercise
      auto disk_read_status = disk_read(0, payload, sector, count);

      // Verify
      CHECK(RES_ERROR == disk_read_status);
      Verify(Method(mock_storage, Read)
                 .Using(kExpectedSector, reinterpret_cast<void *>(payload),
                        kLength))
          .Once();
    }
  }
}
}  // namespace sjsu
