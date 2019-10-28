#pragma once

#include <initializer_list>

#include "L1_Peripheral/example.hpp"
#include "utility/status.hpp"
#include "utility/time.hpp"

// 0. Everything in the library folder must be within the sjsu namespace.
namespace sjsu
{
/// 1. Create your interface class. Refrain from putting the suffix 'Interface'
///    at the end of the name.
///    A HAL Interfaces are not necessary if the HAL you are building on has one
///    meaningful implemenation, for example the SD card implementation. That
///    implementation is a standard and all SD cards must follow it. The types
///    of communication protocols are also standard (SPI in this case), so as
///    long as the SD card driver can accept an SPI peripheral, then it can
///    work.
class ExampleHal
{
 public:
  /// 2. Define core methods for this interface as pure virtual methods, so that
  ///    an specific implementation can use these.
  ///    A good example of when this is useful is when a HAL can be implemented
  ///    in multiple ways. For example, the LCD HAL driver supports operation
  ///    via, I2C, SPI, UART and parallel GPIO. Each implementation can be its
  ///    own implementation of the LCD interface. They would implement the
  ///    Initialize() and Write() methods for their specific peripheral.
  virtual Status Initialize() = 0;
  /// Example Write() method.
  virtual Status Write(const uint8_t * data, size_t size) = 0;
  /// 3. Define any supporting methods that use the core methods below. These
  ///    must not be virtual.
  Status Write(std::initializer_list<uint8_t> data)
  {
    return Write(data.begin(), data.size());
  }
  /// Example utility method that utilizes the virtual Write() method.
  void RunHalRoutine0()
  {
    Write({ 0xAA, 0xBB });
    Delay(500ms);
    Write({ 0xCC, 0xDD });
  }
  /// Example utility method that utilizes the virtual Write() method.
  void Shutdown()
  {
    Write({ 0xDE, 0xAD });
  }
};

/// 4. Implementations should be in their own file, but for simplicity, the
///    implementation is here.
///
///    The folder heirachy should be something like this:
///
///    category/
///    |
///    +-- hal_name.hpp (interface)
///    |
///    +-- hal_name/ (folder containing the implementations)
///         |
///         +-- hal_name_gpio.hpp (implementation for gpio)
///         +-- hal_name_i2c.hpp  (implementation for i2c)
///         +-- hal_name_spi.hpp  (implementation for spi)
///         +-- hal_name_etc.hpp  (implementation for whatever else)
class ExampleHalImplementation : public ExampleHal
{
 public:
  /// Example copy constructor
  /// @param example - object to copy.
  explicit ExampleHalImplementation(const Example & example) : example_(example)
  {
  }
  /// 5. Implement the methods that are required as an implementaiton of the
  ///    interface. Add additional supporting methods if it makes writting these
  ///    two methods easier.
  Status Initialize() override
  {
    Status example_init_status = example_.Initialize();
    return example_init_status;
  }
  Status Write(const uint8_t * data, size_t size) override
  {
    Status status = Status::kSuccess;

    if (!example_.HasCompletedAction())
    {
      status = Status::kNotReadyYet;
    }
    else
    {
      bool write_was_successfull = example_.Write(data, size);
      if (!write_was_successfull)
      {
        status = Status::kBusError;
      }
    }
    return status;
  }

 private:
  const Example & example_;
};

/// 6. In a single implementation hardware abstract layer class, the expectation
/// is that no one is going to inherit this to extend its capability.
class ExampleSingleImplementationHal
{
 public:
  /// Example copy constructor
  /// @param example - object to copy.
  explicit ExampleSingleImplementationHal(const Example & example)
      : example_(example)
  {
  }
  /// Example Initialize() method
  Status Initialize()
  {
    return Status::kSuccess;
  }
  /// 7. Declare core methods of this HAL, that are used by the other supporting
  ///    methods below as virtual so that testing software can replace or spy on
  ///    this method.
  virtual Status Write(const uint8_t * data, size_t size)
  {
    example_.Write(data, size);
    return Status::kSuccess;
  }
  /// 8. Utility methods that use the core methods above should not be declare
  ///    as virtual.
  Status Write(std::initializer_list<uint8_t> data)
  {
    return Write(data.begin(), data.size());
  }
  /// Example utility method that utilizes the virtual Write() method.
  void RunHalRoutine0()
  {
    Write({ 0xAA, 0xBB });
    Delay(500ms);
    Write({ 0xCC, 0xDD });
  }
  /// Example utility method that utilizes the virtual Write() method.
  void Shutdown()
  {
    Write({ 0xDE, 0xAD });
  }

 private:
  const Example & example_;
};
}  // namespace sjsu
