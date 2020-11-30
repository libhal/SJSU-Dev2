#include "L1_Peripheral/stm32f10x/uart.hpp"

#include <algorithm>
#include <numeric>
#include <thread>

#include "L4_Testing/testing_frameworks.hpp"

namespace sjsu::stm32f10x
{
EMIT_ALL_METHODS(UartBase);
TEST_CASE("Testing stm32f10x Uart")
{
  static constexpr units::frequency::hertz_t kDummyClockRate = 8_MHz;
  Mock<sjsu::SystemController> mock_controller;
  Fake(Method(mock_controller, PowerUpPeripheral));
  When(Method(mock_controller, GetClockRate)).AlwaysReturn(kDummyClockRate);
  SystemController::SetPlatformController(&mock_controller.get());

  Mock<sjsu::Pin> mock_rx;
  Mock<sjsu::Pin> mock_tx;
  Mock<sjsu::Pin> mock_pin;

  Fake(Method(mock_rx, ConfigurePullResistor));
  Fake(Method(mock_rx, ConfigureFunction));

  Fake(Method(mock_tx, ConfigurePullResistor));
  Fake(Method(mock_tx, ConfigureFunction));

  DMA_Channel_TypeDef local_dma;
  USART_TypeDef local_usart;

  testing::ClearStructure(&local_usart);
  testing::ClearStructure(&local_dma);

  UartBase::Port_t mock_port = {
    .tx   = mock_tx.get(),
    .rx   = mock_rx.get(),
    .uart = &local_usart,
    .id   = SystemController::Peripherals::kUsart1,
    .dma  = &local_dma,
  };

  std::array<uint8_t, 8> receieve_queue;

  UartBase test_subject(mock_port, receieve_queue);

  SECTION("Initialize()")
  {
    // Exercise
    test_subject.Initialize();

    // Verify
    Verify(Method(mock_controller, PowerUpPeripheral).Using(mock_port.id))
        .Once();
    Verify(Method(mock_controller, PowerUpPeripheral)
               .Using(SystemController::Peripherals::kDma1))
        .Once();
  }

  SECTION("Enable(true)")
  {
    // Setup
    auto data_address_int  = reinterpret_cast<intptr_t>(&local_usart.DR);
    auto queue_address_int = reinterpret_cast<intptr_t>(receieve_queue.data());
    auto data_address      = static_cast<uint32_t>(data_address_int);
    auto queue_address     = static_cast<uint32_t>(queue_address_int);

    // Exercise
    test_subject.SetStateToInitialized();
    test_subject.Enable();

    // Verify
    Verify(Method(mock_rx, ConfigurePullResistor)
               .Using(sjsu::Pin::Resistor::kPullUp))
        .Once();
    Verify(Method(mock_tx, ConfigureFunction).Using(1)).Once();

    CHECK(receieve_queue.size() == local_dma.CNDTR);
    CHECK(data_address == local_dma.CPAR);
    CHECK(queue_address == local_dma.CMAR);
    CHECK(UartBase::kDmaSettings == local_dma.CCR);
    CHECK(UartBase::ControlReg::kControlSettings1 == local_usart.CR1);
    CHECK(UartBase::ControlReg::kControlSettings3 == local_usart.CR3);
  }

  SECTION("ConfigureBaudRate()")
  {
    // Setup
    uint32_t baud_rate;
    SUBCASE("baud rate 0")
    {
      baud_rate = 1200;
    }

    SUBCASE("baud rate 1")
    {
      baud_rate = 2400;
    }

    SUBCASE("baud rate 2")
    {
      baud_rate = 4800;
    }

    SUBCASE("baud rate 3")
    {
      baud_rate = 9600;
    }

    SUBCASE("baud rate 4")
    {
      baud_rate = 14400;
    }

    SUBCASE("baud rate 5")
    {
      baud_rate = 19200;
    }

    SUBCASE("baud rate 6")
    {
      baud_rate = 38400;
    }

    SUBCASE("baud rate 7")
    {
      baud_rate = 57600;
    }

    SUBCASE("baud rate 8")
    {
      baud_rate = 115200;
    }

    SUBCASE("baud rate 9")
    {
      baud_rate = 128000;
    }

    SUBCASE("baud rate 10")
    {
      baud_rate = 256000;
    }

    SUBCASE("baud rate 11")
    {
      baud_rate = 500000;
    }

    INFO("Failure for baud rate " << baud_rate);

    float float_baud_rate   = static_cast<float>(baud_rate);
    float freq              = static_cast<float>(kDummyClockRate);
    float usart_divider     = freq / (16.0f * float_baud_rate);
    uint16_t mantissa       = static_cast<uint16_t>(usart_divider);
    float fraction          = static_cast<float>(usart_divider - mantissa);
    uint16_t fractional_int = static_cast<uint16_t>(std::roundf(fraction * 16));

    if (fractional_int >= 16)
    {
      mantissa       = static_cast<uint16_t>(mantissa + 1U);
      fractional_int = 0;
    }

    // Exercise
    test_subject.ConfigureBaudRate(baud_rate);

    // Verify
    bit::Register baud_reg(&local_usart.BRR);
    CHECK(mantissa == baud_reg.Extract(UartBase::BaudRateReg::kMantissa));
    CHECK(fractional_int == baud_reg.Extract(UartBase::BaudRateReg::kFraction));
  }

  SECTION("Write()")
  {
    // Setup
    // Setup: Payload to be written to the uart port.
    std::array<uint8_t, 7> payload;

    // Setup: Fill the receive queue with letters from 'a' to 'a'+8
    std::iota(payload.begin(), payload.end(), 'a');

    // Setup: Generate test cases for each number of payload lengths
    uint32_t payload_size;

    SUBCASE("1")
    {
      payload_size = 1;
    }

    SUBCASE("2")
    {
      payload_size = 2;
    }

    SUBCASE("3")
    {
      payload_size = 3;
    }

    SUBCASE("4")
    {
      payload_size = 4;
    }

    SUBCASE("5")
    {
      payload_size = 5;
    }

    SUBCASE("6")
    {
      payload_size = 6;
    }

    SUBCASE("7")
    {
      payload_size = 7;
    }

    INFO("Failure with payload size = " << payload_size);

    // Setup: Make sure that the Write sees that the transmit is not currently
    //        empty, meaning it must wait.
    testing::PollingVerification({
        .locking_function =
            [&local_usart]() {
              bit::Register(&local_usart.SR)
                  .Clear(UartBase::StatusReg::kTransitEmpty)
                  .Save();
            },
        .polling_function =
            [&test_subject, &local_usart, &payload, &payload_size]() {
              // Make sure that the DR register has not been populated with any
              // data from the payload.
              REQUIRE(0 == local_usart.DR);
              test_subject.Write(
                  std::span<const uint8_t>(payload.data(), payload_size));
            },
        .release_function =
            [&local_usart]() {
              // Now set the transmit empty flag to allow byte transmission to
              // begin.
              bit::Register(&local_usart.SR)
                  .Set(UartBase::StatusReg::kTransitEmpty)
                  .Save();
            },
    });

    // Exercise
    // Verify
    // Verify: The last byte in the payload, based on the value of
    //         `payload_size` should show up here.
    CHECK(local_usart.DR == payload[payload_size - 1]);
  }

  SECTION("Read()")
  {
    // Setup
    std::array<uint8_t, 8> results;

    // Setup: fill results with zeros.
    std::fill(results.data(), results.end(), 0);

    // Setup: Generate test cases for each number of stored bytes
    uint32_t stored_bytes;

    SUBCASE("1")
    {
      stored_bytes = 1;
    }

    SUBCASE("2")
    {
      stored_bytes = 2;
    }

    SUBCASE("3")
    {
      stored_bytes = 3;
    }

    SUBCASE("4")
    {
      stored_bytes = 4;
    }

    SUBCASE("5")
    {
      stored_bytes = 5;
    }

    SUBCASE("6")
    {
      stored_bytes = 6;
    }

    SUBCASE("7")
    {
      stored_bytes = 7;
    }

    // Setup: fill receieve_queue with zeros.
    std::fill(receieve_queue.data(), receieve_queue.end(), 0);

    // Setup: Fill the receive queue with letters from 'a' to 'a'+8
    std::iota(receieve_queue.begin(), &receieve_queue[stored_bytes], 'a');

    // Setup: Set the DMA length position back by the receive_queue length to
    //        store the data into
    local_dma.CNDTR = receieve_queue.size() - stored_bytes;

    // Exercise
    test_subject.Read(std::span<uint8_t>(results.data(), stored_bytes));

    // Verify
    CHECK(receieve_queue == results);
  }

  SECTION("HasData()")
  {
    // Assumption that UartBase default initializes its read position position.
    // If this is the case, then we can change the DMA location index to test if
    // HasData() is working.
    SECTION("False")
    {
      // Setup: Set DMA index equal to size of the receive queue, meaning that
      //        the DMA has not processed any bytes over uart.
      local_dma.CNDTR = receieve_queue.size();

      // Exercise + Verify
      CHECK(!test_subject.HasData());
    }

    SECTION("True")
    {
      // Setup: Set DMA index equal to size of the receive queue - 5, meaning
      //        that the DMA has moved 5 bytes over uart. And since the `Read()`
      //        method has yet to be run, HasData() should return true.
      local_dma.CNDTR = receieve_queue.size() - 5;

      // Exercise + Verify
      CHECK(test_subject.HasData());
    }
  }

  SECTION("Flush()")
  {
    // Setup assumptions are the same as HasData()

    // Setup: Set DMA index equal to size of the receive queue - 5, meaning
    //        that the DMA has moved 5 bytes over uart. And since the `Read()`
    //        method has yet to be run, HasData() should return true.
    local_dma.CNDTR = receieve_queue.size() - 5;
    REQUIRE(test_subject.HasData());

    // Exercise
    test_subject.Flush();

    // Verify
    CHECK(!test_subject.HasData());
  }

  SECTION("~UartBase()")
  {
    // Setup
    bit::Register(&local_dma.CCR)
        .Set(Dma::Reg::kEnable)
        .Save();

    bit::Register(&local_usart.CR3)
        .Set(UartBase::ControlReg::kDmaReceiverEnable)
        .Save();

    bit::Register(&local_usart.CR1)
        .Set(UartBase::ControlReg::kUsartEnable)
        .Save();

    // Exercise
    test_subject.~UartBase();

    // Verify
    CHECK(!bit::Register(&local_dma.CCR)
               .Read(Dma::Reg::kEnable));
    CHECK(!bit::Register(&local_usart.CR3)
               .Read(UartBase::ControlReg::kDmaReceiverEnable));
    CHECK(!bit::Register(&local_usart.CR1)
               .Read(UartBase::ControlReg::kUsartEnable));
  }
}
}  // namespace sjsu::stm32f10x
