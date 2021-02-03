#include "devices/memory/sd.hpp"

#include "testing/testing_frameworks.hpp"

namespace sjsu::experimental
{
TEST_CASE("Testing SD Card Driver Class")
{
  Mock<sjsu::Spi> mock_spi;
  Mock<sjsu::Gpio> mock_card_detect;
  Mock<sjsu::Gpio> mock_chip_select;

  Fake(Method(mock_chip_select, Gpio::ModuleInitialize),
       Method(mock_chip_select, Gpio::SetDirection),
       Method(mock_chip_select, Gpio::Set));

  Fake(Method(mock_card_detect, Gpio::ModuleInitialize),
       Method(mock_card_detect, Gpio::SetDirection),
       Method(mock_card_detect, Gpio::Set));

  Fake(Method(mock_spi, Spi::ModuleInitialize));
  Fake(OverloadedMethod(
      mock_spi, Spi::Transfer, void(std::span<uint16_t> buffer)));

  Sd sd(mock_spi.get(), mock_chip_select.get(), mock_card_detect.get());

  SECTION("GetMemoryType()")
  {
    // Verify
    CHECK(sd.GetMemoryType() == Storage::Type::kSD);
  }

  // SECTION("Initialize()")
  // {
  //   // Exercise
  //   sd.ModuleInitialize();

  //   // Verify
  //   Verify(Method(mock_chip_select, ModuleInitialize));
  //   Verify(Method(mock_card_detect, ModuleInitialize));

  //   Verify(Method(mock_spi, ModuleInitialize));
  //   CHECK(mock_spi.get().CurrentSettings().clock_rate == 400_kHz);
  //   CHECK(mock_spi.get().CurrentSettings().frame_size ==
  //         SpiSettings_t::FrameSize::kEightBits);

  //   Verify(
  //       Method(mock_chip_select,
  //       SetDirection).Using(Gpio::Direction::kOutput),
  //       Method(mock_chip_select, Set).Using(Gpio::State::kHigh),
  //       Method(mock_card_detect,
  //       SetDirection).Using(Gpio::Direction::kInput));
  // }

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
