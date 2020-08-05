#pragma once

#include <span>
#include <string_view>

#include "L1_Peripheral/gpio.hpp"
#include "L1_Peripheral/uart.hpp"
#include "module.hpp"

namespace sjsu
{
namespace bluetooth
{
class Zs040 final : public sjsu::Module
{
 public:
  enum class Mode : uint8_t
  {
    kMain,
    kMaster,
  };

  enum class State : uint8_t
  {
    kAtCommand = 0,
    kData      = 1,
  };

  enum class BaudRate : char
  {
    kB9600  = '2',
    kB38400 = '4',
  };

  enum class Role : char
  {
    kSlave  = '0',
    kMaster = '1',
    kSensor = '2',
    kBeacon = '3',
    kWeChat = '4',
  };

  enum class AuthType : char
  {
    kNoPassword        = '0',
    kPairing           = '1',
    kPairingAndBinding = '2',
  };

  struct ScanResult
  {
    uint8_t idx;
    char address[12];
    char signal_strength[4];
  };

  struct Command  // NOLINT
  {
    static constexpr std::string_view kAt            = "";
    static constexpr std::string_view kSoftwareReset = "+RESET";
    static constexpr std::string_view kSleep         = "+SLEEP";

    static constexpr std::string_view kVersion    = "+VERSION";
    static constexpr std::string_view kMacAddress = "+LADDR";

    static constexpr std::string_view kBaud           = "+BAUD";
    static constexpr std::string_view kRole           = "+ROLE";
    static constexpr std::string_view kUuid           = "+UUID";
    static constexpr std::string_view kCharacteristic = "+CHAR";

    static constexpr std::string_view kDeviceName = "+NAME";
    static constexpr std::string_view kPin        = "+PIN";

    static constexpr std::string_view kScanDevices = "+INQ";
    static constexpr std::string_view kConnect     = "+CONN";

    static constexpr std::string_view kIac = "+IAC";

    static constexpr std::string_view kScanAccess = "+INQM";
  };

  static constexpr uint32_t kDefaultBaudRate = 9'600;
  // static constexpr uint32_t kAtBaudRate      = 38'400;

  static constexpr std::string_view kCrNl     = "\r\n";
  static constexpr std::string_view kStatusOk = "OK\r\n";

  /// @param uart The UART peripheral used to communicate with the device.
  /// @param key_pion The device's chip enable pin. When driven low, the
  ///                 device will disconnect any connected bluetooth devices.
  /// @param state_pin The device's state pin.
  explicit Zs040(sjsu::Uart & uart,
                 sjsu::Gpio & key_pin   = sjsu::GetInactive<sjsu::Gpio>(),
                 sjsu::Gpio & state_pin = sjsu::GetInactive<sjsu::Gpio>())
      : uart_(uart), key_pin_(key_pin), state_pin_(state_pin)
  {
  }

  virtual void ModuleInitialize() override
  {
    state_pin_.SetAsInput();
    key_pin_.SetAsOutput();
    key_pin_.SetHigh();

    uart_.Initialize();
    uart_.ConfigureFormat();
    // uart_.ConfigureBaudRate(kDefaultBaudRate);
    uart_.ConfigureBaudRate(38400);
  }

  virtual void ModuleEnable([[maybe_unused]] bool enable) override
  {
    uart_.Enable();
  }

  bool GetDeviceState() const
  {
    return state_pin_.Read();
  }

  bool IsAtMode()
  {
    SendCommand(Command::kAt);
    return std::string_view(at_response_buffer_).starts_with(kStatusOk);
  }

  void EnterAtMode() const
  {
    key_pin_.SetHigh();
  }

  void ExitAtMode() const
  {
    key_pin_.SetLow();
  }

  void SoftwareReset()
  {
    SendCommand(Command::kSoftwareReset);
    sjsu::Delay(500ms);
  }

  /// @returns The device's version.
  const std::string_view GetVersion()
  {
    return SendCommand(Command::kVersion);
  }

  /// @returns The device's MAC address as a string in the following format:
  ///          XX:XX:XX:XX:XX:XX.
  const std::string_view GetMacAddress()
  {
    return SendCommand(Command::kMacAddress);
  }

  Role SetRole(Role role)
  {
    const char kRoleSelect = sjsu::Value(role);
    return Role(SendCommand(Command::kRole, &kRoleSelect)[0]);
  }

  //// @returns The current configured role.
  Role GetRole()
  {
    return Role(SendCommand(Command::kRole)[0]);
  }

