#pragma once

#include <cstdlib>
#include <cstring>
#include <iterator>
#include <tuple>

#include "L1_Peripheral/i2c.hpp"
#include "L3_Application/commandline.hpp"
#include "utility/allocator.hpp"
#include "utility/log.hpp"

namespace sjsu
{
/// The I2cCommand [will] allows the user to discover, read, and write to
/// devices on the I2C bus.
class I2cCommand final : public Command
{
 public:
  /// An enumeration that specifies the locations of each command argument in
  /// the command line.
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

  /// Container for each address found when running i2c "discover"
  struct AddressString_t
  {
    /// String large enough to hold the string 0x00
    char str[sizeof("0x00")];

    /// @param address - Address to convert to a string
    explicit AddressString_t(uint8_t address)
    {
      snprintf(str, sizeof(str), "0x%02X", address);
    }
  };

  /// Holds the information needed to perform the I2C transaction with an
  /// external device.
  struct Arguments_t
  {
    /// String containing which operation to perform
    char * operation = nullptr;
    /// Holds the i2c address of the external device to communicate with.
    /// NOTE: Only handles 8-bit i2c addresses currently.
    uint8_t device_address = 0x00;
    /// Holds the memory location mapped within the device.
    /// NOTE: Only supports 8-bit i2c register addresses currently.
    uint8_t register_address = 0x00;
    /// How many bytes to read or write to the device.
    uint8_t length = 0;
    /// Gets set when the1
    bool invalid = false;
  };

  /// I2c usage description and details.
  static constexpr char kDescription[] = R"(Read and write to the i2c bus.
                i2c write <device address> <register address> <data0> ...
                i2c read <device address> <register address> <length>
                i2c discover
  )";

  static inline const char * const kI2cOperations[] = { "read",
                                                        "write",
                                                        "discover",
                                                        nullptr };

  static constexpr uint8_t kFirstI2cAddress = 0x08;
  static constexpr uint8_t kLastI2cAddress  = 0x78;
  static constexpr size_t kNumberOfI2cAddresses =
      kLastI2cAddress - kFirstI2cAddress;

  /// Sole constructor of the I2c command
  explicit I2cCommand(I2c & i2c)
      : Command("i2c", kDescription),
        vector_buffer_(),
        devices_found_(&vector_buffer_),
        i2c_(i2c)
  {
    devices_found_.reserve(kNumberOfI2cAddresses);
  }

  /// Initializes i2c peripheral. MUST be called before calling any other method
  /// in this class.
  void Initialize()
  {
    i2c_.Initialize();
  }

  int AutoComplete(int argc,
                   const char * const argv[],
                   const char * completion[],
                   size_t completion_length) override
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
        for (size_t i = 0; i < completion_length && i < devices_found_.size();
             i++)
        {
          auto & address               = devices_found_[i];
          const char * const kArgument = argv[Args::kDeviceAddress];
          if (std::strstr(address.str, kArgument) == address.str)
          {
            completion[position++] = address.str;
          }
        }
    }
    return static_cast<int>(position);
  }

  int Program(int argc, const char * const argv[]) override
  {
    if (argc - 1 < kOperation)
    {
      sjsu::LogError("Invalid number of arguments, required %d, supplied %d",
                     kOperation,
                     argc);
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
  /// Converts a string holding a numeric value into a byte integer
  ///
  /// @param kArgument - number string to be converted
  /// @param radix - the numeric radix (or numeric base) of the string
  /// @return std::tuple<uint8_t, bool> - the converted value and a bool that
  /// indicates if the value was valid.
  std::tuple<uint8_t, bool> ParseByte(const char * const kArgument,
                                      uint8_t radix)
  {
    char * string_end;
    uint8_t byte =
        static_cast<uint8_t>(std::strtol(kArgument, &string_end, radix));
    return std::make_tuple(byte, (string_end != kArgument));
  }

  /// Returns the i2c operation from the argument string
  ///
  /// @param kOperationArgument - the start of string that contains the i2c
  ///        operation.
  /// @return returns which i2c operation to perform such as write, read,
  /// discover.
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

  /// Converts a list of argv[] into an Arguments_t
  ///
  /// @param argv - list of string arguments
  /// @return Arguments_t - structure full of the i2c arguments
  Arguments_t ParseArguments(int, const char * const argv[])
  {
    Arguments_t args;
    args.operation = GetI2cOperation(argv[Args::kOperation]);
    if (args.operation == nullptr)
    {
      sjsu::LogError("Invalid operation %s", argv[Args::kOperation]);
      args.invalid = true;
    }

    auto [device_address, s0] = ParseByte(argv[Args::kDeviceAddress], 16);
    if (!s0)
    {
      sjsu::LogError(
          "Invalid device address %u, must be a hex number of format 0xAA",
          device_address);
      args.invalid = true;
    }

    auto [register_address, s1] = ParseByte(argv[Args::kRegisterAddress], 16);
    if (!s1)
    {
      sjsu::LogError(
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
    devices_found_.clear();

    for (uint8_t address = kFirstI2cAddress; address < kLastI2cAddress;
         address++)
    {
      uint8_t buffer;
      try
      {
        // This will throw an exception if device is not found on bus (does not
        // acknowledge address)
        i2c_.Read(address, &buffer, sizeof(buffer), 50ms);

        // These lines can only be reached if the I2C read did not throw
        AddressString_t address_string(address);
        devices_found_.push_back(address_string);
      }
      catch (const sjsu::Exception &)
      {
        // Catch this exception and do nothing.
        // Any other exceptions must propogate up.
      }
    }
  }

  int PerformReadOperation(int argc, const char * const argv[])
  {
    if (argc - 1 < kLength)
    {
      sjsu::LogError(
          "Invalid number of arguments for read operation, required %d, "
          "supplied %d",
          kRegisterAddress,
          argc);
      return 1;
    }
    Arguments_t args = ParseArguments(argc, argv);
    uint8_t contents[128];
    if (args.length < sizeof(contents))
    {
      i2c_.WriteThenRead(args.device_address,
                         &args.register_address,
                         1,
                         contents,
                         args.length);
      debug::Hexdump(contents, args.length);
    }
    else
    {
      sjsu::LogError("Length cannot be more then 128 bytes.");
      return 1;
    }
    return 0;
  }

  int PerformWriteOperation(int argc, const char * const argv[])
  {
    if (argc - 1 < kRegisterAddress)
    {
      sjsu::LogError(
          "Invalid number of arguments for write operation, required %d, "
          "supplied %d",
          kRegisterAddress,
          argc);
      return 1;
    }
    Arguments_t args = ParseArguments(argc, argv);
    uint8_t payload[64];
    size_t position;
    size_t number_of_arguments = static_cast<size_t>(argc);
    for (position = 0; position < std::size(payload) &&
                       (Args::kWriteStartByte + position) < number_of_arguments;
         position++)
    {
      payload[position] =
          std::get<0>(ParseByte(argv[Args::kWriteStartByte + position], 16));
    }
    i2c_.Write(args.device_address, payload, position);
    debug::Hexdump(payload, position);
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

  StaticAllocator<sizeof(AddressString_t) * 128> vector_buffer_;
  std::pmr::vector<AddressString_t> devices_found_;
  I2c & i2c_;
};
}  // namespace sjsu
