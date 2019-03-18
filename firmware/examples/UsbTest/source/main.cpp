#include <algorithm>
#include <cinttypes>
#include <cstdint>
#include <cmath>

#include <project_config.hpp>

#include "L0_LowLevel/LPC40xx.h"
#include "L0_LowLevel/system_controller.hpp"
#include "L0_LowLevel/interrupt.hpp"
#include "L1_Drivers/pin.hpp"
#include "L2_HAL/displays/led/onboard_led.hpp"
#include "L2_HAL/switches/button.hpp"
#include "L1_Drivers/uart.hpp"

#include "third_party/etl/vector.h"
#include "utility/log.hpp"
#include "utility/bit.hpp"
#include "utility/time.hpp"
#include "utility/rtos.hpp"

OnBoardLed leds;

constexpr uint8_t DEV_CLK_EN        = 1;
constexpr uint8_t PORTSEL_CLK_EN    = 3;
constexpr uint8_t AHB_CLK_EN        = 4;
constexpr uint8_t kEndpointRealized = 8;
constexpr uint8_t EP_SLOWEN         = 2;
constexpr uint8_t DEV_STATEN        = 3;
constexpr uint8_t CCEMPTY           = 4;
constexpr uint8_t CDFULL            = 5;

bool receive_flag = true;
char rx_buffer[256];

[[maybe_unused]] constexpr uint8_t kSetAddressDevice             = 0xD0;
[[maybe_unused]] constexpr uint8_t kConfigureDeviceDevice        = 0xD8;
[[maybe_unused]] constexpr uint8_t kSetModeDevice                = 0xF3;
[[maybe_unused]] constexpr uint8_t kReadCurrentFrameNumberDevice = 0xF5;
[[maybe_unused]] constexpr uint8_t kReadTestRegisterDevice       = 0xFD;
[[maybe_unused]] constexpr uint8_t kSetDeviceStatus              = 0xFE;
[[maybe_unused]] constexpr uint8_t kGetDeviceStatusDevice        = 0xFE;
[[maybe_unused]] constexpr uint8_t kClearBuffer                  = 0xF2;
[[maybe_unused]] constexpr uint8_t kGetErrorCodeDevice           = 0xFF;
[[maybe_unused]] constexpr uint8_t kReadErrorStatusDevice        = 0xFB;
[[maybe_unused]] constexpr uint8_t kValidateBuffer               = 0xFA;

namespace usb_
{
namespace type
{
constexpr uint8_t kVendorSpecific = 0xFF;
constexpr uint8_t kCdc            = 0x02;
constexpr uint8_t kCdcData        = 0x0a;
}  // namespace type
namespace subclass
{
constexpr uint8_t kAbstractControlModel = 0x02;
}
namespace protocol
{
constexpr uint8_t kV25ter = 0x01;
}
namespace endpoint
{
enum Direction : uint8_t
{
  kOut = 0,
  kIn  = 1,
};
enum TransferType : uint8_t
{
  kControl     = 0b00,
  kIsochronous = 0b01,
  kBulk        = 0b10,
  kInterrupt   = 0b11,
};
}  // namespace endpoint
}  // namespace usb_

// This is the standard device descriptor
struct [[gnu::packed]] DeviceDescriptor_t
{
  uint8_t length;
  uint8_t descriptor_type;
  uint16_t bcd_usb;
  uint8_t device_class;
  uint8_t device_subclass;
  uint8_t device_protocol;
  uint8_t max_packet_size;
  uint16_t id_vendor;
  uint16_t id_product;
  uint16_t bcdDevice;
  uint8_t manufacturer;
  uint8_t product;
  uint8_t serial_number;
  uint8_t num_configurations;
};

struct [[gnu::packed]] DeviceQualifier_t
{
  uint8_t length;
  uint8_t descriptor_type;
  uint16_t bcd_usb;
  uint8_t device_class;
  uint8_t device_subclass;
  uint8_t device_protocol;
  uint8_t max_packet_size;
  uint8_t num_configurations;
  uint8_t reserved;
};

struct [[gnu::packed]] ConfigurationDescriptor_t
{
  uint8_t length;
  uint8_t type;
  uint16_t total_length;
  uint8_t interface_count;
  uint8_t configuration_value;
  uint8_t configuration_string_index;
  uint8_t attributes;
  uint8_t max_power;
};

struct [[gnu::packed]] InterfaceDescriptor_t
{
  uint8_t length              = sizeof(InterfaceDescriptor_t);
  uint8_t type                = 0x4;
  uint8_t interface_number    = 0;
  uint8_t alternative_setting = 0;
  uint8_t endpoint_count;
  uint8_t interface_class    = 0xFF;
  uint8_t interface_subclass = 0;
  uint8_t interface_protocol = 0;
  uint8_t string_index       = 0;
};

struct [[gnu::packed]] EndpointDescriptor_t
{
  uint8_t length;
  uint8_t type = 0x5;
  union {
    uint8_t address;
    struct
    {
      uint8_t logical_address : 4;
      uint8_t reserved : 3;
      uint8_t direction : 1;
    } bit;
  } endpoint;
  union {
    uint8_t value;
    struct
    {
      uint8_t transfer_type : 2;
      uint8_t iso_synchronization : 2;
      uint8_t iso_usage : 2;
      uint8_t reserved : 2;
    } bit;
  } attributes;
  uint16_t max_packet_size;
  uint8_t interval;
};

struct [[gnu::packed]] StringDescriptor_t
{
  uint8_t buffer[256];
  uint8_t size;
  StringDescriptor_t(const char * str)
  {
    buffer[1]                     = 0x03;
    uint32_t length               = 2;
    uint8_t * buffer_string_start = &buffer[2];
    for (size_t i = 0; str[i] != 0; i++)
    {
      buffer_string_start[(i * 2)]     = str[i];
      buffer_string_start[(i * 2) + 1] = 0;
      length += 2;
    }
    buffer[0] = static_cast<uint8_t>(length & 0xff);
  }
};

