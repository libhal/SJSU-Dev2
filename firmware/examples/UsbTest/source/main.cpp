#include <algorithm>
#include <cinttypes>
#include <cstdint>

#include <project_config.hpp>

#include "L0_LowLevel/LPC40xx.h"
#include "L0_LowLevel/system_controller.hpp"
#include "L0_LowLevel/interrupt.hpp"
#include "L1_Drivers/pin.hpp"
#include "L2_HAL/displays/led/onboard_led.hpp"

#include "utility/log.hpp"
#include "utility/bit.hpp"
#include "utility/time.hpp"

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

[[maybe_unused]] constexpr uint8_t kSetAddressDevice             = 0xD0;
[[maybe_unused]] constexpr uint8_t kConfigureDeviceDevice        = 0xD8;
[[maybe_unused]] constexpr uint8_t kSetModeDevice                = 0xF3;
[[maybe_unused]] constexpr uint8_t kReadCurrentFrameNumberDevice = 0xF5;
[[maybe_unused]] constexpr uint8_t kReadTestRegisterDevice       = 0xFD;
[[maybe_unused]] constexpr uint8_t kSetDeviceStatusDevice        = 0xFE;
[[maybe_unused]] constexpr uint8_t kGetDeviceStatusDevice        = 0xFE;
[[maybe_unused]] constexpr uint8_t kClearBuffer                  = 0xF2;
[[maybe_unused]] constexpr uint8_t kGetErrorCodeDevice           = 0xFF;
[[maybe_unused]] constexpr uint8_t kReadErrorStatusDevice        = 0xFB;
[[maybe_unused]] constexpr uint8_t kValidateBuffer               = 0xFA;

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

