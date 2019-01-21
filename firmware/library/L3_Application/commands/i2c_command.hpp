#pragma once

#include <cstdlib>
#include <iterator>
#include <tuple>

#include "L1_Drivers/i2c.hpp"
#include "L3_Application/commandline.hpp"
#include "third_party/etl/vector.h"
#include "utility/log.hpp"

/// The I2cCommand [will] allows the user to discover, read, and write to
/// devices on the I2C bus.
class I2cCommand : public Command
{
 public:
  enum Args
  {
    kName          = 0,
    kOperation     = 1,
    kDeviceAddress = 2,
    // kRegisterAddress && kWriteStartByte have the same argument position
    kRegisterAddress = 3,
    kWriteStartByte  = 3,
    kLength          = 4
  };
  struct AddressString_t
  {
    // String large enough to hold the string 0x00
    char str[sizeof("0x00")];
  };

  struct Arguments_t
  {
    char * operation         = nullptr;
    uint8_t device_address   = 0x00;
    uint8_t register_address = 0x00;
    uint8_t length           = 0;
    bool invalid             = false;
  };

  static constexpr char kDescription[] = R"(Read and write to the i2c bus.
                i2c write <device address> <register address> <data0> ...
                i2c read <device address> <register address> <length>
                i2c discover
  )";

  I2cCommand() : Command("i2c", kDescription) {}

  void Initialize()
  {
    i2c_peripheral_.Initialize();
  }

  std::tuple<uint8_t, bool> ParseByte(const char * const kArgument,
                                      uint8_t radix)
  {
    char * string_end;
    uint8_t byte =
        static_cast<uint8_t>(std::strtol(kArgument, &string_end, radix));
    return std::make_tuple(byte, (string_end != kArgument));
  }

  char * GetI2cOperation(const char * const kOperationArgument)
  {
    for (const char * operation : kI2cOperations)
    {
      if (std::strstr(operation, kOperationArgument) == operation)
      {
        return const_cast<char *>(operation);
      }
    }
    return nullptr;
  }
  Arguments_t ParseArguments(int, const char * const argv[])
  {
    Arguments_t args;
    args.operation = GetI2cOperation(argv[Args::kOperation]);
    if (args.operation == nullptr)
    {
      LOG_ERROR("Invalid operation %s", argv[Args::kOperation]);
      args.invalid = true;
    }

    auto [device_address, s0] = ParseByte(argv[Args::kDeviceAddress], 16);
    if (!s0)
    {
      LOG_ERROR(
          "Invalid device address %u, must be a hex number of format 0xAA",
          device_address);
      args.invalid = true;
    }

    auto [register_address, s1] = ParseByte(argv[Args::kRegisterAddress], 16);
    if (!s1)
    {
      LOG_ERROR(
          "Invalid register address %u, must be a hex number of format 0xAA",
          register_address);
      args.invalid = true;
    }

    auto byte_status_tuple = ParseByte(argv[Args::kLength], 10);
    args.device_address    = device_address;
    args.register_address  = register_address;
    args.length            = std::get<0>(byte_status_tuple);
    return args;
  }

  void I2cDiscover()
  {
    constexpr uint8_t kFirstI2cAddress = 0x08;
    constexpr uint8_t kLastI2cAddress  = 0x78;

    devices_found_.clear();

    for (uint8_t address = kFirstI2cAddress; address < kLastI2cAddress;
         address++)
    {
      if (Status::kSuccess == i2c_->Write(address, nullptr, 0, 50))
      {
        AddressString_t address_string;
        snprintf(address_string.str, sizeof(address_string.str), "0x%02X",
                 address);
        devices_found_.push_back(address_string);
      }
    }
  }

  int PerformReadOperation(int argc, const char * const argv[])
  {
    if (argc - 1 < kLength)
    {
      LOG_ERROR(
          "Invalid number of arguments for read operation, required %d, "
          "supplied %d",
          kRegisterAddress, argc);
      return 1;
    }
    Arguments_t args = ParseArguments(argc, argv);
    uint8_t contents[128];
    if (args.length < sizeof(contents))
    {
      i2c_->WriteThenRead(args.device_address, &args.register_address, 1,
                          contents, args.length);
      debug::Hexdump(contents, args.length);
    }
    else
    {
      LOG_ERROR("Length cannot be more then 128 bytes.");
      return 1;
    }
    return 0;
  }

  int PerformWriteOperation(int argc, const char * const argv[])
  {
    if (argc - 1 < kRegisterAddress)
    {
      LOG_ERROR(
          "Invalid number of arguments for write opeation, required %d, "
          "supplied %d",
          kRegisterAddress, argc);
      return 1;
    }
    Arguments_t args = ParseArguments(argc, argv);
    uint8_t payload[64];
    size_t position;
    for (position = 0; position < std::size(payload) &&
                       argv[Args::kWriteStartByte + position] != nullptr;
         position++)
    {
      payload[position] =
          std::get<0>(ParseByte(argv[Args::kWriteStartByte + position], 16));
    }
    // subtract 1 since position will overshoot by 1
    i2c_->Write(args.device_address, payload, position - 1);
    debug::Hexdump(payload, position - 1);
    return 0;
  }

  int PerformDiscoveryOperation()
  {
    I2cDiscover();
    for (size_t i = 0; i < devices_found_.size(); i++)
    {
      if (i % 16 == 0)
      {
        printf("\n");
      }
      printf("%s ", devices_found_[i].str);
    }
    printf("\n");
    return 0;
  }

  int AutoComplete(int argc, const char * const argv[],
                   const char * completion[], const size_t) override final
  {
    size_t position = 0;
    completion[0]   = nullptr;
    switch (argc - 1)
    {
      // If nothing has been typed for the function, provide the i2c functions
      case Args::kOperation:
        for (const char * operation : kI2cOperations)
        {
          const char * const kArgument = argv[Args::kOperation];
          if (std::strstr(operation, kArgument) == operation)
          {
            completion[position++] = const_cast<char *>(operation);
          }
        }
        break;
      case Args::kDeviceAddress:
        // Do not perform a tab complete when the operation is "discover"
        if (strcmp(argv[Args::kOperation], kI2cOperations[2]) == 0)
        {
          break;
        }
        I2cDiscover();
        for (auto & address : devices_found_)
        {
          const char * const kArgument = argv[Args::kDeviceAddress];
          if (std::strstr(address.str, kArgument) == address.str)
          {
            completion[position++] = address.str;
          }
        }
    }
    return position;
  }

  int Program(int argc, const char * const argv[]) override final
  {
    if (argc - 1 < kOperation)
    {
      LOG_ERROR("Invalid number of arguments, required %d, supplied %d",
                kOperation, argc);
      return 1;
    }

    const char * operation = GetI2cOperation(argv[Args::kOperation]);

    if (operation == kI2cOperations[0])  // read
    {
      return PerformReadOperation(argc, argv);
    }
    else if (operation == kI2cOperations[1])  // write
    {
      return PerformWriteOperation(argc, argv);
    }
    else if (operation == kI2cOperations[2])  // discover
    {
      return PerformDiscoveryOperation();
    }

    return 0;
  }

 private:
  static inline const char * const kI2cOperations[] = { "read", "write",
                                                        "discover", nullptr };
  etl::vector<AddressString_t, command::kAutoCompleteOptions> devices_found_;
  I2c i2c_peripheral_;
  I2cInterface * i2c_ = &i2c_peripheral_;
};