struct [[gnu::packed]] StringLanguages_t
{
  static constexpr uint16_t kEnglishUS = 0x0409;

  uint8_t length    = sizeof(StringLanguages_t);
  uint8_t type      = 0x03;
  uint16_t language = kEnglishUS;
};

class UsbInterface
{
 public:
  enum class Direction : uint8_t
  {
    kOut = 0,
    kIn  = 1,
    kMaxDirections,
  };

  enum class SetupPacketType : uint8_t
  {
    kStandard = 0,
    kClass    = 1,
    kVendor   = 2,
    kMaxRequestTypes,
  };

  enum class Recipient : uint8_t
  {
    kDevice    = 0,
    kInterface = 1,
    kEndpoint  = 2,
    kMaxRecipients,
  };

  enum class DescriptorTypes
  {
    kDevice          = 1,
    kConfiguration   = 2,
    kString          = 3,
    kInterface       = 4,
    kEndpoint        = 5,
    kDeviceQualifier = 6,
  };

  enum Endpoint : uint8_t
  {
    kControl     = 0b00,
    kIsochronous = 0b01,
    kBulk        = 0b10,
    kInterrupt   = 0b11,
  };

  union [[gnu::packed]] RequestType_t {
    uint8_t raw;
    struct [[gnu::packed]]
    {
      unsigned recepient : 5;
      unsigned type : 2;
      unsigned direction : 1;
    } bit;
  };

  struct [[gnu::packed]] SetupPacket_t
  {
    RequestType_t request_type;
    uint8_t request;
    union {
      uint16_t raw;
      struct
      {
        uint8_t index;
        uint8_t type;
      } descriptor;
    } value;
    uint16_t index;
    uint16_t length;
  };

  using EndpointProcedure = Status (*)(UsbInterface * interface,
                                       const void * buffer, size_t length);
  using ControlProcedure  = Status (*)(UsbInterface * interface,
                                      const SetupPacket_t * packet);

  struct ControlProcedureMap_t
  {
    RequestType_t request_type;
    uint8_t request_number;
    ControlProcedure procedure;
  };

  struct EndpointInformation_t
  {
    Endpoint endpoint_type;
    Direction direction;
    size_t size;
    uint8_t endpoint_address;
    EndpointProcedure procedure;
  };

  static inline constexpr size_t kControlProcedures = 12;
  // x2 for both IN and OUT direction endpoints
  static inline constexpr size_t kMaxNumberOfEndpoints = 16 * 2;

  static inline const char * string_descriptor[] = {
    "San Jose State University CmpE Department", "SJTwo Development Board",
    "SJSU-Dev2-0"
  };

  // Set of virtual methods that a USB object should implement
  virtual void Initialize() = 0;
  virtual void Enable()     = 0;
  // virtual size_t ReadEndpoint(uint32_t endpoint, void * buffer,
  //                             size_t length)                               =
  //                             0;
  virtual size_t WriteEndpoint(uint32_t endpoint, const void * buffer,
                               size_t length)                              = 0;
  virtual bool RealizeEndpoint(EndpointInformation_t endpoint_information) = 0;
  virtual void EnableInterrupt()                                           = 0;
  virtual void Stall()                                                     = 0;
  virtual void SetDeviceAddress(uint8_t address)                           = 0;
  virtual void SetToConfiguredMode()                                       = 0;
  // Methods that use virtual methods to handle USB operations
  bool HasEnumerated() const
  {
    return usb_enumeration_complete;
  }
  void AddEndpoint(EndpointInformation_t endpoint_information)
  {
    endpoint_list.push_back(endpoint_information);
  }
  size_t WriteEndpoint(uint32_t endpoint, std::initializer_list<uint8_t> list)
  {
    return WriteEndpoint(endpoint, reinterpret_cast<const void *>(list.begin()),
                         list.size());
  }
  void SendZeroLengthPacket()
  {
    WriteEndpoint(0, nullptr, 0);
  }
  void LargeWriteEndpoint(uint32_t endpoint, const void * data, size_t length)
  {
    const uint8_t * payload      = reinterpret_cast<const uint8_t *>(data);
    bool send_zero_length_packet = ((length % 64) == 0);
    int32_t total_length         = length;

    for (size_t i = 0; total_length > 0; i++)
    {
      size_t loop_transmit_length = 64;
      if (total_length < 64)
      {
        loop_transmit_length = total_length;
      }
      WriteEndpoint(endpoint, &payload[i * 64], loop_transmit_length);
      total_length -= loop_transmit_length;
    }
    if (send_zero_length_packet)
    {
      SendZeroLengthPacket();
    }
  }

  Status GetDescriptor(const SetupPacket_t * setup_packet)
  {
    Status status = Status::kSuccess;

    DescriptorTypes descriptor =
        static_cast<DescriptorTypes>(setup_packet->value.descriptor.type);

    switch (descriptor)
    {
      case DescriptorTypes::kDevice:
      {
        size_t min_length =
            std::min({ static_cast<size_t>(setup_packet->length),
                       sizeof(device_descriptor) });
        WriteEndpoint(0, &device_descriptor, min_length);
        break;
      }
      case DescriptorTypes::kConfiguration:
      {
        uint8_t full_descriptor[256];

        // debug::Hexdump(full_descriptor, position);
        // If the host is asking to recieve a payload greater than or equal to
        // our configuration descriptor's total length then we should send the
        // rest of the information.
        if (setup_packet->length >= configuration_descriptor.total_length)
        {
          memcpy(full_descriptor, &configuration_descriptor,
               sizeof(configuration_descriptor));

          size_t position = sizeof(configuration_descriptor);

          for (size_t i = 0; i < descriptor_list.size(); i++)
          {
            memcpy(&full_descriptor[position], descriptor_list[i].data,
                  descriptor_list[i].size);
            position += descriptor_list[i].size;
          }
          LargeWriteEndpoint(0, full_descriptor, position);
        }
        else
        {
          size_t min_length =
              std::min({ static_cast<size_t>(setup_packet->length),
                         sizeof(configuration_descriptor) });
          LargeWriteEndpoint(0, &configuration_descriptor, min_length);
        }
        break;
      }
      case DescriptorTypes::kString:
      {
        if (setup_packet->value.descriptor.index == 0)
        {
          StringLanguages_t languages;
          WriteEndpoint(0, &languages, sizeof(languages));
        }
        else
        {
          StringDescriptor_t formatted_string_descriptor(
              string_descriptor[setup_packet->value.descriptor.index - 1]);
          LargeWriteEndpoint(0, formatted_string_descriptor.buffer,
                             formatted_string_descriptor.size);
        }
        break;
      }
      default:
      {
        status = Status::kNotHandled;
        break;
      }
    }
    return status;
  }

