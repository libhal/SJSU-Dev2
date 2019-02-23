#include <inttypes.h>
#include <cstdint>

#include <project_config.hpp>

#include "L0_LowLevel/LPC40xx.h"
#include "L0_LowLevel/system_controller.hpp"
#include "L0_LowLevel/interrupt.hpp"
#include "L1_Drivers/pin.hpp"

#include "utility/log.hpp"
#include "utility/bit.hpp"
#include "utility/time.hpp"

LPC_USB_TypeDef * usb = LPC_USB;

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

// This is the standard device descriptor
struct [[gnu::packed]] UsbDeviceDescription_t
{
  uint8_t length;
  uint8_t descriptor_type;
  uint16_t bcdUSB;
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
const uint8_t kUsbDeviceClassCommunications = 0x02;
const UsbDeviceDescription_t descriptor     = {
  .length             = sizeof(descriptor),
  .descriptor_type    = 0x01,
  .bcdUSB             = 0x0200,
  .device_class       = 0xFF,
  .device_subclass    = 0,
  .device_protocol    = 0,
  .max_packet_size    = 64,
  .id_vendor          = 0x5555,
  .id_product         = 0xA5A5,
  .bcdDevice          = 0x0010,
  .manufacturer       = 0,
  .product            = 0,
  .serial_number      = 0,
  .num_configurations = 0,
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

struct DeviceDescriptor_t
{
  UsbDeviceDescription_t device;
  UsbDeviceQualifier_t qualifier;
};

const UsbDeviceQualifier_t qualifier =
{
  .length = sizeof(qualifier),
  .descriptor_type = 0x06,
  .bcd_usb = 0x0200,
  .device_class = 0xFF,
  .device_subclass = 0,
  .device_protocol = 0,
  .max_packet_size = 64,
  .num_configurations = 0,
  .reserved = 0,
};

const DeviceDescriptor_t device_descriptor = {
  .device = descriptor,
  .qualifier = qualifier
};

void SerialInterfaceEngineWrite(uint8_t command, uint8_t data)
{
  usb->DevIntClr = (1 << CCEMPTY);
  usb->CmdCode   = (command << 16) | (0x05 << 8);
  while (!bit::Read(usb->DevIntSt, CCEMPTY))
  {
    continue;
  }
  usb->DevIntClr = (1 << CCEMPTY);
  usb->CmdCode   = (data << 16) | (0x01 << 8);
  while (!bit::Read(usb->DevIntSt, CCEMPTY))
  {
    continue;
  }
  usb->DevIntClr = (1 << CCEMPTY);
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

void EndpointSend(uint8_t endpoint, const uint32_t * data, size_t length)
{
  UsbControl enable_endpoint_write = { .bits = {
                                           .read_enable      = 0,
                                           .write_enable     = 1,
                                           .logical_endpoint = endpoint } };
  usb->Ctrl   = enable_endpoint_write.data;
  usb->TxPLen = length;
  for (size_t i = 0; i < length; i += 4)
  {
    usb->TxData = data[i];
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

void EndpointRecieve(uint32_t endpoint, uint32_t * buffer)
{
  UsbControl enable_endpoint_read  = { .bits = {
                                          .read_enable      = 1,
                                          .write_enable     = 0,
                                          .logical_endpoint = endpoint } };
  usb->Ctrl                        = enable_endpoint_read.data;
  RecievePacketLength recieve_info = { .data = usb->RxPLen };
  for (size_t i = 0; i < recieve_info.bits.packet_length / 4; i++)
  {
    buffer[i] = usb->RxData;
  }
}

void RealizeEndpoint(uint8_t endpoint, size_t size)
{
  usb->DevIntSt = bit::Clear(usb->DevIntSt, kEndpointRealized);
  usb->ReEp |= 1 << endpoint;
  usb->EpInd = endpoint;
  usb->EpIntEn |= 1 << endpoint;
  usb->MaxPSize = size;
  while (!bit::Read(usb->DevIntSt, kEndpointRealized))
  {
    continue;
  }
  usb->DevIntSt = bit::Clear(usb->DevIntSt, kEndpointRealized);
  LOG_INFO("Set control endpoint %u to size %zu!", endpoint, size);
}
void ControlEndpointHandler(uint32_t * buffer)
{
  constexpr uint8_t kGetRequest    = 0x80;
  constexpr uint8_t kGetDescriptor = 0x06;
  if (buffer[0] & 0xFF)
  {
    const uint8_t request = (buffer[0] >> 8) & 0xFF;
    switch (request)
    {
      case kGetDescriptor:
        EndpointSend(0, reinterpret_cast<const uint32_t *>(&device_descriptor),
                     sizeof(device_descriptor));
        SJ2_PRINT_VARIABLE(sizeof(device_descriptor), "%zu");
        break;
      default: LOG_ERROR("Unhandled USB request %u", request); break;
    }
  }
  else
  {
    LOG_ERROR("Unhandled USB request type %lu", buffer[0] & 0xFF);
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
  usb->DevIntEn = bit::Set(usb->DevIntEn, 6);
  usb->DevIntEn = bit::Set(usb->DevIntEn, 7);
  usb->DevIntEn = bit::Set(usb->DevIntEn, 9);
  LOG_INFO("Enabled Interrupts and Priority");

  RegisterIsr(USB_IRQn, []() {
    LOG_CRITICAL("USB IRQ OCCURED!");
    for (size_t i = 0; i < 32; i++)
    {
      if (usb->EpIntSt & (1 << i))
      {
        uint32_t buffer[64 / 4] = {0};
        SerialInterfaceEngineRead(kClearBuffer, false);
        if (i % 2 == 0)
        {
          EndpointRecieve(i, buffer);
          if (i == 0)
          {
            ControlEndpointHandler(buffer);
          }
        }
        usb->EpIntClr = 1 << i;
        while (!bit::Read(usb->DevIntSt, CDFULL))
        {
          continue;
        }
        debug::Hexdump(buffer, sizeof(buffer));
        LOG_INFO("Endpoint%zu Interrupt Occured!" , i);
      }
    }
    for (size_t i = 0; i < 10; i++)
    {
      if (usb->DevIntSt & 1 << i)
      {
        switch (i)
        {
          case 3:
            LOG_CRITICAL("Device Reset");
            RealizeEndpoint(0, 64);
            RealizeEndpoint(1, 64);
            LOG_CRITICAL(
                "Dev Status = 0x%02X",
                SerialInterfaceEngineRead(kGetDeviceStatusDevice, false));
            break;
          case 6: LOG_CRITICAL("Rx Packet"); break;
          case 7: LOG_CRITICAL("Tx Packet"); break;
          case 9:
            LOG_WARNING("Error: 0x%02X",
                        SerialInterfaceEngineRead(0xFB, false));
            LOG_WARNING("Error Code: 0x%02X",
                        SerialInterfaceEngineRead(0xFF, false));
            break;
          default: LOG_WARNING("Device Interrupt Status %zu Cleared!", i);
        }
        usb->DevIntClr = 1 << i;
      }
    }
    LOG_CRITICAL("USB ISR Done!\n\n");
  });
  LOG_INFO("ISR Registered!");

  SerialInterfaceEngineWrite(kSetAddressDevice, 0b1000'0000);
  SerialInterfaceEngineWrite(kSetDeviceStatusDevice, 0b0000'0001);
  SerialInterfaceEngineWrite(kSetModeDevice, 0b0000'0001);
  SerialInterfaceEngineWrite(kConfigureDeviceDevice, 0b0000'0001);
  LOG_WARNING("Clk Status = 0x%02X",
              SerialInterfaceEngineRead(kReadTestRegisterDevice, true));
  Delay(100);
  LOG_WARNING("Dev Status = 0x%02X",
              SerialInterfaceEngineRead(kGetDeviceStatusDevice, false));
  LOG_WARNING("Err Status = 0x%02X",
              SerialInterfaceEngineRead(kGetErrorCodeDevice, false));

  LOG_INFO("Halting all Actions...");
  Halt();
  return 0;
}
