#include "peripherals/can.hpp"

#include <vector>

#include "testing/testing_frameworks.hpp"

namespace doctest
{
template <>
struct StringMaker<sjsu::Can::Message_t>
{
  static String convert(const sjsu::Can::Message_t & message)  // NOLINT
  {
    std::string str;

    str += "\nsjsu::Can::Message_t\n{\n";

    str += "  id: " + std::to_string(message.id);
    str += ",\n";
    str += "  is_remote_request: " + std::to_string(message.is_remote_request);
    str += ",\n";
    str += "  length: " + std::to_string(message.length);
    str += ",\n";
    str += "  format: " + std::to_string(static_cast<uint8_t>(message.format));
    str += ",\n";
    str += "  payload: { ";
    int i              = 0;
    int minimum_length = std::min(static_cast<int>(message.length),
                                  static_cast<int>(message.payload.size()));

    for (i = 0; i < minimum_length; i++)
    {
      str += std::to_string(message.payload[i]) + ", ";
    }

    str += "},\n";
    str += "}\n";

    String result(str.data(), str.size());

    return result;
  }
};
}  // namespace doctest

namespace sjsu
{
bool operator==(const Can::Message_t & lhs, const Can::Message_t & rhs)
{
  if (lhs.id != rhs.id)
  {
    return false;
  }

  if (lhs.is_remote_request != rhs.is_remote_request)
  {
    return false;
  }

  if (lhs.format != rhs.format)
  {
    return false;
  }

  if (lhs.length != rhs.length)
  {
    return false;
  }

  for (int i = 0; i < lhs.length; i++)
  {
    if (lhs.payload[i] != rhs.payload[i])
    {
      return false;
    }
  }
  return true;
}

bool operator!=(const Can::Message_t & lhs, const Can::Message_t & rhs)
{
  return !operator==(lhs, rhs);
}

TEST_CASE("Testing CAN Peripheral Interface")
{
  Mock<Can> mock_can;

  // Store messages passed to the virtual Send() in the saved_message variable.
  std::vector<Can::Message_t> saved_message;
  When(OverloadedMethod(mock_can, Can::Send, void(const Can::Message_t &)))
      .AlwaysDo([&saved_message](const Can::Message_t & can_message) {
        saved_message.push_back(can_message);
      });

  Can & test_subject = mock_can.get();

  SECTION("Send()>")
  {
    // Setup

    std::array<uint8_t, 9> exceeds_can_length = {
      0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99,
    };

    std::array<Can::Message_t, 5> expected_messages = {
      Can::Message_t{
          .id      = 0x144,
          .length  = 4,
          .payload = { 0x12, 0x5d, 0xbb, 0xaa },
      },
      Can::Message_t{
          .id      = 0x780,
          .length  = 7,
          .payload = { 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77 },
      },
      Can::Message_t{
          .id      = 0x781,
          .length  = 1,
          .payload = { 0xAA },
      },
      Can::Message_t{
          .id      = 0x781,
          .length  = 0,
          .payload = {},
      },
      Can::Message_t{
          .id      = 0x999,
          .length  = 8,
          .payload = { 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88 },
      },
    };

    // Exercise
    // Exercise: std::initializer_list<uint8_t>
    test_subject.Send(0x144, { 0x12, 0x5d, 0xbb, 0xaa });
    test_subject.Send(0x780, { 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77 });
    test_subject.Send(0x781, { 0xAA });
    test_subject.Send(0x781, {});

    // Exercise: std::span<uint8_t>
    test_subject.Send(0x144,
                      std::span(expected_messages[0].payload)
                          .subspan(0, expected_messages[0].length));
    test_subject.Send(0x780,
                      std::span(expected_messages[1].payload)
                          .subspan(0, expected_messages[1].length));
    test_subject.Send(0x781,
                      std::span(expected_messages[2].payload)
                          .subspan(0, expected_messages[2].length));
    test_subject.Send(0x781,
                      std::span(expected_messages[3].payload)
                          .subspan(0, expected_messages[3].length));

    // Exercise: This particular case exceeds the number of bytes allowed and
    //           will be truncated to fit the 8 byte payload size.
    test_subject.Send(0x999,
                      { 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99 });
    test_subject.Send(0x999, exceeds_can_length);

    // Verify
    CHECK(expected_messages[0] == saved_message[0]);
    CHECK(expected_messages[1] == saved_message[1]);
    CHECK(expected_messages[2] == saved_message[2]);
    CHECK(expected_messages[3] == saved_message[3]);

    CHECK(expected_messages[0] == saved_message[0 + 4]);
    CHECK(expected_messages[1] == saved_message[1 + 4]);
    CHECK(expected_messages[2] == saved_message[2 + 4]);
    CHECK(expected_messages[3] == saved_message[3 + 4]);

    CHECK(expected_messages[4] == saved_message[0 + 8]);
    CHECK(expected_messages[4] == saved_message[1 + 8]);
  }
}

TEST_CASE("Testing GetInactive<Can>()")
{
  // Setup
  Can & inactive_can = GetInactive<Can>();

  // Exercise
  // Exercise: All the things in order to hit coverage on our side.
  inactive_can.Initialize();
  inactive_can.Send(0x111, {});

  // Verify
  // Verify: that the methods that should return constant values return the
  //         expected values.
  CHECK(Can::Message_t{} == inactive_can.Receive());
  CHECK(false == inactive_can.HasData());
  CHECK(true == inactive_can.SelfTest(0x111));
  CHECK(false == inactive_can.IsBusOff());
}

CanSettings_t::ReceiveHandler receive_handler;

TEST_CASE("Testing CanNetwork")
{
  Mock<Can> mock_can;
  Fake(Method(mock_can, Can::ModuleInitialize));

  Can & can = mock_can.get();
  StaticMemoryResource<1024> memory_resource;
  CanNetwork network(can, &memory_resource);

  SECTION("Initialize()")
  {
    // Setup
    When(Method(mock_can, Can::HasData)).Return(true);
    When(Method(mock_can, Can::Receive)).Return({});

    // Exercise
    network.Initialize();
    network.ManuallyCallReceiveHandler();

    // Verify
    Verify(Method(mock_can, Can::HasData), Method(mock_can, Can::Receive))
        .Once();
  }

  SECTION("Node_t* CaptureMessage(id)")
  {
    // Setup
    size_t initial_memory_usage = memory_resource.MemoryAvailable();

    const std::array<Can::Message_t, 6> kExpectedMessages = {
      Can::Message_t{
          .id      = 0x111,
          .length  = 4,
          .payload = { 0x12, 0x5d, 0xbb, 0xaa },
      },
      Can::Message_t{
          .id      = 0x222,
          .length  = 7,
          .payload = { 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77 },
      },
      Can::Message_t{
          .id      = 0x333,
          .length  = 1,
          .payload = { 0xAA },
      },
      Can::Message_t{
          .id      = 0x444,
          .length  = 0,
          .payload = {},
      },
      Can::Message_t{
          .id      = 0x555,
          .length  = 8,
          .payload = { 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88 },
      },
      Can::Message_t{
          .id      = 0x555,
          .length  = 4,
          .payload = { 0x55, 0x66, 0x77, 0x88 },
      },
    };

    When(Method(mock_can, Can::HasData)).AlwaysReturn(true);
    When(Method(mock_can, Can::Receive))
        .Return(kExpectedMessages[0])
        .Return(kExpectedMessages[1])
        .Return(kExpectedMessages[2])
        .Return(kExpectedMessages[3])
        .Return(kExpectedMessages[4])
        .AlwaysReturn(kExpectedMessages[5]);

    CanNetwork::Node_t * message0 = network.CaptureMessage(0x111);
    CanNetwork::Node_t * message1 = network.CaptureMessage(0x222);
    CanNetwork::Node_t * message2 = network.CaptureMessage(0x333);
    CanNetwork::Node_t * message3 = network.CaptureMessage(0x444);
    CanNetwork::Node_t * message4 = network.CaptureMessage(0x555);

    // Exercise: Manually call the ReceiveHandler which will call Receive() once
    // and place the message into the hashmap in location 0x111. Each exercise
    // step will call Receive() again and will place them into the next node in
    // the hash map.
    {
      // Exercise
      network.ManuallyCallReceiveHandler();

      // Verify
      CHECK(kExpectedMessages[0] == message0->SecureGet());
      CHECK(kExpectedMessages[1] != message1->SecureGet());
      CHECK(kExpectedMessages[2] != message2->SecureGet());
      CHECK(kExpectedMessages[3] != message3->SecureGet());
      CHECK(kExpectedMessages[4] != message4->SecureGet());
    }

    {
      // Exercise
      network.ManuallyCallReceiveHandler();

      // Verify
      CHECK(kExpectedMessages[0] == message0->SecureGet());
      CHECK(kExpectedMessages[1] == message1->SecureGet());
      CHECK(kExpectedMessages[2] != message2->SecureGet());
      CHECK(kExpectedMessages[3] != message3->SecureGet());
      CHECK(kExpectedMessages[4] != message4->SecureGet());
    }

    {
      // Exercise
      network.ManuallyCallReceiveHandler();

      // Verify
      CHECK(kExpectedMessages[0] == message0->SecureGet());
      CHECK(kExpectedMessages[1] == message1->SecureGet());
      CHECK(kExpectedMessages[2] == message2->SecureGet());
      CHECK(kExpectedMessages[3] != message3->SecureGet());
      CHECK(kExpectedMessages[4] != message4->SecureGet());
    }

    {
      // Exercise
      network.ManuallyCallReceiveHandler();

      // Verify
      CHECK(kExpectedMessages[0] == message0->SecureGet());
      CHECK(kExpectedMessages[1] == message1->SecureGet());
      CHECK(kExpectedMessages[2] == message2->SecureGet());
      CHECK(kExpectedMessages[3] == message3->SecureGet());
      CHECK(kExpectedMessages[4] != message4->SecureGet());
    }

    {
      // Exercise
      network.ManuallyCallReceiveHandler();

      // Verify
      CHECK(kExpectedMessages[0] == message0->SecureGet());
      CHECK(kExpectedMessages[1] == message1->SecureGet());
      CHECK(kExpectedMessages[2] == message2->SecureGet());
      CHECK(kExpectedMessages[3] == message3->SecureGet());
      CHECK(kExpectedMessages[4] == message4->SecureGet());
    }

    // Exercise: since kExpectedMessages[4] and kExpectedMessages[5] have the
    // same ID they should both populate message4. But since the last manual
    // call to Receive() will overwrite message4 with kExpectedMessages[5], we
    // should check that is happening.
    {
      // Exercise
      network.ManuallyCallReceiveHandler();

      // Verify
      CHECK(kExpectedMessages[0] == message0->SecureGet());
      CHECK(kExpectedMessages[1] == message1->SecureGet());
      CHECK(kExpectedMessages[2] == message2->SecureGet());
      CHECK(kExpectedMessages[3] == message3->SecureGet());
      CHECK(kExpectedMessages[4] != message4->SecureGet());
      CHECK(kExpectedMessages[5] == message4->SecureGet());
    }

    // Verify: That memory was utilized from the memory resource
    CHECK(initial_memory_usage > memory_resource.MemoryAvailable());

    // Verify: That the internal map contains each of these keys
    CHECK(network.GetInternalMap().contains(0x111));
    CHECK(network.GetInternalMap().contains(0x222));
    CHECK(network.GetInternalMap().contains(0x333));
    CHECK(network.GetInternalMap().contains(0x444));
    CHECK(network.GetInternalMap().contains(0x555));
  }

  SECTION("CaptureMessage(id) std::bad_alloc")
  {
    // Setup
    size_t initial_memory_usage = memory_resource.MemoryAvailable();

    // Exercise + Verify
    CHECK_THROWS_AS(([&network]() {
                      for (int i = 0; i < 1000; i++)
                      {
                        [[maybe_unused]] auto * message =
                            network.CaptureMessage(i);
                      }
                    })(),
                    std::bad_alloc);

    // Verify: That memory was utilized from the memory resource
    CHECK(initial_memory_usage > memory_resource.MemoryAvailable());
  }

  SECTION("ManuallyCallReceiveHandler()")
  {
    // Setup
    When(Method(mock_can, Can::HasData)).Return(true).Return(false);
    When(Method(mock_can, Can::Receive)).Return({});

    // Exercise
    network.ManuallyCallReceiveHandler();
    network.ManuallyCallReceiveHandler();

    // Verify
    // Verify: HasData() should be called twice since we called the receive
    //         handler twice but...
    Verify(Method(mock_can, Can::HasData)).Twice();
    // Verify: but... the Receive() call should only happen once since HasData()
    //         only return true once in a subcase such as this.
    Verify(Method(mock_can, Can::Receive)).Once();
  }

  SECTION("CanBus()")
  {
    // Setup
    // Exercise
    // Verify
    CHECK(&can == &network.CanBus());
  }
}
}  // namespace sjsu