  Status SetAddress(const SetupPacket_t * setup_packet)
  {
    uint8_t address = static_cast<uint8_t>(setup_packet->value.raw);
    SetDeviceAddress(address);
    SendZeroLengthPacket();
    return Status::kSuccess;
  }

  Status SetConfiguration(const SetupPacket_t * setup_packet)
  {
    configuration_number = setup_packet->value.raw;
    for (size_t i = 0; i < endpoint_list.size(); i++)
    {
      RealizeEndpoint(endpoint_list[i]);
    }
    SetToConfiguredMode();
    usb_enumeration_complete = true;
    SendZeroLengthPacket();
    return Status::kSuccess;
  }

  Status SetFeature([[maybe_unused]] const SetupPacket_t * setup_packet)
  {
    SendZeroLengthPacket();
    return Status::kSuccess;
  }

  void ControlEndpointHandler(const void * buffer)
  {
    // putchar('V');
    const SetupPacket_t * setup_packet =
        reinterpret_cast<const SetupPacket_t *>(buffer);

    Status status = Status::kNotHandled;

    SJ2_PRINT_VARIABLE(control_procedure.size(), "%zu");

    for (size_t i = 0; i < control_procedure.size(); i++)
    {
      SJ2_PRINT_VARIABLE(control_procedure[i].request_type.raw, "0x%02X");
      SJ2_PRINT_VARIABLE(control_procedure[i].request_number, "0x%02X");
      SJ2_PRINT_VARIABLE(setup_packet->request_type.raw, "0x%02X");
      SJ2_PRINT_VARIABLE(setup_packet->request, "0x%02X");

      if (control_procedure[i].request_type.raw ==
              setup_packet->request_type.raw &&
          control_procedure[i].request_number == setup_packet->request)
      {
        // printf(":%zu:", i);
        if (control_procedure[i].procedure != nullptr)
        {
          status = control_procedure[i].procedure(this, setup_packet);
          if (status == Status::kSuccess)
          {
            break;
          }
        }
        else
        {
          putchar('$');
        }
      }
    }

    if (status == Status::kNotHandled)
    {
      Stall();
    }
  }

  void AddStandardControlProcedures()
  {
    constexpr uint8_t kSetAddress       = 0x05;
    constexpr uint8_t kSetFeature       = 0x02;
    constexpr uint8_t kGetDescriptor    = 0x06;
    constexpr uint8_t kSetConfiguration = 0x09;

    // clang-format off
    const ControlProcedureMap_t GetDescriptorControlProcedure = {
      .request_type = {
        .bit = {
          .recepient = static_cast<uint8_t>(Recipient::kDevice),
          .type = static_cast<uint8_t>(SetupPacketType::kStandard),
          .direction = static_cast<uint8_t>(Direction::kIn),
        },
      },
      .request_number = kGetDescriptor,
      .procedure = [](UsbInterface * interface,
                      const SetupPacket_t * setup_packet) -> Status
      {
        return interface->GetDescriptor(setup_packet);
      },
    };

    const ControlProcedureMap_t SetAddressControlProcedure = {
      .request_type = {
        .bit = {
            .recepient = static_cast<uint8_t>(Recipient::kDevice),
            .type = static_cast<uint8_t>(SetupPacketType::kStandard),
            .direction = static_cast<uint8_t>(Direction::kOut),
        },
      },
      .request_number = kSetAddress,
      .procedure = [](UsbInterface * interface,
                      const SetupPacket_t * setup_packet) -> Status
      {
        return interface->SetAddress(setup_packet);
      },
    };

    const ControlProcedureMap_t SetConfigurationControlProcedure = {
      .request_type = {
        .bit = {
            .recepient = static_cast<uint8_t>(Recipient::kDevice),
            .type = static_cast<uint8_t>(SetupPacketType::kStandard),
            .direction = static_cast<uint8_t>(Direction::kOut),
        },
      },
      .request_number = kSetConfiguration,
      .procedure = [](UsbInterface * interface,
                      const SetupPacket_t * setup_packet) -> Status
      {
        return interface->SetConfiguration(setup_packet);
      },
    };

    const ControlProcedureMap_t SetFeatureControlProcedure = {
      .request_type = {
        .bit = {
            .recepient = static_cast<uint8_t>(Recipient::kDevice),
            .type = static_cast<uint8_t>(SetupPacketType::kStandard),
            .direction = static_cast<uint8_t>(Direction::kOut),
        },
      },
      .request_number = kSetFeature,
      .procedure = [](UsbInterface * interface,
                      const SetupPacket_t * setup_packet) -> Status
      {
        return interface->SetFeature(setup_packet);
      },
    };
    // clang-format on

    control_procedure.push_back(GetDescriptorControlProcedure);
    control_procedure.push_back(SetAddressControlProcedure);
    control_procedure.push_back(SetConfigurationControlProcedure);
    control_procedure.push_back(SetFeatureControlProcedure);
  }

  bool AddControlProcedure(ControlProcedureMap_t control_procedure_map)
  {
    control_procedure.push_back(control_procedure_map);
    return true;
  }

  struct Descriptor_t
  {
    const void * data;
    size_t size;
  };

  static inline constexpr size_t kMaxDescriptors = 10;