const uint8_t kUsbDeviceClassCommunications    = 0x02;
const UsbDeviceDescription_t device_descriptor = {
  .length             = sizeof(device_descriptor),
  .descriptor_type    = 0x01,
  .bcd_usb            = 0x0110,
  .device_class       = 0xFF,
  .device_subclass    = 0,
  .device_protocol    = 0,
  .max_packet_size    = 64,
  .id_vendor          = 0xA5A5,
  .id_product         = 0xA5A5,
  .bcdDevice          = 0x0010,
  .manufacturer       = 1,
  .product            = 2,
  .serial_number      = 3,
  .num_configurations = 1,
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
  uint8_t buffer[64];
  uint8_t size;
  StringDescriptor_t(const char * str)
  {
    buffer[1]                     = 0x03;
    uint8_t length                = 2;
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

InterfaceDescriptor_t interface_descriptor = {
  .length              = sizeof(interface_descriptor),
  .type                = 0x4,
  .interface_number    = 0,
  .alternative_setting = 0,
  .endpoint_count      = 1,
  .interface_class     = 0xFF,
  .interface_subclass  = 0,
  .interface_protocol  = 0,
  .string_index        = 0,
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
  uint8_t attributes;
  uint16_t max_packet_size;
  uint8_t interval;
};

EndpointDescriptor_t endpoint_descriptor = {
  .length          = sizeof(endpoint_descriptor),
  .type            = 0x5,
  .endpoint        = { .bitmap = { .logical_address = 2,
                            .reserved        = 0,
                            .direction       = 0 } },
  .attributes      = 0b10,
  .max_packet_size = 64,
  .interval        = 1,
};

struct [[gnu::packed]] LinkedConfig_t
{
  ConfigurationDescriptor_t config;
  InterfaceDescriptor_t interface;
  EndpointDescriptor_t endpoint;
};

LinkedConfig_t omni = {
  .config =
      {
          .length                     = sizeof(ConfigurationDescriptor_t),
          .type                       = 0x02,
          .total_length               = sizeof(omni),
          .interface_count            = 1,
          .configuration_value        = 1,
          .configuration_string_index = 0,
          .attributes                 = 0b1000'0000,
          .max_power                  = 250,
      },
  .interface = interface_descriptor,
  .endpoint  = endpoint_descriptor,
};

const char * string_descriptor[] = { "K.A.M.M.C.E. Research", "Kammce USB",
                                     "S.A.M.E.E.R. Clone #2" };

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

void SerialInterfaceEngineWrite(uint8_t command, uint8_t data,
                                bool data_present = true)
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

void EndpointSend(uint32_t endpoint, const uint32_t * data, size_t length)
{
  UsbControl enable_endpoint_write = { .bits = { .read_enable  = 0,
                                                 .write_enable = 1,
                                                 .logical_endpoint =
                                                     endpoint & 0xF } };
  uint8_t physical_endpoint        = static_cast<uint8_t>((endpoint * 2) + 1);
  uint16_t status = SerialInterfaceEngineRead(physical_endpoint);
  if (status & (1 << 3))
  {
    return;
  }
  usb->Ctrl      = enable_endpoint_write.data;
  usb->TxPLen    = length;
  int new_length = 0;
  for (size_t i = 0; new_length < length; i++)
  {
    usb->TxData = data[i];
    new_length += 4;
  }
  SerialInterfaceEngineWrite(kValidateBuffer, 0, false);
  usb->Ctrl = 0;
  ClearEndpointInterrupt(1);
  LOG_INFO("TX_PACKET with Validate!");
}

void EndpointSend(uint32_t endpoint, std::initializer_list<uint32_t> list)
{
  EndpointSend(endpoint, list.begin(), list.size());
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

void EndpointRecieve(uint32_t endpoint, uint32_t * buffer)
{
  LOG_DEBUG("RX EP%lu", endpoint);
  UsbControl enable_endpoint_read = { .bits = { .read_enable  = 1,
                                                .write_enable = 0,
                                                .logical_endpoint =
                                                    endpoint & 0xF } };

  usb->Ctrl      = enable_endpoint_read.data;
  uint32_t bytes = usb->RxPLen & ~((1 << 10) | (1 << 11));
  SJ2_PRINT_VARIABLE(bytes, "%ld");
  for (size_t i = 0; bytes != 0; i++)
  {
    buffer[i] = usb->RxData;
    bytes -= 4;
  }
  usb->Ctrl = 0;
  ClearEndpointInterrupt(endpoint);

  uint8_t physical_endpoint = static_cast<uint8_t>(endpoint * 2);
  [[maybe_unused]] uint16_t endpoint_status =
      SerialInterfaceEngineRead(physical_endpoint);
  SerialInterfaceEngineRead(kClearBuffer);
  LOG_DEBUG("~RX EP%lu", endpoint);
}

void RealizeEndpoint(uint8_t endpoint, size_t size)
{
  usb->DevIntClr |= 1 << kEndpointRealized;
  usb->ReEp |= 1 << endpoint;
  usb->EpInd    = endpoint;
  usb->MaxPSize = size;
  while (!bit::Read(usb->DevIntSt, kEndpointRealized))
  {
    continue;
  }
  usb->EpIntEn |= 1 << endpoint;
  usb->DevIntClr |= 1 << kEndpointRealized;
  uint8_t set_endpoint_mode_command = static_cast<uint8_t>(0x40 + endpoint);
  SerialInterfaceEngineWrite(set_endpoint_mode_command, 0);
  SerialInterfaceEngineWrite(set_endpoint_mode_command, 0);
  LOG_DEBUG("Set control endpoint %u to size %zu!", endpoint, size);
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

[[gnu::always_inline]] void SendZeroLengthPacket() {
  EndpointSend(0, nullptr, 0);
}

uint16_t configuration_number = 0;

struct [[gnu::packed]] StringLanguages_t
{
  static constexpr uint16_t kEnglishUS = 0x0409;
  uint8_t length = sizeof(StringLanguages_t);
  uint8_t type = 0x03;
  uint16_t language = kEnglishUS;
};

void ControlEndpointHandler(const uint32_t * buffer)
{
  constexpr uint8_t kGetRequest       = 0x80;
  constexpr uint8_t kSetRequest       = 0x00;
  constexpr uint8_t kSetAddress       = 0x05;
  constexpr uint8_t kGetDescriptor    = 0x06;
  constexpr uint8_t kGetConfiguration = 0x08;
  constexpr uint8_t kSetConfiguration = 0x09;

  const SetupPacket_t * setup_packet =
      reinterpret_cast<const SetupPacket_t *>(buffer);

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
                         sizeof(device_descriptor) });
          EndpointSend(0,
                       reinterpret_cast<const uint32_t *>(&device_descriptor),
                       min_length);
        }
        else if (setup_packet->value.descriptor.type == 2)
        {
          size_t min_length = std::min(
              { static_cast<size_t>(setup_packet->length), sizeof(omni) });
          EndpointSend(0, reinterpret_cast<const uint32_t *>(&omni),
                       min_length);
        }
        else if (setup_packet->value.descriptor.type == 3)
        {
          if (setup_packet->value.descriptor.index == 0)
          {
            StringLanguages_t languages;
            EndpointSend(0,
                         reinterpret_cast<const uint32_t *>(&languages),
                         sizeof(languages));
          }
          else
          {
            StringDescriptor_t descriptor(
                string_descriptor[setup_packet->value.descriptor.index - 1]);
            EndpointSend(0,
                         reinterpret_cast<const uint32_t *>(descriptor.buffer),
                         descriptor.size);
          }
        }
        else
        {
          SerialInterfaceEngineWrite(0x40, 1);
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
        LOG_ERROR("Unhandled GET USB request %u", setup_packet->request);
        SerialInterfaceEngineWrite(0x40, 1);
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
        SerialInterfaceEngineWrite(kSetAddressDevice, address);
        SendZeroLengthPacket();

        break;
      }
      case kSetConfiguration:
      {
        configuration_number = setup_packet->value.raw;
        SendZeroLengthPacket();
        break;
      }
      default:
        SerialInterfaceEngineWrite(0x40, 1);
        break;
    }
  }
  else
  {
    LOG_ERROR("Unhandled USB request type %lu", buffer[0] & 0xFF);
    SerialInterfaceEngineWrite(0x40, 1);
  }
  LOG_DEBUG("~Handler");
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

  RealizeEndpoint(0, 64);
  RealizeEndpoint(1, 64);

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
    for (size_t i = 0; i < 10; i++)
    {
      if (usb->DevIntSt & 1 << i)
      {
        usb->DevIntClr = 1 << i;
        switch (i)
        {
          case 3:
          {
            uint16_t status = SerialInterfaceEngineRead(kGetDeviceStatusDevice);
            if (status & (1 << 4))
            {
              RealizeEndpoint(0, 64);
              RealizeEndpoint(1, 64);
              // puts("R!");
              all_good_here = false;
            }
            LOG_DEBUG("Device Change Status: 0x%02X", status);
            break;
          }
          case 9:
            LOG_WARNING("Error: 0x%02X", SerialInterfaceEngineRead(0xFB));
            LOG_WARNING("Error Code: 0x%02X", SerialInterfaceEngineRead(0xFF));
            all_good_here = false;
            break;
          default:
            LOG_DEBUG("Device Interrupt Status %zu Cleared!", i);
        }
      }
    }
    if ((usb->EpIntSt & 1) && all_good_here)
    {
      uint32_t buffer[64 / 4] = { 0 };
      EndpointRecieve(0, buffer);
      ControlEndpointHandler(buffer);
      ClearEndpointInterrupt(0);
    }

    LOG_DEBUG("USB ISR Done!\n\n");
  });
  LOG_INFO("ISR Registered!");

  SerialInterfaceEngineWrite(kSetAddressDevice, 0b1000'0000);
  SerialInterfaceEngineWrite(kSetDeviceStatusDevice, 0b0000'0001);
  SerialInterfaceEngineWrite(kSetModeDevice, 0b0000'0001);
  SerialInterfaceEngineWrite(kConfigureDeviceDevice, 0b0000'0001);
  LOG_WARNING("Clk Status = 0x%02X",
              SerialInterfaceEngineRead(kReadTestRegisterDevice, true));
  LOG_WARNING("Dev Status = 0x%02X",
              SerialInterfaceEngineRead(kGetDeviceStatusDevice));
  LOG_WARNING("Err Status = 0x%02X",
              SerialInterfaceEngineRead(kGetErrorCodeDevice));

  LOG_CRITICAL("Halting all Actions...");
  Halt();
  return 0;
}
