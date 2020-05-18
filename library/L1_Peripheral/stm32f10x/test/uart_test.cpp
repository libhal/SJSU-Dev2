#include <algorithm>
#include <thread>

#include "L1_Peripheral/stm32f10x/uart.hpp"
#include "L4_Testing/testing_frameworks.hpp"

namespace sjsu::stm32f10x
{
EMIT_ALL_METHODS(UartBase);
TEST_CASE("Testing stm32f10x Uart", "[stm32f10x-uart]")
{
  static constexpr units::frequency::hertz_t kDummyClockRate = 8_MHz;
  Mock<sjsu::SystemController> mock_controller;
  Fake(Method(mock_controller, PowerUpPeripheral));
  When(Method(mock_controller, GetClockRate)).AlwaysReturn(kDummyClockRate);
  SystemController::SetPlatformController(&mock_controller.get());

  Mock<sjsu::Pin> mock_rx;
  Mock<sjsu::Pin> mock_tx;
  Mock<sjsu::Pin> mock_pin;

  Fake(Method(mock_rx, SetPull));
  Fake(Method(mock_tx, SetPinFunction));

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
    // Setup
    static constexpr uint32_t kExpectedBaudRate = 115200;

    Mock<UartBase> spy(test_subject);
    Fake(Method(spy, SetBaudRate));

    intptr_t data_address_int = reinterpret_cast<intptr_t>(&local_usart.DR);
    intptr_t queue_address_int =
        reinterpret_cast<intptr_t>(receieve_queue.data());

    auto data_address  = static_cast<uint32_t>(data_address_int);
    auto queue_address = static_cast<uint32_t>(queue_address_int);

    // Exercise
    test_subject.Initialize(kExpectedBaudRate);

    // Verify
    Verify(Method(spy, SetBaudRate).Using(kExpectedBaudRate)).Once();
    Verify(Method(mock_controller, PowerUpPeripheral).Using(mock_port.id))
        .Once();
    Verify(Method(mock_controller, PowerUpPeripheral)
               .Using(SystemController::Peripherals::kDma1))
        .Once();
    Verify(Method(mock_rx, SetPull).Using(sjsu::Pin::Resistor::kPullUp)).Once();
    Verify(Method(mock_tx, SetPinFunction).Using(1)).Once();

    CHECK(receieve_queue.size() == local_dma.CNDTR);
    CHECK(data_address == local_dma.CPAR);
    CHECK(queue_address == local_dma.CMAR);
    CHECK(UartBase::DmaReg::kDmaSettings == local_dma.CCR);
    CHECK(UartBase::ControlReg::kControlSettings1 == local_usart.CR1);
    CHECK(UartBase::ControlReg::kControlSettings3 == local_usart.CR3);
  }

  SECTION("SetBaudRate()")
  {
    // Setup
    uint32_t baud_rate = GENERATE(1200, 2400, 4800, 9600, 14400, 19200, 38400,
                                  57600, 115200, 128000, 256000, 500000);

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
    test_subject.SetBaudRate(baud_rate);

    // Verify
    bit::Register baud_reg(&local_usart.BRR);
    CHECK(mantissa == baud_reg.Extract(UartBase::BaudRateReg::kMantissa));
    CHECK(fractional_int == baud_reg.Extract(UartBase::BaudRateReg::kFraction));
  }

  SECTION("Write()")
  {
    // Setup
    // Setup: Payload to be written to the uart port.
    std::array<char, 7> payload;

    // Setup: Fill the receive queue with letters from 'a' to 'a'+8
    std::iota(payload.begin(), payload.end(), 'a');

    // Setup: Generate test cases for each number of payload lengths
    uint32_t payload_size = GENERATE(1, 2, 3, 4, 5, 6, 7);

    INFO("Failure with payload size = " << payload_size)

    // Setup: Make sure that the Write sees that the transmit is not currently
    //        empty, meaning it must wait.
    bit::Register(&local_usart.SR)
        .Clear(UartBase::StatusReg::kTransitEmpty)
        .Save();

    std::thread initial_transmit_wait([&local_usart]() {
      // Allow some time to pass
      std::this_thread::sleep_for(1us);

      // Make sure that the DR register has not been populated with any data
      // from the payload.
      REQUIRE(0 == local_usart.DR);

      // Allow some time to pass
      std::this_thread::sleep_for(1us);

      // Now set the transmit empty flag to allow byte transmission to begin.
      bit::Register(&local_usart.SR)
          .Set(UartBase::StatusReg::kTransitEmpty)
          .Save();
    });

    // Exercise
    test_subject.Write(payload.data(), payload_size);
    initial_transmit_wait.join();

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
    uint32_t stored_bytes = GENERATE(1, 2, 3, 4, 5, 6, 7);

    // Setup: fill receieve_queue with zeros.
    std::fill(receieve_queue.data(), receieve_queue.end(), 0);

    // Setup: Fill the receive queue with letters from 'a' to 'a'+8
    std::iota(receieve_queue.begin(), &receieve_queue[stored_bytes], 'a');

    // Setup: Set the DMA length position back by the receive_queue length to
    //        store the data into
    local_dma.CNDTR = receieve_queue.size() - stored_bytes;

    // Exercise
    test_subject.Read(results.data(), stored_bytes);

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
        .Set(UartBase::UartBase::DmaReg::kEnable)
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
               .Read(UartBase::UartBase::DmaReg::kEnable));
    CHECK(!bit::Register(&local_usart.CR3)
               .Read(UartBase::ControlReg::kDmaReceiverEnable));
    CHECK(!bit::Register(&local_usart.CR1)
               .Read(UartBase::ControlReg::kUsartEnable));
  }
}
}  // namespace sjsu::stm32f10x
