#include <array>
#include <cstdint>
#include <numeric>

#include "L0_Platform/stm32f10x/stm32f10x.h"
#include "L1_Peripheral/stm32f10x/system_controller.hpp"
#include "L4_Testing/testing_frameworks.hpp"
#include "utility/units.hpp"

namespace sjsu::stm32f10x
{
EMIT_ALL_METHODS(SystemController);

TEST_CASE("Testing stm32f10x SystemController", "[stm32f10x-systemcontroller]")
{
  std::array<const SystemController::PeripheralID *, 10> id = {
    // AHB
    &stm32f10x::SystemController::Peripherals::kFlitf,
    &stm32f10x::SystemController::Peripherals::kCrc,
    &stm32f10x::SystemController::Peripherals::kFsmc,
    // AHB1
    &stm32f10x::SystemController::Peripherals::kTimer14,
    &stm32f10x::SystemController::Peripherals::kWindowWatchdog,
    &stm32f10x::SystemController::Peripherals::kSpi2,
    // AHB2
    &stm32f10x::SystemController::Peripherals::kGpioG,
    &stm32f10x::SystemController::Peripherals::kAdc2,
    &stm32f10x::SystemController::Peripherals::kTimer1,
    &stm32f10x::SystemController::Peripherals::kAdc3,
  };

  std::array<volatile uint32_t, 3> local_enables;
  std::fill(local_enables.begin(), local_enables.end(), 0);

  stm32f10x::SystemController::enable[0] = &local_enables[0];
  stm32f10x::SystemController::enable[1] = &local_enables[1];
  stm32f10x::SystemController::enable[2] = &local_enables[2];

  SystemController test_subject;

  SECTION("GetSystemFrequency()")
  {
    CHECK(8_MHz <= test_subject.GetSystemFrequency());
  }

  SECTION("PowerUpPeripheral()")
  {
    for (size_t i = 0; i < id.size(); i++)
    {
      INFO("Failed on index: " << i);

      // Exercise
      test_subject.PowerUpPeripheral(*id[i]);

      // Verify
      auto enable_word = local_enables[id[i]->device_id / 32];
      CHECK(bit::Read(enable_word, id[i]->device_id % 32));
    }
  }

  SECTION("PowerDownPeripheral()")
  {
    // Setup
    constexpr auto kAllOnes = std::numeric_limits<uint32_t>::max();
    // Setup: Set every bit in the enables to 1 indicating that the all
    // peripherals are enabled. The PowerDownPeripheral() should clear these
    // bits to 1.
    std::fill(local_enables.begin(), local_enables.end(), kAllOnes);

    for (size_t i = 0; i < id.size(); i++)
    {
      INFO("Failed on index: " << i);

      // Exercise
      test_subject.PowerDownPeripheral(*id[i]);

      // Verify
      auto enable_word = local_enables[id[i]->device_id / 32];
      CHECK(!bit::Read(enable_word, id[i]->device_id % 32));
    }
  }

  SECTION("IsPeripheralPoweredUp()")
  {
    // Setup: Map of expected powered on and off peripherals
    std::array<bool, id.size()> is_set = {
      true,   // [0]
      true,   // [1]
      false,  // [2]
      true,   // [3]
      true,   // [4]
      true,   // [5]
      false,  // [6]
      true,   // [7]
      false,  // [8]
      true,   // [9]
    };

    for (uint32_t i = 0; i < is_set.size(); i++)
    {
      auto & enable_word = local_enables[id[i]->device_id / 32];
      enable_word =
          bit::Insert(enable_word, is_set[i], id[i]->device_id % 32, 1);
    }

    for (size_t i = 0; i < id.size(); i++)
    {
      INFO("Failed on index: " << i);
      // Exercise + Verify
      CHECK(is_set[i] == test_subject.IsPeripheralPoweredUp(*id[i]));
    }
  }

  stm32f10x::SystemController::enable[0] = &RCC->AHBENR;
  stm32f10x::SystemController::enable[1] = &RCC->APB1ENR;
  stm32f10x::SystemController::enable[2] = &RCC->APB2ENR;
}
}  // namespace sjsu::stm32f10x
