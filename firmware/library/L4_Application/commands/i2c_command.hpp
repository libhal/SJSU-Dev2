#pragma once

#include <iterator>

#include "L2_Utilities/log.hpp"
#include "L4_Application/commandline.hpp"

/// The I2cCommand [will] allows the user to discover, read, and write to
/// devices on the I2C bus.
class I2cCommand : public Command
{
 public:
  static constexpr char kDescription[] =
      "Read, write and discover devices on the i2c bus";

  constexpr I2cCommand() : Command("i2c", kDescription) {}

  int Program(int argc, const char * const argv[]) override final
  {
    LOG_INFO("This is the i2c program. It doesn't do anything yet...");
    LOG_INFO("argv[0] = %s", (argc > 1) ? argv[1] : "");
    return 0;
  }

  int AutoComplete(int argc, const char * const argv[],
                   const char * completion[],
                   const size_t kCompletionArrayLength) override final
  {
    size_t position = 0;
    completion[0]   = nullptr;
    switch (argc)
    {
      // If nothing has been typed for the function, provide the i2c functions
      case 2:
        for (size_t i = 0;
             i < std::size(kI2cFunctionList) && i < kCompletionArrayLength; i++)
        {
          if (std::strstr(kI2cFunctionList[i], argv[1]) == kI2cFunctionList[i])
          {
            completion[position++] = const_cast<char *>(kI2cFunctionList[i]);
          }
        }
        break;
      case 3:
        for (size_t i = 0;
             i < std::size(kI2cAddresses) && i < kCompletionArrayLength; i++)
        {
          if (std::strstr(kI2cAddresses[i], argv[2]) == kI2cAddresses[i])
          {
            completion[position++] = const_cast<char *>(kI2cAddresses[i]);
          }
        }
        break;
      default: break;
    }
    return position;
  }

 private:
  static inline const char * const kI2cFunctionList[] = { "read", "write",
                                                          "discover", nullptr };
  static inline const char kI2cAddresses[][5] = { "0x1C", "0x25", "0x33" };
};