  static size_t CountInterfaces(const Descriptor_t & descriptor)
  {
    size_t number_of_interfaces = 0;
    const uint8_t * array       = static_cast<const uint8_t *>(descriptor.data);
    for (size_t position = 0; position < descriptor.size;)
    {
      // The first byte of all descriptors is the length of the descriptor
      size_t leap = array[position];
      // The following byte for all descriptors is the descriptor type id
      uint8_t descriptor_type = array[position + 1];
      if (descriptor_type == static_cast<uint8_t>(DescriptorTypes::kInterface))
      {
        number_of_interfaces++;
      }
      position += leap;
    }
    return number_of_interfaces;
  }
  bool AddDescriptor(Descriptor_t descriptor)
  {
    descriptor_list.push_back(descriptor);
    configuration_descriptor.interface_count += CountInterfaces(descriptor);
    configuration_descriptor.total_length += descriptor.size;
    return true;
  }
  const DeviceDescriptor_t & GetDeviceDescriptor()
  {
    return device_descriptor;
  }
  const ConfigurationDescriptor_t & GetConfigurationDescriptor()
  {
    return configuration_descriptor;
  }

 protected:
  etl::vector<ControlProcedureMap_t, kControlProcedures> control_procedure;
  static inline etl::vector<EndpointInformation_t, kMaxNumberOfEndpoints>
      endpoint_list;
  etl::vector<Descriptor_t, kMaxDescriptors> descriptor_list;

  const DeviceDescriptor_t device_descriptor = {
    .length             = sizeof(device_descriptor),
    .descriptor_type    = 0x01,
    .bcd_usb            = 0x0110,
    .device_class       = 0,
    .device_subclass    = 0,
    .device_protocol    = 0,
    .max_packet_size    = 64,
    .id_vendor          = 0x1337,
    .id_product         = 0xB055,
    .bcdDevice          = 0x0100,
    .manufacturer       = 1,
    .product            = 2,
    .serial_number      = 0,
    .num_configurations = 1,
  };

  ConfigurationDescriptor_t configuration_descriptor = {
    .length = sizeof(ConfigurationDescriptor_t),
    .type   = 0x02,
    // Updated by AddDescriptor to actual value
    .total_length = sizeof(ConfigurationDescriptor_t),
    // Updated by AddDescriptor to actual value
    .interface_count            = 0,
    .configuration_value        = 1,
    .configuration_string_index = 0,
    .attributes                 = 0,
    .max_power                  = 250,
  };

  const EndpointInformation_t control_recieve_procedure = {
    .endpoint_type    = Endpoint::kControl,
    .direction        = Direction::kOut,
    .size             = 64,
    .endpoint_address = 0,
    .procedure        = [](UsbInterface * usb_interface, const void * buffer,
                    size_t length) -> Status {
      usb_interface->ControlEndpointHandler(buffer);
      return Status::kSuccess;
    },
  };
  const EndpointInformation_t control_transmit_procedure = {
    .endpoint_type    = Endpoint::kControl,
    .direction        = Direction::kIn,
    .size             = 64,
    .endpoint_address = 0,
    .procedure        = nullptr,
  };
  uint16_t configuration_number = 0;
  static inline bool usb_enumeration_complete = false;
};

class Usb final : public UsbInterface, protected Lpc40xxSystemController
{
 public:
  static inline constexpr size_t kNumberOfEndpoints = 16 * 2;

  static inline LPC_USB_TypeDef * usb_register = LPC_USB;
  static inline Usb * default_usb              = nullptr;

  union [[gnu::packed]] UsbControl {
    struct [[gnu::packed]]
    {
      uint32_t read_enable : 1;
      uint32_t write_enable : 1;
      uint32_t logical_endpoint : 4;
    } bit;
    uint32_t data;
  };

  union [[gnu::packed]] RecievePacketLength {
    struct [[gnu::packed]]
    {
      uint32_t packet_length : 10;
      uint32_t is_data_valid : 1;
      uint32_t packet_read : 1;
    } bit;
    uint32_t data;
  };

