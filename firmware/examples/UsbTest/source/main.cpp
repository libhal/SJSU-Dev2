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

void SerialInterfaceEngineWrite(uint8_t command, uint8_t data)
{
  usb->DevIntClr = (1 << CCEMPTY);
  usb->CmdCode = (command << 16) | (0x05 << 8);
  while (!bit::Read(usb->DevIntSt, CCEMPTY))
  {
    continue;
  }
  LOG_INFO("COMMAND 0x%02X PART 1/2", command);
  usb->DevIntClr = (1 << CCEMPTY);
  usb->CmdCode   = (data << 16) | (0x01 << 8);
  while (!bit::Read(usb->DevIntSt, CCEMPTY))
  {
    continue;
  }
  LOG_INFO("COMMAND Data 0x%02X PART 1/2", data);
  usb->DevIntClr = (1 << CCEMPTY);
}


uint16_t SerialInterfaceEngineRead(uint8_t command, bool two_bytes = false)
{
  uint16_t result = 0;
  usb->DevIntClr = (1 << CCEMPTY) | (1 << CDFULL);
  usb->CmdCode = (command << 16) | (0x05 << 8);
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

  result = static_cast<uint16_t>(usb->CmdData);

  if (two_bytes)
  {
    usb->CmdCode   = (command << 16) | (0x02 << 8);
    while (!bit::Read(usb->DevIntSt, CDFULL))
    {
      continue;
    }
    usb->DevIntClr = (1 << CDFULL);
    result = static_cast<uint16_t>(result | usb->CmdCode << 8);
  }
  return result;
}

int main(void)
{
  LOG_INFO("Usb Raw Test Starting...");
  Delay(1000);
  Lpc40xxSystemController sys_ctrl;
  // TODO: Set Clock source to main external clock
  // TODO: Set USB clock to 48Mhz
  sys_ctrl.SetAlternatePll(Lpc40xxSystemController::PllInput::kF16MHz, 48);
  sys_ctrl.SelectUsbClockSource(Lpc40xxSystemController::UsbSource::kAlternatePllClock);
  sys_ctrl.SelectUsbClockDivider(Lpc40xxSystemController::UsbDivider::kDivideBy1);
  sys_ctrl.PowerUpPeripheral(Lpc40xxSystemController::Peripherals::kUsb);

  LOG_INFO("Usb Powered up!");
  usb->USBClkCtrl = bit::Set(usb->USBClkCtrl, DEV_CLK_EN);
  usb->USBClkCtrl = bit::Set(usb->USBClkCtrl, AHB_CLK_EN);
  LOG_INFO("Usb Clock Enable bits set!");
  while (!bit::Read(usb->USBClkSt, DEV_CLK_EN))
  {
    continue;
  }
  LOG_INFO("Device Clock Enabled!");
  while (!bit::Read(usb->USBClkSt, AHB_CLK_EN))
  {
    continue;
  }
  LOG_INFO("AHB Clock Enabled!");

  // usb->USBClkCtrl = bit::Set(usb->USBClkCtrl, PORTSEL_CLK_EN);
  // // TODO: Find the darn PORTSEL register in the LPC40xx.h file. Where is it!
  // *(reinterpret_cast<uint32_t *>(usb) + 0x110) = 0;
  // usb->USBClkCtrl = bit::Clear(usb->USBClkCtrl, PORTSEL_CLK_EN);
  // LOG_INFO("Port Select Clock Enabled!");

  Pin usb_dplus(0, 29);
  Pin usb_dminus(0, 30);
  Pin usb_vbus(1, 30);
  usb_dplus.SetPinFunction(0b001);
  usb_dminus.SetPinFunction(0b001);
  usb_vbus.SetMode(Pin::Mode::kInactive);
  usb_vbus.SetPinFunction(0b010);

  LOG_INFO("Pin functions set for USB!");

  usb->ReEp    |= 1 << 0;
  usb->EpInd    = 0;
  usb->MaxPSize = 64;
  while (!bit::Read(usb->DevIntSt, kEndpointRealized))
  {
    continue;
  }
  usb->DevIntSt = bit::Clear(usb->DevIntSt, kEndpointRealized);

  LOG_INFO("Set control endpoint 0 to size 64!");
  usb->ReEp    |= 1 << 1;
  usb->EpInd    = 1;
  usb->MaxPSize = 64;
  while (!bit::Read(usb->DevIntSt, kEndpointRealized))
  {
    continue;
  }
  usb->DevIntSt = bit::Clear(usb->DevIntSt, kEndpointRealized);
  LOG_INFO("Set control endpoint 1 to size 64!");

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

  usb->EpIntEn = 0xFF'FF'FF'FF;

  usb->EpIntPri = 0;
  LOG_INFO("Enabled Interrupts and Priority");

  // TODO: Ignoring SET MODE instruction for now as the defaults should be fine
  usb->DevIntEn = bit::Set(usb->DevIntEn, EP_SLOWEN);
  usb->DevIntEn = bit::Set(usb->DevIntEn, DEV_STATEN);
  LOG_INFO("Enabled Interrupts and Priority");

  RegisterIsr(USB_IRQn, []() {
    printf("USB IRQ OCCURED!\n");
    LOG_INFO("EpIntSt = 0x%08lX", usb->EpIntSt);
    for (size_t i = 0; i < 32; i++)
    {
      if (usb->EpIntSt & 1 << i)
      {
        LOG_INFO("Interrupt Ep%zu Occured!", i);
        usb->EpIntClr = 1 << i;
        while (!bit::Read(usb->DevIntSt, CDFULL))
        {
          continue;
        }
      }
    }
    for (size_t i = 0; i < 32; i++)
    {
      if (usb->DevIntSt & 1 << i)
      {
        switch(i)
        {
          case 6:
            LOG_WARNING("Rx Packet");
            break;
          case 7:
            LOG_WARNING("Tx Packet");
            break;
          case 8:
            LOG_WARNING("Endpoint Realized");
            break;
          case 9:
            LOG_WARNING("Error: 0x%02X", SerialInterfaceEngineRead(0xFB, false));
            break;
        }
        usb->DevIntClr = 1 << i;
        LOG_INFO("\tDevice Interrupt Status %zu Cleared!", i);
      }
    }
  });
  LOG_INFO("ISR Registered!");

  SerialInterfaceEngineWrite(0xD0, 0b1000'0000);
  SerialInterfaceEngineWrite(0xFE, 0b0000'0001);
  SerialInterfaceEngineWrite(0xF3, 0b0000'0001);
  SerialInterfaceEngineWrite(0xD8, 0b0000'0001);
  LOG_WARNING("Clk Status = 0x%02X", SerialInterfaceEngineRead(0xFD, true));
  Delay(100);
  LOG_WARNING("Dev Status = 0x%02X", SerialInterfaceEngineRead(0xFE, false));
  LOG_WARNING("Err Status = 0x%02X", SerialInterfaceEngineRead(0xFF, false));
  LOG_INFO("Halting all Actions...");
  Halt();
  return 0;
}