  /// @param device_name The name to set, must be 18 bytes or less.
  const std::string_view SetDeviceName(const std::string_view device_name)
  {
    return SendCommand(Command::kDeviceName, device_name, true);
  }

  const std::string_view GetDeviceName()
  {
    return SendCommand(Command::kDeviceName);
  }

  BaudRate SetBaudRate(const BaudRate baud)
  {
    const char baud_select = sjsu::Value(baud);
    return BaudRate(SendCommand(Command::kBaud, &baud_select)[0]);
  }

  BaudRate GetBaudRate()
  {
    return BaudRate(SendCommand(Command::kBaud)[0]);
  }

  /// @param uuid The 6 byte UUID string ranging between '0x0001' to '0xFFFE'.
  /// @returns
  const std::string_view SetUuid(const std::string_view uuid)
  {
    return SendCommand(Command::kUuid, uuid);
  }

  /// @returns The 6 byte UUID string ranging between '0x0001' to '0xFFFE'.
  const std::string_view GetUuid()
  {
    return SendCommand(Command::kUuid);
  }

  const std::string_view SetCharacteristic(
      const std::string_view characteristic)
  {
    return SendCommand(Command::kCharacteristic, characteristic);
  }

  const std::string_view GetCharacteristic()
  {
    return SendCommand(Command::kCharacteristic);
  }

  // ---------------------------------------------------------------------------
  // Master Mode
  // ---------------------------------------------------------------------------

  void Scan()
  {
    constexpr uint8_t kEmptyChar           = 255;
    constexpr std::string_view kInqueryEnd = "+INQE\r\n";

    SendCommand(Command::kScanDevices, "1");

    char c            = kEmptyChar;
    bool search_ended = false;
    size_t idx        = 0;
    char buffer[200];
    memset(buffer, '\0', sizeof(buffer));

    do
    {
      c = uart_.Read();
      if (c != kEmptyChar)
      {
        buffer[idx++] = c;
      }

      search_ended =
          std::string_view(buffer).find(kInqueryEnd) != std::string_view::npos;
    } while (!search_ended && (idx < sizeof(buffer)));

    sjsu::LogInfo("Scan done... %s", buffer);
  }

  void StopScan()
  {
    SendCommand(Command::kScanDevices, "0");
  }

  void Connect(uint8_t device_index)
  {
    const char index = static_cast<char>(device_index + '0');
    SendCommand(Command::kConnect, &index);
  }

  void GetConnectionStatus() {}

  void Disconnect() {}

  // ---------------------------------------------------------------------------
  // iBeacon
  // ---------------------------------------------------------------------------

  // ---------------------------------------------------------------------------
  //
  // ---------------------------------------------------------------------------

  const std::string_view SendCommand(const std::string_view command,
                                     const std::string_view parameter = "",
                                     bool wait_for_ok                 = false)
  {
    constexpr std::string_view kPrefix = "AT";

    uart_.Write(kPrefix);
    uart_.Write(command);
    uart_.Write(parameter);
    uart_.Write(kCrNl);

    Read(at_response_buffer_);

    std::string_view parsed_response(at_response_buffer_);
    parsed_response.remove_suffix(parsed_response.size() -
                                  parsed_response.find(kCrNl));
    parsed_response.remove_prefix(std::min(
        parsed_response.find_first_not_of(command), parsed_response.size()));
    parsed_response.remove_prefix(std::min(
        parsed_response.find_first_not_of("="), parsed_response.size()));

    if (wait_for_ok)
    {
      char status_buffer[10];
      Read(status_buffer);
      // TODO: throw if ERROR:
    }

    return parsed_response;
  }

 private:
  void Read(std::span<char> buffer)
  {
    constexpr uint8_t kEmptyChar = 255;
    uint32_t idx                 = 0;
    char c                       = kEmptyChar;

    memset(buffer.data(), '\0', buffer.size());

    do
    {
      c = uart_.Read();
      if (c != kEmptyChar)
      {
        buffer[idx++] = c;
      }
    } while (c != '\n' && (idx < buffer.size()));
  }

  sjsu::Uart & uart_;
  sjsu::Gpio & key_pin_;
  sjsu::Gpio & state_pin_;

  /// Buffer for holding responses in AT Command Mode.
  ///
  /// @note This will not be able to suport the HELP command
  char at_response_buffer_[30];
};
}  // namespace bluetooth
}  // namespace sjsu