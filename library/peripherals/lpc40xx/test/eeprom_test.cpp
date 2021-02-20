/// This is the eeprom.cpp test file

#include "peripherals/lpc40xx/eeprom.hpp"

#include "testing/testing_frameworks.hpp"

namespace sjsu::lpc40xx
{
TEST_CASE("Testing EEPROM")
{
  // Simulate local version of LPC_EEPROM
  LPC_EEPROM_TypeDef local_eeprom;

  // Clear memory locations and assign eeprom_register to local variable
  testing::ClearStructure(&local_eeprom);
  Eeprom::eeprom_register = &local_eeprom;

  // Set mock for sjsu::SystemController
  constexpr units::frequency::hertz_t kDummySystemClockFrequency = 48_MHz;
  Mock<sjsu::SystemController> mock_system_controller;
  When(Method(mock_system_controller, GetClockRate))
      .AlwaysReturn(kDummySystemClockFrequency);
  sjsu::SystemController::SetPlatformController(&mock_system_controller.get());

  // Creating test EEPROM object
  Eeprom test_eeprom;

  // Write/Read tests only check one 32-bit transfer, so size is 4 bytes
  constexpr size_t kPayloadSize = 4;

  SECTION("Initialization")
  {
    // Setup
    // Setup: Wait State Register Values should be 2, 3, and 1
    constexpr uint32_t kWaitStateValues = 0b00000010'00000011'00000001;
    constexpr uint32_t kClockDivider    = 128;

    // Exercise
    test_eeprom.Initialize();

    // Verify
    CHECK(local_eeprom.WSTATE == kWaitStateValues);
    CHECK(local_eeprom.CLKDIV == kClockDivider);
    CHECK(local_eeprom.PWRDWN == 0);
  }

  SECTION("PowerDown")
  {
    // Setup
    test_eeprom.Initialize();
    local_eeprom.PWRDWN = 0;

    // Exercise
    test_eeprom.PowerDown();

    // Verify
    CHECK(local_eeprom.PWRDWN == 1);
  }

  SECTION("Writing")
  {
    // Setup
    std::array<uint8_t, kPayloadSize> wdata;
    constexpr uint16_t kAddress = 0x3F4;
    wdata[0]                    = 0b11110000;
    wdata[1]                    = 0b00001111;
    wdata[2]                    = 0b10101010;
    wdata[3]                    = 0b01010101;

    const uint32_t kExpectedValue =
        (wdata[3] << 24) + (wdata[2] << 16) + (wdata[1] << 8) + (wdata[0]);

    // Setup: Status register bits must be set to 1 so that Write doesn't block
    local_eeprom.INT_STATUS = (1 << 26) | (1 << 28);

    // Exercise
    test_eeprom.Write(kAddress, wdata);

    // Verify
    CHECK(local_eeprom.ADDR == kAddress);
    CHECK(local_eeprom.CMD == Eeprom::kEraseProgram);
    CHECK(local_eeprom.WDATA == kExpectedValue);
  }

  SECTION("Reading")
  {
    // Setup
    constexpr uint16_t kAddress = 0x534;
    constexpr uint32_t kReadVal = 0x12345678;
    std::array<uint8_t, kPayloadSize> rdata;
    // Setup: Placing value in register which read function will be reading from
    local_eeprom.RDATA = kReadVal;

    // Exercise
    test_eeprom.Read(kAddress, rdata);

    // Verify
    uint32_t read_value =
        (rdata[3] << 24) + (rdata[2] << 16) + (rdata[1] << 8) + (rdata[0]);
    CHECK(local_eeprom.ADDR == kAddress);
    CHECK(local_eeprom.CMD == Eeprom::kRead32Bits);
    CHECK(read_value == kReadVal);
  }

  SECTION("Methods that return constants")
  {
    CHECK(test_eeprom.GetBlockSize() == 4_B);
    CHECK(test_eeprom.GetMemoryType() == Storage::Type::kEeprom);
    CHECK(test_eeprom.GetCapacity() == 4_kB);
    CHECK(test_eeprom.IsReadOnly() == false);
    CHECK(test_eeprom.IsMediaPresent() == true);
  }

  // Reset eeprom_register back to original value
  // in case future tests depend on it
  Eeprom::eeprom_register = LPC_EEPROM;
}
}  // namespace sjsu::lpc40xx
