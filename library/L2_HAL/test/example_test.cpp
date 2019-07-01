#include "L2_HAL/example.hpp"
#include "L4_Testing/testing_frameworks.hpp"

namespace sjsu
{
TEST_CASE("Testing Example Implementation", "[example-implementation]")
{
  // 1. Setup your mocks
  Mock<sjsu::Example> mock_example;
  sjsu::ExampleHalImplementation test_subject(mock_example.get());

  // 2. Test that your Initialize operates as intended
  SECTION("Initialize")
  {
    // 3. We stub Initialize for our mock object to always return successful
    //    and test the behaviour.
    When(Method(mock_example, Initialize)).AlwaysReturn(Status::kSuccess);
    // 4. Execute the method under test.
    Status initialize_status = test_subject.Initialize();
    // 5. Test that the return type is what is expected. In this case, the code
    //    returns what ever the example implementation returns.
    CHECK(Status::kSuccess == initialize_status);
    Verify(Method(mock_example, Initialize)).Once();

    // 6. This will undo the stub above
    mock_example.Reset();
    // 7. Repeat with new behaviour.
    When(Method(mock_example, Initialize)).AlwaysReturn(Status::kTimedOut);

    initialize_status = test_subject.Initialize();
    CHECK(Status::kTimedOut == initialize_status);
    Verify(Method(mock_example, Initialize)).Once();
  }
  // 8. The approach in the SECTION(Write) test is to stub the methods of the
  //    mock example in order to test the different branches of the Write()
  //    method.
  SECTION("Write")
  {
    // 9. Declare dummy array to pass into the Write() method.
    constexpr uint8_t kDummyArray[4] = { 1, 2, 3, 4 };
    // 10. Stub HasCompletedAction() to always return true to evaluate the
    //     "else" branch in Write().
    When(Method(mock_example, HasCompletedAction)).AlwaysReturn(true);
    // 11. Stub Write() to always return true to evaluate the "if" branch in
    //     Write();
    When(Method(mock_example, Write)).AlwaysReturn(true);

    Status write_status = test_subject.Write(kDummyArray, sizeof(kDummyArray));
    Verify(Method(mock_example, Write).Using(kDummyArray, sizeof(kDummyArray)))
        .Once();
    CHECK(Status::kSuccess == write_status);
    // Repeat the above for every branch of the code.
    mock_example.Reset();
    When(Method(mock_example, HasCompletedAction)).AlwaysReturn(true);
    When(Method(mock_example, Write)).AlwaysReturn(false);

    write_status = test_subject.Write(kDummyArray, sizeof(kDummyArray));
    Verify(Method(mock_example, Write).Using(kDummyArray, sizeof(kDummyArray)))
        .Once();
    CHECK(Status::kBusError == write_status);

    mock_example.Reset();
    When(Method(mock_example, HasCompletedAction)).AlwaysReturn(false);

    write_status = test_subject.Write(kDummyArray, sizeof(kDummyArray));
    CHECK(Status::kNotReadyYet == write_status);
  }
}

// 12. If your interface contains utility methods, then a test of the interface
//     itself must be created
TEST_CASE("Testing Example Interface", "[example-interface]")
{
  // 13. Create a mock object of the example interface itself
  Mock<sjsu::ExampleHal> mock_example_interface;

  uint8_t test_buffer[32] = { 0 };
  size_t test_buffer_size = 0;
  // 14. Fake the behaviour of the virtual methods in your interface that your
  //     utility methods actually use.
  //     In this special case, we take an array, so we need to replace its
  //     functionality
  When(OverloadedMethod(
           mock_example_interface, Write, Status(const uint8_t *, size_t)))
      .AlwaysDo([&](const uint8_t * data, size_t size) -> Status {
        memcpy(&test_buffer[test_buffer_size], data, size);
        test_buffer_size += size;
        return Status::kSuccess;
      });
  // 15. Even though it might seem like a good idea to fake Initialize, as
  //     calling it will cause our test to fail. But this is exactly what should
  //     be done. None of our utility methods call Initialize, thus, it
  //     shouldn't be faked out. So if any utility methods DO call Initialize,
  //     our test will fail.
  //
  // Keep the folowing line commented out:
  //
  //    Fake(Method(mock_example_interface, Initialize));

  sjsu::ExampleHal & test_subject = mock_example_interface.get();

  SECTION("RunHalRoutine0")
  {
    constexpr uint8_t kExpectedArray[] = {
      0xAA,
      0xBB,
      0xCC,
      0xDD,
    };
    test_subject.RunHalRoutine0();
    for (size_t i = 0; i < test_buffer_size; i++)
    {
      // NOTE: this is done to keep CATCH2 from converting the uint8_t to ascii
      // characters. If this isn't done, the result is either empty, or the
      // error ? character.
      CHECK(static_cast<uint32_t>(kExpectedArray[i]) ==
            static_cast<uint32_t>(test_buffer[i]));
    }
    // 16. Clean shared buffer for use in the other test SECTION().
    memset(test_buffer, 0, sizeof(test_buffer));
  }
  SECTION("Shutdown")
  {
    constexpr uint8_t kExpectedArray[] = {
      0xDE,
      0xAD,
    };
    test_subject.Shutdown();
    for (size_t i = 0; i < test_buffer_size; i++)
    {
      CHECK(static_cast<uint32_t>(kExpectedArray[i]) ==
            static_cast<uint32_t>(test_buffer[i]));
    }
    memset(test_buffer, 0, sizeof(test_buffer));
  }
}
}  // namespace sjsu
