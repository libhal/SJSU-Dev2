#pragma once

#include <cstddef>
#include <cstdint>
#include <initializer_list>

#include "utility/status.hpp"

// 0. Everything in the library folder must be within the sjsu namespace.
namespace sjsu
{
class Example
{
 public:
  // 1. Peripheral API:
  // Every peripheral interface will need to have a set of methods that the
  // implementing class will need to provide to make the Peripheral feature
  // complete. The number of virtual methods should be as low as possible to
  // keep the vtable as small as possible.

  // 2. Implementation methods must be pure virtual (Required)
  // By using virtual, the platform implementation can override these methods
  // with its own implementation.
  // These methods are pure (= 0) to force the implementing class to
  // implement every single virtual method specified here.

  // 4. Class must be operable when made const (Required)
  // every method must be made const, such that the object itself
  // can be used in a const context.

  // 5. Refrain from interface constructors (Best Practice)
  // Interfaces should refrain from having their own constructor.

  // 6. Every peripheral must have an Initialize() method (Required)
  // Every peripheral must have an Initialize() method that the user can use to
  // turn on and enable the hardware. This is the first method the user must
  // execute before running any other methods of the peripheral. Initialize
  // should return a status indicating if it was successful, or if some error
  // occured.
  virtual Status Initialize() const = 0;
  // 7. API for data transfer
  // If your peripheral is a communication protocol, the standard for sending
  // and receiving data is to follow the method signature. A write or read that
  // only transfer a single byte is not optimal as a function call is not
  // optimal when stream large amounts of data.
  virtual bool Write(const uint8_t * data, size_t length) const = 0;
  // 7.1 Read Timeouts
  // Timeout should not be apart of the API if it doesn't make sense for the
  // protocol. Example would be something like SPI where the master drives the
  // read operation. This is useful for something like UART where the event of
  // recieving data is asyncronous.
  virtual Status Read(uint8_t * data,
                      size_t length,
                      uint32_t timeout) const = 0;
  // 8. Setters
  // Should be prefixed with the word "Set". Setters are good for
  // configuring the peripheral after Initialize() has run.
  // Here are a few examples.
  virtual void SetValue(uint32_t value) const         = 0;
  virtual void SetClockRate(uint32_t frequency) const = 0;
  // 9. Getters
  // Should be prefixed with the word "Get". Getters should retrieve
  // the current configuration of the peripheral. These are generally
  // discouraged as adding them increases the vtable without bring much value.
  // Rather than supply Getters, unit tests should make sure that the setters
  // actually work.
  virtual float GetValue() const        = 0;
  virtual uint32_t GetClockRate() const = 0;
  // 10. Status methods
  // Methods that indicate the status of the peripheral should be prefixed
  // with the word "Has". These must return bool.
  virtual bool HasCompletedAction() const = 0;
  // 11. Utility methods:
  // The follow methods use the virtual methods to control the peripheral. They
  // should give a variety use methods for using the API.

  /// Write a single byte
  /// @param byte - the byte you would like to write.
  /// @usage
  ///    example.Write(0x22);
  void Write(uint8_t byte)
  {
    Write(&byte, 1);
  }
  // Write bytes using an array literal
  // @param data - array literal of bytes.
  // @usage
  //    example.Write({ 0xAA, 0xBB, 0x02, 0x54 });
  //
  void Write(std::initializer_list<uint8_t> data)
  {
    Write(data.begin(), data.size());
  }
  // Overload of Read that waits forever if a timeout was not supplied.
  Status Read(uint8_t * data, size_t size)
  {
    return Read(data, size, UINT32_MAX);
  }
  // Wait to receive just 1 byte
  uint8_t Read(uint32_t timeout = UINT32_MAX)
  {
    uint8_t byte;
    // load byte with byte recieved from uart buffer.
    // If Recieve returns TIMEOUT, runs out of time, then load byte with 0xFF.
    if (Read(&byte, 1, timeout) == Status::kTimedOut)
    {
      byte = 0;
    }
    return byte;
  }
  // Add additional utility methods here.
 protected:
  // Prefer not to use the protected section for anything. If it must be used it
  // can only be used for const variables.
 private:
  // Private should almost never be used in the interface
};
}  // namespace sjsu