  Usb()
  {
    default_usb = this;
  }
  void Initialize() override
  {
    SetAlternatePll(Lpc40xxSystemController::PllInput::kF12MHz, 48);
    SelectUsbClockSource(
        Lpc40xxSystemController::UsbSource::kAlternatePllClock);
    SelectUsbClockDivider(Lpc40xxSystemController::UsbDivider::kDivideBy1);
    PowerUpPeripheral(Lpc40xxSystemController::Peripherals::kUsb);

    usb_register->USBClkCtrl =
        bit::Set(Usb::usb_register->USBClkCtrl, DEV_CLK_EN);

    Wait(kMaxWait, []() -> bool {
      return bit::Read(Usb::usb_register->USBClkSt, DEV_CLK_EN);
    });

    usb_register->USBClkCtrl =
        bit::Set(Usb::usb_register->USBClkCtrl, AHB_CLK_EN);

    Wait(kMaxWait, []() -> bool {
      return bit::Read(Usb::usb_register->USBClkSt, AHB_CLK_EN);
    });

    Pin usb_dplus(0, 29);
    Pin usb_dminus(0, 30);
    Pin usb_vbus(1, 30);

    usb_dplus.SetPinFunction(0b001);
    usb_dminus.SetPinFunction(0b001);
    usb_vbus.SetMode(Pin::Mode::kInactive);
    usb_vbus.SetPinFunction(0b010);

    RealizeEndpoint(control_recieve_procedure);
    RealizeEndpoint(control_transmit_procedure);

    constexpr size_t k8Bits = 8;

    for (size_t i = 0; i < sizeof(usb_register->EpIntSt) * k8Bits; i++)
    {
      if (usb_register->EpIntSt & 1 << i)
      {
        usb_register->EpIntClr = 1 << i;
        Wait(kMaxWait, []() -> bool {
          return bit::Read(usb_register->DevIntSt, CDFULL);
        });
      }
    }

    for (size_t i = 0; i < sizeof(usb_register->DevIntSt) * k8Bits; i++)
    {
      if (usb_register->DevIntSt & 1 << i)
      {
        usb_register->DevIntClr = 1 << i;
      }
    }
    usb_register->EpIntPri = 0;
    // TODO(kammce): Ignoring SET MODE instruction for now as the defaults
    // should be fine.
    usb_register->DevIntEn = bit::Set(usb_register->DevIntEn, EP_SLOWEN);
    usb_register->DevIntEn = bit::Set(usb_register->DevIntEn, DEV_STATEN);
    usb_register->DevIntEn = bit::Set(usb_register->DevIntEn, 9);
  }
  void Enable() override
  {
    SerialInterfaceEngineWrite(kSetAddressDevice, 0b1000'0000, true);
    SerialInterfaceEngineWrite(kSetDeviceStatus, 0b0000'0001, true);
    SerialInterfaceEngineWrite(kSetModeDevice, 0b0000'0001, true);
    SerialInterfaceEngineWrite(kConfigureDeviceDevice, 0b0000'0001, true);
  }
  static void UsbInterruptServiceRoutine()
  {
    // putchar('.');
    uint8_t buffer[64] = { 0 };
    if (usb_register->EpIntSt)
    {
      if (usb_register->EpIntSt & 1)
      {
        size_t length = ReadEndpoint(0, buffer, sizeof(buffer));
        default_usb->control_recieve_procedure.procedure(default_usb, buffer,
                                                         length);
        ClearEndpointInterrupt(0);
      }
      for (size_t i = 2; i < 32; i++)
      {
        if (usb_register->EpIntSt & (1 << i))
        {
          printf(SJ2_HI_BACKGROUND_GREEN "\n1 << %d\n" SJ2_COLOR_RESET, i);
          if (endpoint_list[i].procedure != nullptr)
          {
            memset(buffer, 0, sizeof(buffer));
            size_t length = ReadEndpoint(static_cast<uint16_t>(i >> 1), buffer,
                                         sizeof(buffer));
            endpoint_list[i].procedure(default_usb, buffer, length);
          }
          ClearEndpointInterrupt(i);
        }
      }
    }

    for (size_t i = 0; i < 10; i++)
    {
      if (usb_register->DevIntSt & 1 << i)
      {
        switch (i)
        {
          case 3:
          {
            [[maybe_unused]] uint16_t status =
                SerialInterfaceEngineRead(kGetDeviceStatusDevice);
            LOG_DEBUG("Device Change Status: 0x%02X", status);
            usb_enumeration_complete = false;
            break;
          }
          case 9:
          {
            uint16_t error      = SerialInterfaceEngineRead(0xFB);
            uint16_t error_code = SerialInterfaceEngineRead(0xFF);
            if (!(error & (1 << 1)))
            {
              LOG_WARNING("Error: 0x%02X", error);
              LOG_WARNING("Error Code: 0x%02X", error_code);
            }
            break;
          }
          default:
            LOG_DEBUG("Device Interrupt Status %zu Cleared!", i);
        }
        usb_register->DevIntClr = 1 << i;
      }
    }
    // putchar('_');
  }
  void EnableInterrupt() override
  {
    RegisterIsr(USB_IRQn, UsbInterruptServiceRoutine);
  }

  static uint8_t PhysicalEndpoint(uint32_t endpoint, Direction direction)
  {
    uint32_t direction_value = static_cast<uint32_t>(direction);
    return static_cast<uint8_t>((endpoint * 2) + direction_value);
  }

  static size_t ReadEndpoint(uint32_t endpoint, void * buffer, size_t length)
  {
    uint8_t physical_endpoint = PhysicalEndpoint(endpoint, Direction::kOut);
    if (!(usb_register->ReEp & (1 << physical_endpoint)))
    {
      LOG_WARNING("RX) Endpoint %" PRIu32 " PHY: %" PRIu8 " not realized",
                  endpoint, physical_endpoint);
      return 0;
    }
    LOG_DEBUG("RX EP%lu", endpoint);
    UsbControl enable_endpoint_read = { .bit = { .read_enable  = 1,
                                                 .write_enable = 0,
                                                 .logical_endpoint =
                                                     endpoint & 0xF } };

    usb_register->Ctrl = enable_endpoint_read.data;
    size_t RxPLen      = usb_register->RxPLen;
    bool packet_ready  = (RxPLen & (1 << 11));
    if (!packet_ready)
    {
      return 0;
    }
    size_t bytes       = (RxPLen & 0b1'1111'1111);
    size_t bytes_ceil  = (bytes > 64) ? 64 : bytes;
    size_t buffer_size = bytes_ceil;

    uint32_t * buffer_32bit = static_cast<uint32_t *>(buffer);
    // TODO(kammce): Badly needs to be cleaned up. The ANDing cannot continue!
    for (size_t i = 0; bytes != 0; i++)
    {
      buffer_32bit[i] = usb_register->RxData;
      if (bytes == 3)
      {
        buffer_32bit[i] &= 0xFFFFFF;
      }
      else if (bytes == 2)
      {
        buffer_32bit[i] &= 0xFFFF;
      }
      else if (bytes == 1)
      {
        buffer_32bit[i] &= 0xFF;
      }
      bytes -= (bytes >= 4) ? 4 : bytes % 4;
    }
    usb_register->Ctrl = 0;
    ClearEndpointInterrupt(endpoint);

    [[maybe_unused]] uint16_t endpoint_status =
        SerialInterfaceEngineRead(physical_endpoint);
    SerialInterfaceEngineRead(kClearBuffer);
    return buffer_size;
  }
  size_t WriteEndpoint(uint32_t endpoint, const void * buffer,
                       size_t length) override
  {
    constexpr uint16_t kPacketOverwrittenBySetupPacket = 1 << 3;
    const uint32_t * data = reinterpret_cast<const uint32_t *>(buffer);

    uint8_t physical_endpoint = PhysicalEndpoint(endpoint, Direction::kIn);
    if (!(usb_register->ReEp & (1 << physical_endpoint)))
    {
      LOG_WARNING("TX) Endpoint %" PRIu32 " PHY: %" PRIu8 " not realized",
                  endpoint, physical_endpoint);
      return 0;
    }
    UsbControl enable_endpoint_write = { .bit = { .read_enable  = 0,
                                                  .write_enable = 1,
                                                  .logical_endpoint =
                                                      endpoint & 0xF } };
    uint16_t status = SerialInterfaceEngineRead(physical_endpoint);
    if (status & kPacketOverwrittenBySetupPacket)
    {
      return 0;
    }
    usb_register->Ctrl   = enable_endpoint_write.data;
    usb_register->TxPLen = length;
    size_t new_length    = 0;
    for (size_t i = 0; new_length < length; i++)
    {
      usb_register->TxData = data[i];
      new_length += 4;
    }
    SerialInterfaceEngineWrite(kValidateBuffer);
    usb_register->Ctrl = 0;

    Wait(2000, [physical_endpoint]() -> bool {
      return bit::Read(usb_register->EpIntSt, physical_endpoint);
    });

    ClearEndpointInterrupt(physical_endpoint);
  }
  bool RealizeEndpoint(EndpointInformation_t endpoint_information) override
  {
    uint32_t physical_endpoint = PhysicalEndpoint(
        endpoint_information.endpoint_address, endpoint_information.direction);

    usb_register->DevIntClr |= 1 << kEndpointRealized;
    usb_register->ReEp |= 1 << physical_endpoint;
    usb_register->EpInd    = physical_endpoint;
    usb_register->MaxPSize = endpoint_information.size;

    Wait(kMaxWait, []() -> bool {
      return bit::Read(usb_register->DevIntSt, kEndpointRealized);
    });

    usb_register->EpIntEn |= 1 << physical_endpoint;
    usb_register->DevIntClr |= 1 << kEndpointRealized;
    uint8_t set_endpoint_mode_command =
        static_cast<uint8_t>(0x40 + physical_endpoint);
    SerialInterfaceEngineWrite(set_endpoint_mode_command, 0, true);
    SerialInterfaceEngineWrite(kClearBuffer);

    return true;
  }
  void Stall()
  {
    // TODO(kammce): expand this to work with any endpoint!
    SerialInterfaceEngineWrite(0x40, 1, true);
  }
  void SetDeviceAddress(uint8_t address) override
  {
    address = static_cast<uint8_t>(bit::Set(address, 7));
    // Set Address using the serial inte
    SerialInterfaceEngineWrite(kSetAddressDevice, address, true);
  }
  void SetToConfiguredMode() override
  {
    SerialInterfaceEngineWrite(kConfigureDeviceDevice, 0b0000'0001, true);
  }

 private:
  static void ClearEndpointInterrupt(uint32_t endpoint)
  {
    if (usb_register->EpIntSt & (1 << endpoint))
    {
      usb_register->EpIntClr = 1 << endpoint;
      while (!bit::Read(usb_register->DevIntSt, CDFULL))
      {
        continue;
      }
    }
  }

  static void SerialInterfaceEngineWrite(uint8_t command, uint8_t data = 0,
                                         bool data_present = false)
  {
    usb_register->DevIntClr = (1 << CCEMPTY);
    usb_register->CmdCode   = (command << 16) | (0x05 << 8);
    while (!bit::Read(usb_register->DevIntSt, CCEMPTY))
    {
      continue;
    }
    usb_register->DevIntClr = (1 << CCEMPTY);
    if (data_present)
    {
      usb_register->CmdCode = (data << 16) | (0x01 << 8);
      while (!bit::Read(usb_register->DevIntSt, CCEMPTY))
      {
        continue;
      }
      usb_register->DevIntClr = (1 << CCEMPTY);
    }
  }

  static uint16_t SerialInterfaceEngineRead(uint8_t command,
                                            bool two_bytes = false)
  {
    uint16_t result         = 0;
    usb_register->DevIntClr = (1 << CCEMPTY) | (1 << CDFULL);
    usb_register->CmdCode   = (command << 16) | (0x05 << 8);
    while (!bit::Read(usb_register->DevIntSt, CCEMPTY))
    {
      continue;
    }
    usb_register->DevIntClr = (1 << CCEMPTY);
    usb_register->CmdCode   = (command << 16) | (0x02 << 8);
    while (!bit::Read(usb_register->DevIntSt, CDFULL))
    {
      continue;
    }
    usb_register->DevIntClr = (1 << CDFULL);

    result = static_cast<uint16_t>(usb_register->CmdData & 0xFF);

    if (two_bytes)
    {
      usb_register->CmdCode = (command << 16) | (0x02 << 8);
      while (!bit::Read(usb_register->DevIntSt, CDFULL))
      {
        continue;
      }
      usb_register->DevIntClr = (1 << CDFULL);

      result =
          static_cast<uint16_t>(result | ((usb_register->CmdCode & 0xFF) << 8));
    }
    return result;
  }
};

class UsbSerialPort
{
 public:
  static inline constexpr uint8_t kCsInterface = 0x24;
  static inline constexpr uint8_t kCsEndpoint  = 0x25;

  struct [[gnu::packed]] CdcFunctionalHeaderDescriptor_t
  {
    uint8_t function_length;
    uint8_t descriptor_type;
    uint8_t descriptor_subtype;
    uint16_t bcdCDC;
  };

  struct [[gnu::packed]] CdcCallManagementDesciptor_t
  {
    uint8_t function_length;
    uint8_t descriptor_type;
    uint8_t descriptor_subtype;
    uint8_t capabilities;
    uint8_t data_interface;
  };

  struct [[gnu::packed]] CdcAbstractControlModelFunctionDescriptor_t
  {
    uint8_t function_length;
    uint8_t descriptor_type;
    uint8_t descriptor_subtype;
    uint8_t capabilities;
  };

  struct [[gnu::packed]] CdcUnionFunctionDescriptor_t
  {
    uint8_t function_length;
    uint8_t descriptor_type;
    uint8_t descriptor_subtype;
    uint8_t master_interface;
    uint8_t slave_interface;
  };

  struct [[gnu::packed]] VirtualComPortDescriptor_t
  {
    InterfaceDescriptor_t communication;
    CdcFunctionalHeaderDescriptor_t header;
    CdcAbstractControlModelFunctionDescriptor_t control;
    CdcUnionFunctionDescriptor_t union_function;
    CdcCallManagementDesciptor_t management;
    EndpointDescriptor_t interrupt;
    InterfaceDescriptor_t data;
    EndpointDescriptor_t transmit;
    EndpointDescriptor_t receive;
  };
  // clang-format off
  static inline const VirtualComPortDescriptor_t
      virtual_serial_port_descriptor = {
    .communication =
    {
      .length              = sizeof(InterfaceDescriptor_t),
      .type                = 0x4,
      .interface_number    = 0,
      .alternative_setting = 0,
      .endpoint_count      = 1,
      .interface_class     = usb_::type::kCdc,
      .interface_subclass  = usb_::subclass::kAbstractControlModel,
      .interface_protocol  = usb_::protocol::kV25ter,
      .string_index        = 0,
    },
    .header = {
      .function_length = sizeof(CdcFunctionalHeaderDescriptor_t),
      .descriptor_type = kCsInterface,
      .descriptor_subtype = 0x00,
      .bcdCDC = 0x0110,
    },
    .control = {
      .function_length = sizeof(CdcAbstractControlModelFunctionDescriptor_t),
      .descriptor_type = kCsInterface,
      .descriptor_subtype = 0x02,
      .capabilities = 0x00,
    },
    .union_function = {
      .function_length = sizeof(CdcUnionFunctionDescriptor_t),
      .descriptor_type = kCsInterface,
      .descriptor_subtype = 0x06,
      .master_interface = 0x00,
      .slave_interface = 0x01,
    },
    .management = {
      .function_length = sizeof(CdcCallManagementDesciptor_t),
      .descriptor_type = kCsInterface,
      .descriptor_subtype = 0x01,
      .capabilities = 0b0000'0000,
      .data_interface = 0x01,
    },
    .interrupt =
    {
      .length          = sizeof(EndpointDescriptor_t),
      .type            = 0x5,
      .endpoint        = {
        .bit = {
          .logical_address = 1,
          .reserved        = 0,
          .direction       = usb_::endpoint::Direction::kIn,
        }
      },
      .attributes      = {
        .bit = {
          .transfer_type = usb_::endpoint::TransferType::kInterrupt,
          .iso_synchronization = 0,
          .iso_usage = 0,
          .reserved = 0,
        }
      },
      .max_packet_size = 64,
      .interval        = 0x02,
    },
    .data =
    {
      .length              = sizeof(InterfaceDescriptor_t),
      .type                = 0x4,
      .interface_number    = 1,
      .alternative_setting = 0,
      .endpoint_count      = 2,
      .interface_class     = usb_::type::kCdcData,
      .interface_subclass  = 0,
      .interface_protocol  = 0,
      .string_index        = 0,
    },
    .transmit =
    {
      .length          = sizeof(EndpointDescriptor_t),
      .type            = 0x5,
      .endpoint        = {
        .bit = {
          .logical_address = 2,
          .reserved        = 0,
          .direction       = usb_::endpoint::Direction::kIn,
        }
      },
      .attributes      = {
        .bit = {
          .transfer_type = usb_::endpoint::TransferType::kBulk,
          .iso_synchronization = 0,
          .iso_usage = 0,
          .reserved = 0,
        }
      },
      .max_packet_size = 64,
      .interval        = 0,
    },
    .receive =
    {
      .length          = sizeof(EndpointDescriptor_t),
      .type            = 0x5,
      .endpoint        = {
        .bit = {
          .logical_address = 2,
          .reserved        = 0,
          .direction       = usb_::endpoint::Direction::kOut,
        }
      },
      .attributes      = {
        .bit = {
          .transfer_type = usb_::endpoint::TransferType::kBulk,
          .iso_synchronization = 0,
          .iso_usage = 0,
          .reserved = 0,
        }
      },
      .max_packet_size = 64,
      .interval        = 0,
    },
  };
  // clang-format on
 private:
};

class UsbHumanInterfaceDevice
{
 public:
  struct [[gnu::packed]] HIDClassDescriptor_t
  {
    uint8_t length;
    uint8_t type;
    uint16_t release_number;
    uint8_t localization_contry_code;
    uint8_t hid_class_descriptor_count;
    uint8_t report_descriptor_type;
    uint16_t report_descriptor_length;
  };

  struct [[gnu::packed]] HIDDescriptor_t
  {
    InterfaceDescriptor_t hid_interface;
    HIDClassDescriptor_t hid_class;
    EndpointDescriptor_t hid_endpoint;
  };

 private:
};

const char hid_mouse_descriptor[50] = {
  0x05, 0x01,  // USAGE_PAGE (Generic Desktop)
  0x09, 0x02,  // USAGE (Mouse)
  0xa1, 0x01,  // COLLECTION (Application)
  0x09, 0x01,  //   USAGE (Pointer)
  0xa1, 0x00,  //   COLLECTION (Physical)
  0x05, 0x09,  //     USAGE_PAGE (Button)
  0x19, 0x01,  //     USAGE_MINIMUM (Button 1)
  0x29, 0x03,  //     USAGE_MAXIMUM (Button 3)
  0x15, 0x00,  //     LOGICAL_MINIMUM (0)
  0x25, 0x01,  //     LOGICAL_MAXIMUM (1)
  0x95, 0x03,  //     REPORT_COUNT (3)
  0x75, 0x01,  //     REPORT_SIZE (1)
  0x81, 0x02,  //     INPUT (Data,Var,Abs)
  0x95, 0x01,  //     REPORT_COUNT (1)
  0x75, 0x05,  //     REPORT_SIZE (5)
  0x81, 0x03,  //     INPUT (Cnst,Var,Abs)
  0x05, 0x01,  //     USAGE_PAGE (Generic Desktop)
  0x09, 0x30,  //     USAGE (X)
  0x09, 0x31,  //     USAGE (Y)
  0x15, 0x81,  //     LOGICAL_MINIMUM (-127)
  0x25, 0x7f,  //     LOGICAL_MAXIMUM (127)
  0x75, 0x08,  //     REPORT_SIZE (8)
  0x95, 0x02,  //     REPORT_COUNT (2)
  0x81, 0x06,  //     INPUT (Data,Var,Rel)
  0xc0,        //   END_COLLECTION
  0xc0         // END_COLLECTION
};

struct JoystickReport_t
{
  uint8_t throttle;
  uint8_t x;
  uint8_t y;
  uint8_t rotary : 4;
  uint8_t button0 : 1;
  uint8_t button1 : 1;
  uint8_t button2 : 1;
  uint8_t button3 : 1;
};

const char hid_joystick_descriptor[77] = {
  0x05, 0x01,        // USAGE_PAGE (Generic Desktop)
  0x15, 0x00,        // LOGICAL_MINIMUM (0)
  0x09, 0x04,        // USAGE (Joystick)
  0xa1, 0x01,        // COLLECTION (Application)
  0x05, 0x02,        //   USAGE_PAGE (Simulation Controls)
  0x09, 0xbb,        //   USAGE (Throttle)
  0x15, 0x81,        //   LOGICAL_MINIMUM (-127)
  0x25, 0x7f,        //   LOGICAL_MAXIMUM (127)
  0x75, 0x08,        //   REPORT_SIZE (8)
  0x95, 0x01,        //   REPORT_COUNT (1)
  0x81, 0x02,        //   INPUT (Data,Var,Abs)
  0x05, 0x01,        //   USAGE_PAGE (Generic Desktop)
  0x09, 0x01,        //   USAGE (Pointer)
  0xa1, 0x00,        //   COLLECTION (Physical)
  0x09, 0x30,        //     USAGE (X)
  0x09, 0x31,        //     USAGE (Y)
  0x95, 0x02,        //     REPORT_COUNT (2)
  0x81, 0x02,        //     INPUT (Data,Var,Abs)
  0xc0,              //   END_COLLECTION
  0x09, 0x39,        //   USAGE (Hat switch)
  0x15, 0x00,        //   LOGICAL_MINIMUM (0)
  0x25, 0x03,        //   LOGICAL_MAXIMUM (3)
  0x35, 0x00,        //   PHYSICAL_MINIMUM (0)
  0x46, 0x0e, 0x01,  //   PHYSICAL_MAXIMUM (270)
  0x65, 0x14,        //   UNIT (Eng Rot:Angular Pos)
  0x75, 0x04,        //   REPORT_SIZE (4)
  0x95, 0x01,        //   REPORT_COUNT (1)
  0x81, 0x02,        //   INPUT (Data,Var,Abs)
  0x05, 0x09,        //   USAGE_PAGE (Button)
  0x19, 0x01,        //   USAGE_MINIMUM (Button 1)
  0x29, 0x04,        //   USAGE_MAXIMUM (Button 4)
  0x15, 0x00,        //   LOGICAL_MINIMUM (0)
  0x25, 0x01,        //   LOGICAL_MAXIMUM (1)
  0x75, 0x01,        //   REPORT_SIZE (1)
  0x95, 0x04,        //   REPORT_COUNT (4)
  0x55, 0x00,        //   UNIT_EXPONENT (0)
  0x65, 0x00,        //   UNIT (None)
  0x81, 0x02,        //   INPUT (Data,Var,Abs)
  0xc0               // END_COLLECTION
};

struct [[gnu::packed]] MouseReport_t
{
  uint8_t buttons;
  int8_t x;
  int8_t y;
};

struct [[gnu::packed]] AcmNotification
{
  uint8_t type;
  uint8_t notification_code;
  uint16_t value;
  uint16_t index;
  uint16_t length;
};

void vSendByte(void *ptr)
{

  Usb usb;
  usb.AddStandardControlProcedures();
  usb.AddDescriptor(
      { .data = &UsbSerialPort::virtual_serial_port_descriptor,
        .size = sizeof(UsbSerialPort::virtual_serial_port_descriptor) });
  usb.AddEndpoint({
      .endpoint_type    = Usb::Endpoint::kBulk,
      .direction        = Usb::Direction::kOut,
      .size             = 64,
      .endpoint_address = 2,
      .procedure        = [](UsbInterface * interface, const void * buffer,
                      size_t length) -> Status {
        printf("%.*s", length, buffer);
        return Status::kSuccess;
      },
  });
  usb.AddEndpoint({
      .endpoint_type    = Usb::Endpoint::kBulk,
      .direction        = Usb::Direction::kIn,
      .size             = 64,
      .endpoint_address = 2,
      .procedure        = nullptr,
  });
  usb.Initialize();
  usb.EnableInterrupt();
  usb.Enable();

  while (true)
  {
    if (usb.HasEnumerated())
    {
      const char text[] = "Write Anything>\n";
      usb.WriteEndpoint(2, text, sizeof(text));
      vTaskDelay(5000);
    }
  }
}

int main(void)
{
  // LOG_WARNING("Clk Status = 0x%02X",
  //             SerialInterfaceEngineRead(kReadTestRegisterDevice, true));
  // LOG_WARNING("Dev Status = 0x%02X",
  //             SerialInterfaceEngineRead(kGetDeviceStatusDevice));
  // LOG_WARNING("Err Status = 0x%02X",
  //             SerialInterfaceEngineRead(kGetErrorCodeDevice));

  LOG_CRITICAL("Halting all Actions...");
  xTaskCreate(vSendByte, "USB Send", 2048, nullptr, rtos::kHigh, nullptr);
  vTaskStartScheduler();
  Halt();
  return 0;
}
