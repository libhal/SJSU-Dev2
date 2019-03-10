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

#include "utility/log.hpp"
#include "utility/bit.hpp"
#include "utility/time.hpp"

#include "utility/rtos.hpp"

LPC_USB_TypeDef * usb = LPC_USB;

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
struct [[gnu::packed]] UsbDeviceDescription_t
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

struct [[gnu::packed]] UsbDeviceQualifier_t
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

union [[gnu::packed]] StringDescriptor_t {
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
    } bitmap;
  } endpoint;
  union {
    uint8_t value;
    struct
    {
      uint8_t transfer_type : 2;
      uint8_t iso_synchronization : 2;
      uint8_t iso_usage : 2;
      uint8_t reserved : 2;
    } bitmap;
  } attributes;
  uint16_t max_packet_size;
  uint8_t interval;
};

struct [[gnu::packed]] LinkedConfig_t
{
  ConfigurationDescriptor_t config;
  InterfaceDescriptor_t interface;
  EndpointDescriptor_t endpoint;
};

const uint8_t kUsbDeviceClassCommunications    = 0x02;
const UsbDeviceDescription_t device_descriptor = {
  .length             = sizeof(device_descriptor),
  .descriptor_type    = 0x01,
  .bcd_usb            = 0x0110,
  .device_class       = usb_::type::kCdc,
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

LinkedConfig_t omni = {
  .config =
      {
          .length                     = sizeof(ConfigurationDescriptor_t),
          .type                       = 0x02,
          .total_length               = sizeof(omni),
          .interface_count            = 1,
          .configuration_value        = 1,
          .configuration_string_index = 3,
          .attributes                 = 0b1000'0000,
          .max_power                  = 250,
      },
  .interface =
      {
          .length              = sizeof(InterfaceDescriptor_t),
          .type                = 0x4,
          .interface_number    = 0,
          .alternative_setting = 0,
          .endpoint_count      = 1,
          .interface_class     = 0xFF,
          .interface_subclass  = 0,
          .interface_protocol  = 0,
          .string_index        = 0,
      },
  .endpoint =
      {
          .length          = sizeof(EndpointDescriptor_t),
          .type            = 0x5,
          .endpoint        = { .bitmap = { .logical_address = 2,
                                    .reserved        = 0,
                                    .direction       = 0 } },
          .attributes      = { .bitmap =
                              {
                                  .transfer_type =
                                      usb_::endpoint::TransferType::kBulk,
                                  .iso_synchronization = 0,
                                  .iso_usage           = 0,
                                  .reserved            = 0,
                              } },
          .max_packet_size = 64,
          .interval        = 1,
      },
};

// The CDC functional header
struct [[gnu::packed]] CdcFunctionalHeaderDescriptor_t
{
  uint8_t function_length;
  uint8_t descriptor_type;
  uint8_t descriptor_subtype;
  uint16_t bcdCDC;
};

// The CDC call management descriptor
struct [[gnu::packed]] CdcCallManagementDesciptor_t
{
  uint8_t function_length;
  uint8_t descriptor_type;
  uint8_t descriptor_subtype;
  uint8_t capabilities;
  uint8_t data_interface;
};

// The CDC ACM descriptor
struct [[gnu::packed]] CdcAbstractControlModelFunctionDescriptor_t
{
  uint8_t function_length;
  uint8_t descriptor_type;
  uint8_t descriptor_subtype;
  uint8_t capabilities;
};

// The CDC union descriptor
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
  ConfigurationDescriptor_t config;
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

const UsbDeviceDescription_t cdc_device_descriptor = {
  .length             = sizeof(device_descriptor),
  .descriptor_type    = 0x01,
  .bcd_usb            = 0x0110,
  .device_class       = usb_::type::kCdc,
  .device_subclass    = 0,
  .device_protocol    = 0,
  .max_packet_size    = 64,
  .id_vendor          = 0x1337,
  .id_product         = 0xB055,
  .bcdDevice          = 0x0100,
  .manufacturer       = 1,
  .product            = 2,
  .serial_number      = 3,
  .num_configurations = 1,
};

constexpr uint8_t kCsInterface = 0x24;
constexpr uint8_t kCsEndpoint  = 0x25;

// clang-format off
VirtualComPortDescriptor_t virtual_serial_port_descriptor = {
  .config =
  {
    .length                     = sizeof(ConfigurationDescriptor_t),
    .type                       = 0x02,
    .total_length               = sizeof(virtual_serial_port_descriptor),
    .interface_count            = 2,
    .configuration_value        = 1,
    .configuration_string_index = 0,
    .attributes                 = 0, // 0b1000'0000,
    .max_power                  = 250,
  },
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
      .bitmap = {
        .logical_address = 1,
        .reserved        = 0,
        .direction       = usb_::endpoint::Direction::kIn,
      }
    },
    .attributes      = {
      .bitmap = {
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
      .bitmap = {
        .logical_address = 2,
        .reserved        = 0,
        .direction       = usb_::endpoint::Direction::kIn,
      }
    },
    .attributes      = {
      .bitmap = {
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
      .bitmap = {
        .logical_address = 2,
        .reserved        = 0,
        .direction       = usb_::endpoint::Direction::kOut,
      }
    },
    .attributes      = {
      .bitmap = {
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

struct [[gnu::packed]] SerialComPortDescriptor_t
{
  ConfigurationDescriptor_t config;
  InterfaceDescriptor_t interface;
  EndpointDescriptor_t transmit;
  EndpointDescriptor_t receive;
};

// clang-format off
SerialComPortDescriptor_t serial_port_descriptor = {
  .config =
  {
    .length                     = sizeof(ConfigurationDescriptor_t),
    .type                       = 0x02,
    .total_length               = sizeof(serial_port_descriptor),
    .interface_count            = 1,
    .configuration_value        = 1,
    .configuration_string_index = 0,
    .attributes                 = 0, // 0b1000'0000,
    .max_power                  = 250,
  },
  .interface =
  {
    .length              = sizeof(InterfaceDescriptor_t),
    .type                = 0x4,
    .interface_number    = 0,
    .alternative_setting = 0,
    .endpoint_count      = 2,
    .interface_class     = 0, // usb_::type::kCdcData,
    .interface_subclass  = 0,
    .interface_protocol  = 0,
    .string_index        = 0,
  },
  .transmit =
  {
    .length          = sizeof(EndpointDescriptor_t),
    .type            = 0x5,
    .endpoint        = {
      .bitmap = {
        .logical_address = 2,
        .reserved        = 0,
        .direction       = usb_::endpoint::Direction::kIn,
      }
    },
    .attributes      = {
      .bitmap = {
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
      .bitmap = {
        .logical_address = 2,
        .reserved        = 0,
        .direction       = usb_::endpoint::Direction::kOut,
      }
    },
    .attributes      = {
      .bitmap = {
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

const char * string_descriptor[] = {
  "San Jose State University CmpE Department", "SJTwo Development Board",
  "0.0.1"
};

void ClearEndpointInterrupt(uint32_t endpoint)
{
  if (usb->EpIntSt & (1 << endpoint))
  {
    usb->EpIntClr = 1 << endpoint;
    while (!bit::Read(usb->DevIntSt, CDFULL))
    {
      continue;
    }
  }
}

void SerialInterfaceEngineWrite(uint8_t command, uint8_t data = 0,
                                bool data_present = false)
{
  usb->DevIntClr = (1 << CCEMPTY);
  usb->CmdCode   = (command << 16) | (0x05 << 8);
  while (!bit::Read(usb->DevIntSt, CCEMPTY))
  {
    continue;
  }
  usb->DevIntClr = (1 << CCEMPTY);
  if (data_present)
  {
    usb->CmdCode = (data << 16) | (0x01 << 8);
    while (!bit::Read(usb->DevIntSt, CCEMPTY))
    {
      continue;
    }
    usb->DevIntClr = (1 << CCEMPTY);
  }
}

uint16_t SerialInterfaceEngineRead(uint8_t command, bool two_bytes = false)
{
  uint16_t result = 0;
  usb->DevIntClr  = (1 << CCEMPTY) | (1 << CDFULL);
  usb->CmdCode    = (command << 16) | (0x05 << 8);
  while (!bit::Read(usb->DevIntSt, CCEMPTY))
  {
    continue;
  }
  usb->DevIntClr = (1 << CCEMPTY);
  usb->CmdCode   = (command << 16) | (0x02 << 8);
  while (!bit::Read(usb->DevIntSt, CDFULL))
  {
    continue;
  }
  usb->DevIntClr = (1 << CDFULL);

  result = static_cast<uint16_t>(usb->CmdData & 0xFF);

  if (two_bytes)
  {
    usb->CmdCode = (command << 16) | (0x02 << 8);
    while (!bit::Read(usb->DevIntSt, CDFULL))
    {
      continue;
    }
    usb->DevIntClr = (1 << CDFULL);

    result = static_cast<uint16_t>(result | ((usb->CmdCode & 0xFF) << 8));
  }
  return result;
}

union UsbControl {
  struct
  {
    uint32_t read_enable : 1;
    uint32_t write_enable : 1;
    uint32_t logical_endpoint : 4;
  } bits;
  uint32_t data;
};

void EndpointSend(uint32_t endpoint, const void * data_ptr, size_t length)
{
  const uint32_t * data = reinterpret_cast<const uint32_t *>(data_ptr);
  uint8_t physical_endpoint = static_cast<uint8_t>((endpoint * 2) + 1);
  if (!(usb->ReEp & (1 << physical_endpoint)))
  {
    LOG_WARNING("TX) Endpoint %" PRIu32 " PHY: %" PRIu8 " not realized",
                endpoint, physical_endpoint);
    return;
  }
  UsbControl enable_endpoint_write = { .bits = { .read_enable  = 0,
                                                 .write_enable = 1,
                                                 .logical_endpoint =
                                                     endpoint & 0xF } };
  uint16_t status = SerialInterfaceEngineRead(physical_endpoint);
  if (status & (1 << 3))
  {
    return;
  }
  usb->Ctrl         = enable_endpoint_write.data;
  usb->TxPLen       = length;
  size_t new_length = 0;
  for (size_t i = 0; new_length < length; i++)
  {
    usb->TxData = data[i];
    new_length += 4;
  }
  SerialInterfaceEngineWrite(kValidateBuffer);
  usb->Ctrl = 0;

  Wait(2000, [physical_endpoint]() -> bool {
    return bit::Read(usb->EpIntSt, physical_endpoint);
  });

  ClearEndpointInterrupt(physical_endpoint);
  LOG_INFO("TX_PACKET with Validate!");
}

void EndpointSend(uint32_t endpoint, std::initializer_list<uint8_t> list)
{
  EndpointSend(endpoint, reinterpret_cast<const void *>(list.begin()),
               list.size());
}

void SendZeroLengthPacket()
{
  EndpointSend(0, nullptr, 0);
}

void EndpointSendLarge(uint32_t endpoint, const uint32_t * data, size_t length)
{
  bool send_zero_length_packet = ((length % 64) == 0);
  int32_t total_length         = length;

  for (size_t i = 0; total_length > 0; i++)
  {
    size_t loop_transmit_length = 64;
    if (total_length < 64)
    {
      loop_transmit_length = total_length;
    }
    EndpointSend(endpoint, &data[(64 / 4) * i], loop_transmit_length);
    total_length -= loop_transmit_length;
  }
  if (send_zero_length_packet)
  {
    SendZeroLengthPacket();
  }
}

union RecievePacketLength {
  struct
  {
    uint32_t packet_length : 10;
    uint32_t is_data_valid : 1;
    uint32_t packet_read : 1;
  } bits;
  uint32_t data;
};

size_t EndpointReceive(uint32_t endpoint, uint32_t * buffer)
{
  uint8_t physical_endpoint = static_cast<uint8_t>(endpoint * 2);
  if (!(usb->ReEp & (1 << physical_endpoint)))
  {
    LOG_WARNING("RX) Endpoint %" PRIu32 " PHY: %" PRIu8 " not realized",
                endpoint, physical_endpoint);
    return 0;
  }
  LOG_DEBUG("RX EP%lu", endpoint);
  UsbControl enable_endpoint_read = { .bits = { .read_enable  = 1,
                                                .write_enable = 0,
                                                .logical_endpoint =
                                                    endpoint & 0xF } };

  usb->Ctrl         = enable_endpoint_read.data;
  size_t RxPLen     = usb->RxPLen;
  bool packet_ready = (RxPLen & (1 << 11));
  if (!packet_ready)
  {
    return 0;
  }
  size_t bytes       = (RxPLen & 0b1'1111'1111);
  size_t bytes_ceil  = (bytes > 64) ? 64 : bytes;
  size_t buffer_size = bytes_ceil;

  for (size_t i = 0; bytes != 0; i++)
  {
    buffer[i] = usb->RxData;
    if (bytes == 3)
    {
      buffer[i] &= 0xFFFFFF;
    }
    else if (bytes == 2)
    {
      buffer[i] &= 0xFFFF;
    }
    else if (bytes == 1)
    {
      buffer[i] &= 0xFF;
    }
    bytes -= (bytes >= 4) ? 4 : bytes % 4;
  }
  usb->Ctrl = 0;
  ClearEndpointInterrupt(endpoint);

  [[maybe_unused]] uint16_t endpoint_status =
      SerialInterfaceEngineRead(physical_endpoint);
  SerialInterfaceEngineRead(kClearBuffer);
  return buffer_size;
}

using EndpointProcedure = void (*)(const uint32_t * buffer, size_t length);

EndpointProcedure endpoint_isrs[32] = { nullptr };

void RealizeEndpoint(uint8_t endpoint, usb_::endpoint::Direction direction,
                     size_t size, EndpointProcedure isr = nullptr)
{
  uint32_t physical_endpoint = (endpoint * 2) + direction;

  usb->DevIntClr |= 1 << kEndpointRealized;
  usb->ReEp |= 1 << physical_endpoint;
  usb->EpInd    = physical_endpoint;
  usb->MaxPSize = size;

  while (!bit::Read(usb->DevIntSt, kEndpointRealized))
  {
    continue;
  }

  usb->EpIntEn |= 1 << physical_endpoint;
  usb->DevIntClr |= 1 << kEndpointRealized;
  uint8_t set_endpoint_mode_command =
      static_cast<uint8_t>(0x40 + physical_endpoint);
  SerialInterfaceEngineWrite(set_endpoint_mode_command, 0, true);
  SerialInterfaceEngineWrite(kClearBuffer);
  LOG_DEBUG("Set control endpoint %u to size %zu!", physical_endpoint, size);

  endpoint_isrs[physical_endpoint] = isr;
}
struct [[gnu::packed]] SetupPacket_t
{
  uint8_t request_type;
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

uint16_t configuration_number = 0;

struct [[gnu::packed]] StringLanguages_t
{
  static constexpr uint16_t kEnglishUS = 0x0409;
  uint8_t length                       = sizeof(StringLanguages_t);
  uint8_t type                         = 0x03;
  uint16_t language                    = kEnglishUS;
};

void ReadUsbBuffer(const uint32_t * buffer, size_t length)
{
  printf("%.*s", length, reinterpret_cast<const char *>(buffer));
  EndpointSend(serial_port_descriptor.transmit.endpoint.bitmap.logical_address,
               buffer, length);
}

struct [[gnu::packed]] AcmNotification
{
  uint8_t type;
  uint8_t notification_code;
  uint16_t value;
  uint16_t index;
  uint16_t length;
};

void ControlEndpointHandler(const uint32_t * buffer)
{
  constexpr uint8_t kGetRequest       = 0x80;
  constexpr uint8_t kSetRequest       = 0x00;
  constexpr uint8_t kSetAddress       = 0x05;
  constexpr uint8_t kGetDescriptor    = 0x06;
  constexpr uint8_t kGetConfiguration = 0x08;
  constexpr uint8_t kSetConfiguration = 0x09;

  constexpr uint8_t kSetFeature                       = 0x02;
  constexpr uint8_t kSetComFeature                    = 0x21;
  constexpr uint8_t kGetLineCoding                    = 0x21;
  constexpr uint8_t kSetLineCoding                    = 0x20;
  constexpr uint8_t kUsbCdcRequestSetControlLineState = 0x22;

  const SetupPacket_t * setup_packet =
      reinterpret_cast<const SetupPacket_t *>(buffer);

  constexpr uint8_t kCdcNotifySerialState = 0x20;
  AcmNotification notification            = {
    .type              = 0xA1,
    .notification_code = kCdcNotifySerialState,
    .value             = 0,
    .index             = 0,
    .length            = 2,
  };

  if (setup_packet->request_type == kGetRequest)
  {
    // debug::Hexdump(buffer, 8);
    switch (setup_packet->request)
    {
      case kGetDescriptor:
      {
        if (setup_packet->value.descriptor.type == 1)
        {
          size_t min_length =
              std::min({ static_cast<size_t>(setup_packet->length),
                         sizeof(cdc_device_descriptor) });
          EndpointSend(
              0, reinterpret_cast<const uint32_t *>(&cdc_device_descriptor),
              min_length);
        }
        else if (setup_packet->value.descriptor.type == 6)
        {
          // Stall on qualifier request
          SerialInterfaceEngineWrite(0x40, 1, true);
        }
        else if (setup_packet->value.descriptor.type == 2)
        {
          size_t min_length =
              std::min({ static_cast<size_t>(setup_packet->length),
                         sizeof(virtual_serial_port_descriptor) });
          EndpointSendLarge(0,
                            reinterpret_cast<const uint32_t *>(
                                &virtual_serial_port_descriptor),
                            min_length);
        }
        else if (setup_packet->value.descriptor.type == 3)
        {
          if (setup_packet->value.descriptor.index == 0)
          {
            StringLanguages_t languages;
            EndpointSend(0, reinterpret_cast<const uint32_t *>(&languages),
                         sizeof(languages));
          }
          else
          {
            StringDescriptor_t descriptor(
                string_descriptor[setup_packet->value.descriptor.index - 1]);
            EndpointSendLarge(
                0, reinterpret_cast<const uint32_t *>(descriptor.buffer),
                descriptor.size);
          }
        }
        else
        {
          SerialInterfaceEngineWrite(0x40, 1, true);
        }
        break;
      }
      case kGetConfiguration:
      {
        const uint32_t kConfigurationNumber = setup_packet->value.raw;
        EndpointSend(0, &kConfigurationNumber, 1);
        break;
      }
      default:
        printf(SJ2_HI_RED "Unhandled GET USB request %u" SJ2_COLOR_RESET,
               setup_packet->request);
        SerialInterfaceEngineWrite(0x40, 1, true);
        break;
    }
  }
  else if (setup_packet->request_type == kSetRequest)
  {
    switch (setup_packet->request)
    {
      case kSetAddress:
      {
        uint8_t address =
            static_cast<uint8_t>(bit::Set(setup_packet->value.raw, 7));
        LOG_DEBUG("New Address = 0x%02X :: %u", setup_packet->value.raw,
                  setup_packet->value.raw);
        SerialInterfaceEngineWrite(kSetAddressDevice, address, true);
        SendZeroLengthPacket();

        break;
      }
      case kSetConfiguration:
      {
        configuration_number = setup_packet->value.raw;
        RealizeEndpoint(1, usb_::endpoint::Direction::kIn, 64);
        RealizeEndpoint(2, usb_::endpoint::Direction::kOut, 64, ReadUsbBuffer);
        RealizeEndpoint(2, usb_::endpoint::Direction::kIn, 64);
        SerialInterfaceEngineWrite(kConfigureDeviceDevice, 0b0000'0001, true);
        SendZeroLengthPacket();
        break;
      }
      default:
        SerialInterfaceEngineWrite(0x40, 1, true);
        break;
    }
  }
  else if (setup_packet->request_type == kSetComFeature)
  {
    SendZeroLengthPacket();
    if (setup_packet->request == 0x22)
    {
      uint8_t value = static_cast<uint8_t>(setup_packet->value.raw);
      printf("DTR = %d | RTS = %d\n", value & 0b1, value >> 1);
    }
    else if (setup_packet->request == kGetLineCoding)
    {
      EndpointSend(1, reinterpret_cast<const uint32_t *>(&notification),
                   sizeof(notification));
    }
    else if (setup_packet->request == kSetLineCoding)
    {
      SendZeroLengthPacket();
    }
    else if (setup_packet->request_type == kUsbCdcRequestSetControlLineState)
    {
      SendZeroLengthPacket();
    }
    else
    {
      printf("R:0x%02X V:0x%02X\n", setup_packet->request,
             setup_packet->value.raw);
    }
  }
  else if (setup_packet->request_type == kSetFeature)
  {
    SendZeroLengthPacket();
  }
  else
  {
    LOG_ERROR("Unhandled USB request type 0x%02X :: 0x%02X",
              setup_packet->request_type, setup_packet->request);
    SerialInterfaceEngineWrite(0x40, 1, true);
  }
  LOG_DEBUG("~Handler");
}

void vSendByte([[maybe_unused]] void * pointer)
{
  const uint8_t kPrompt[] = "\nType whatever you like> ";
  while (true)
  {
    _SJ2_PRINT_VARIABLE(usb->ReEp, "0x%08lX");
    _SJ2_PRINT_VARIABLE(usb->EpIntSt, "0x%08lX");
    for (int i = 0; i < 5; i++)
    {
      vTaskDelay(1000);
      putchar('.');
    }
    putchar('S');
    putchar('\n');
    EndpointSend(
        serial_port_descriptor.transmit.endpoint.bitmap.logical_address,
        kPrompt, sizeof(kPrompt));
  }
}

int main(void)
{
  LOG_INFO("Usb Raw Test Starting...");
  Lpc40xxSystemController sys_ctrl;

  sys_ctrl.SetAlternatePll(Lpc40xxSystemController::PllInput::kF12MHz, 48);
  sys_ctrl.SelectUsbClockSource(
      Lpc40xxSystemController::UsbSource::kAlternatePllClock);
  sys_ctrl.SelectUsbClockDivider(
      Lpc40xxSystemController::UsbDivider::kDivideBy1);
  sys_ctrl.PowerUpPeripheral(Lpc40xxSystemController::Peripherals::kUsb);

  LPC_SC->CLKOUTCFG = bit::Insert(LPC_SC->CLKOUTCFG, 0x3, 0, 4);
  LPC_SC->CLKOUTCFG = bit::Insert(LPC_SC->CLKOUTCFG, 0xF, 4, 4);
  LPC_SC->CLKOUTCFG = bit::Set(LPC_SC->CLKOUTCFG, 8);
  Pin clock_output(1, 25);
  clock_output.SetPinFunction(0b101);

  LOG_INFO("Usb Powered up!");
  usb->USBClkCtrl = bit::Set(usb->USBClkCtrl, DEV_CLK_EN);
  LOG_INFO("Usb Clock Enable bits set!");
  while (!bit::Read(usb->USBClkSt, DEV_CLK_EN))
  {
    continue;
  }
  LOG_INFO("Device Clock Enabled!");
  usb->USBClkCtrl = bit::Set(usb->USBClkCtrl, AHB_CLK_EN);
  while (!bit::Read(usb->USBClkSt, AHB_CLK_EN))
  {
    continue;
  }
  LOG_INFO("AHB Clock Enabled!");

  // usb->USBClkCtrl = bit::Set(usb->USBClkCtrl, PORTSEL_CLK_EN);
  // // // TODO: Find the darn PORTSEL register in the LPC40xx.h file. Where is
  // it!
  // *(reinterpret_cast<uint32_t *>(0x2008'C110)) = 0;
  // usb->USBClkCtrl = bit::Clear(usb->USBClkCtrl, PORTSEL_CLK_EN);
  LOG_INFO("Port Select Clock Enabled!");

  Pin usb_dplus(0, 29);
  Pin usb_dminus(0, 30);
  Pin usb_vbus(1, 30);
  usb_dplus.SetPinFunction(0b001);
  usb_dminus.SetPinFunction(0b001);
  usb_vbus.SetMode(Pin::Mode::kInactive);
  usb_vbus.SetPinFunction(0b010);

  LOG_INFO("Pin functions set for USB!");

  RealizeEndpoint(0, usb_::endpoint::Direction::kIn, 64);
  RealizeEndpoint(0, usb_::endpoint::Direction::kOut, 64);

  LOG_INFO("Checking EP Interrupts 0x%08lX!", usb->EpIntSt);
  for (size_t i = 0; i < 32; i++)
  {
    if (usb->EpIntSt & 1 << i)
    {
      LOG_INFO("\tClear Interrupt on EP %zu!", i);
      usb->EpIntClr = 1 << i;
      while (!bit::Read(usb->DevIntSt, CDFULL))
      {
        Delay(500);
        LOG_INFO("EpIntSt = %lu", usb->EpIntSt);
        continue;
      }
    }
  }

  for (size_t i = 0; i < 32; i++)
  {
    if (usb->DevIntSt & 1 << i)
    {
      LOG_INFO("\tClear Dev Interrupt on EP %zu!", i);
      usb->DevIntClr = 1 << i;
      LOG_INFO("\tDevice Interrupt Status %zu Cleared!", i);
    }
  }
  usb->EpIntPri = 0;
  // TODO: Ignoring SET MODE instruction for now as the defaults should be fine
  usb->DevIntEn = bit::Set(usb->DevIntEn, EP_SLOWEN);
  usb->DevIntEn = bit::Set(usb->DevIntEn, DEV_STATEN);
  usb->DevIntEn = bit::Set(usb->DevIntEn, 9);
  LOG_INFO("Enabled Interrupts and Priority");

  RegisterIsr(USB_IRQn, []() {
    bool all_good_here = true;

    uint32_t buffer[64 / 4] = { 0 };
    if (usb->EpIntSt)
    {
      if ((usb->EpIntSt & 1) && all_good_here)
      {
        EndpointReceive(0, buffer);
        ControlEndpointHandler(buffer);
        ClearEndpointInterrupt(0);
      }

      for (size_t i = 2; i < 32; i++)
      {
        if (usb->EpIntSt & (1 << i))
        {
          printf(SJ2_HI_BACKGROUND_GREEN "\n1 << %d\n" SJ2_COLOR_RESET, i);
          if (endpoint_isrs[i] != nullptr)
          {
            memset(buffer, 0, sizeof(buffer));
            size_t length = EndpointReceive(
                static_cast<uint16_t>(floorf(static_cast<float>(i) / 2)),
                buffer);
            endpoint_isrs[i](buffer, length);
          }
          ClearEndpointInterrupt(i);
        }
      }
    }

    for (size_t i = 0; i < 10; i++)
    {
      if (usb->DevIntSt & 1 << i)
      {
        switch (i)
        {
          case 3:
          {
            [[maybe_unused]] uint16_t status =
                SerialInterfaceEngineRead(kGetDeviceStatusDevice);
            LOG_DEBUG("Device Change Status: 0x%02X", status);
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
            all_good_here = false;
            break;
          }
          default:
            LOG_DEBUG("Device Interrupt Status %zu Cleared!", i);
        }
        usb->DevIntClr = 1 << i;
      }
    }

    LOG_DEBUG("USB ISR Done!\n\n");
  });
  LOG_INFO("ISR Registered!");

  SerialInterfaceEngineWrite(kSetAddressDevice, 0b1000'0000, true);
  SerialInterfaceEngineWrite(kSetDeviceStatus, 0b0000'0001, true);
  SerialInterfaceEngineWrite(kSetModeDevice, 0b0000'0001, true);
  SerialInterfaceEngineWrite(kConfigureDeviceDevice, 0b0000'0001, true);
  LOG_WARNING("Clk Status = 0x%02X",
              SerialInterfaceEngineRead(kReadTestRegisterDevice, true));
  LOG_WARNING("Dev Status = 0x%02X",
              SerialInterfaceEngineRead(kGetDeviceStatusDevice));
  LOG_WARNING("Err Status = 0x%02X",
              SerialInterfaceEngineRead(kGetErrorCodeDevice));

  LOG_CRITICAL("Halting all Actions...");
  xTaskCreate(vSendByte, "USB Send", 2048, nullptr, rtos::kHigh, nullptr);
  vTaskStartScheduler();
  return 0;
}
