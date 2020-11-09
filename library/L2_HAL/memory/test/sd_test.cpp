#include "L2_HAL/memory/sd.hpp"

#include "L4_Testing/testing_frameworks.hpp"

namespace sjsu::experimental
{
EMIT_ALL_METHODS(Sd);

TEST_CASE("Testing SD Card Driver Class")
{
  Mock<sjsu::Spi> mock_spi;
  Mock<sjsu::Gpio> mock_card_detect;
  Mock<sjsu::Gpio> mock_chip_select;

  Fake(Method(mock_chip_select, ModuleInitialize),
       Method(mock_chip_select, ModuleEnable),
       Method(mock_chip_select, SetDirection),
       Method(mock_chip_select, Set));

  Fake(Method(mock_card_detect, ModuleInitialize),
       Method(mock_card_detect, ModuleEnable),
       Method(mock_card_detect, SetDirection),
       Method(mock_card_detect, Set));

  Fake(Method(mock_spi, ModuleInitialize),
       Method(mock_spi, ModuleEnable),
       Method(mock_spi, ConfigureFrameSize),
       Method(mock_spi, ConfigureClockMode),
       Method(mock_spi, ConfigureFrequency));

  Sd sd(mock_spi.get(), mock_chip_select.get(), mock_card_detect.get());

  SECTION("GetMemoryType()")
  {
    // Verify
    CHECK(sd.GetMemoryType() == Storage::Type::kSD);
  }

  SECTION("Initialize()")
  {
    // Exercise
    sd.ModuleInitialize();

    // Verify
    Verify(Method(mock_chip_select, ModuleInitialize));
    Verify(Method(mock_card_detect, ModuleInitialize));

    Verify(Method(mock_spi, ModuleInitialize));
    Verify(
        Method(mock_spi, ConfigureFrameSize).Using(Spi::FrameSize::kEightBits));
    Verify(Method(mock_spi, ConfigureClockMode));
    Verify(Method(mock_spi, ConfigureFrequency).Using(400_kHz));
    Verify(Method(mock_spi, ModuleEnable).Using(true));

    Verify(Method(mock_chip_select, ModuleEnable));
    Verify(Method(mock_card_detect, ModuleEnable));

    Verify(
        Method(mock_chip_select, SetDirection).Using(Gpio::Direction::kOutput));
    Verify(Method(mock_chip_select, Set).Using(Gpio::State::kHigh));
    Verify(
        Method(mock_card_detect, SetDirection).Using(Gpio::Direction::kInput));
  }

  SECTION("IsMediaPresent()")
  {
    SECTION("Active Low")
    {
      SECTION("Is Present")
      {
        // Setup: false indicates a low voltage, as the pin signals are all
        // active high by default.
        When(Method(mock_card_detect, Read)).Return(false);

        // Exercise
        bool media_is_present = sd.IsMediaPresent();

        // Verify
        CHECK(media_is_present);
      }

      SECTION("Is NOT Present")
      {
        // Setup: true indicates a high voltage, as the pin signals are all
        // active high by default.
        When(Method(mock_card_detect, Read)).Return(true);

        // Exercise
        bool media_is_present = sd.IsMediaPresent();

        // Verify
        CHECK(!media_is_present);
      }
    }

    SECTION("Active High")
    {
      sjsu::Sd active_low_sd(mock_spi.get(),
                             mock_chip_select.get(),
                             mock_card_detect.get(),
                             sd.kDefaultSpiFrequency,
                             sjsu::Gpio::State::kHigh);

      SECTION("Is Present")
      {
        // Setup
        When(Method(mock_card_detect, Read)).Return(true);

        // Exercise
        bool media_is_present = active_low_sd.IsMediaPresent();

        // Verify
        CHECK(media_is_present);
      }

      SECTION("Is NOT Present")
      {
        // Setup
        When(Method(mock_card_detect, Read)).Return(false);

        // Exercise
        bool media_is_present = active_low_sd.IsMediaPresent();

        // Verify
        CHECK(!media_is_present);
      }
    }
  }

  SECTION("GetBlockSize()")
  {
    CHECK(512_B == sd.GetBlockSize());
  }

  SECTION("IsReadOnly()")
  {
    CHECK(false == sd.IsReadOnly());
  }

  SECTION("GetCapacity()")
  {
    // Setup
    Sd::CardInfo_t & card_info = sd.GetCardInfo();

    constexpr uint32_t kCSize = 0x003B37;

    // Setup: Bits [48:69] are C_SIZE register which contains
    card_info.csd.byte[7] = (kCSize >> 16) & 0xFF;
    card_info.csd.byte[8] = (kCSize >> 8) & 0xFF;
    card_info.csd.byte[9] = (kCSize >> 0) & 0xFF;

    const units::data::byte_t kExpectedSize = (kCSize + 1) * 512_KiB;

    // Exercise
    auto actual_size = sd.GetCapacity();

    // Verify
    INFO("Expected = " << kExpectedSize.to<float>()
                       << " :: Actual = " << actual_size.to<float>());
    CHECK(kExpectedSize == actual_size);
  }

  SECTION("Enable()")
  {
    // NOT TESTED!
  }

  SECTION("Read()")
  {
    // NOT TESTED!
  }

  SECTION("Write()")
  {
    // NOT TESTED!
  }
}  // namespace sjsu::experimental
}  // namespace sjsu::experimental
