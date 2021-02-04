#include "peripherals/stm32f4xx/system_controller.hpp"

#include <array>
#include <cstdint>
#include <numeric>

#include "platforms/targets/stm32f4xx/stm32f4xx.h"
#include "testing/testing_frameworks.hpp"
#include "utility/math/units.hpp"

namespace sjsu::stm32f4xx
{
TEST_CASE("Testing stm32f4xx SystemController")
{
  std::array<const SystemController::ResourceID *, 13> id = {
    &stm32f4xx::SystemController::Peripherals::kGpioA,
    &stm32f4xx::SystemController::Peripherals::kGpioH,
    &stm32f4xx::SystemController::Peripherals::kUsbOtgHS,
    // AHB2
    &stm32f4xx::SystemController::Peripherals::kCamera,
    &stm32f4xx::SystemController::Peripherals::kHash,
    &stm32f4xx::SystemController::Peripherals::kUsbOtgFs,
    // AHB3
    &stm32f4xx::SystemController::Peripherals::kFlexStaticMemory,
    // APB1
    &stm32f4xx::SystemController::Peripherals::kTimer2,
    &stm32f4xx::SystemController::Peripherals::kUart4,
    &stm32f4xx::SystemController::Peripherals::kDac,
    // APB2
    &stm32f4xx::SystemController::Peripherals::kTimer1,
    &stm32f4xx::SystemController::Peripherals::kAdc3,
    &stm32f4xx::SystemController::Peripherals::kTimer11,
  };

  std::array<volatile uint32_t, 5> local_enables;

  stm32f4xx::SystemController::enable[0] = &local_enables[0];
  stm32f4xx::SystemController::enable[1] = &local_enables[1];
  stm32f4xx::SystemController::enable[2] = &local_enables[2];
  stm32f4xx::SystemController::enable[3] = &local_enables[3];
  stm32f4xx::SystemController::enable[4] = &local_enables[4];

  SystemController test_subject;

  SECTION("GetClockRate()")
  {
    CHECK(16_MHz <= test_subject.GetClockRate(
                        stm32f4xx::SystemController::Peripherals::kCpu));
  }

  SECTION("PowerUpPeripheral()")
  {
    // Setup
    std::fill(local_enables.begin(), local_enables.end(), 0);

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
    // Setup
    std::fill(local_enables.begin(), local_enables.end(), 0);

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
      true,   // [10]
      false,  // [11]
      false,  // [12]
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

  stm32f4xx::SystemController::enable[0] = &RCC->AHB1ENR;
  stm32f4xx::SystemController::enable[1] = &RCC->AHB2ENR;
  stm32f4xx::SystemController::enable[2] = &RCC->AHB3ENR;
  stm32f4xx::SystemController::enable[3] = &RCC->APB1ENR;
  stm32f4xx::SystemController::enable[4] = &RCC->APB2ENR;
}
}  // namespace sjsu::stm32f4xx
