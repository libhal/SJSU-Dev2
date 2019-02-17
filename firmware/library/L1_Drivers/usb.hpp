#pragma once

#include "L0_LowLevel/LPC40xx.h"
#include "L0_LowLevel/system_controller.hpp"

class UsbInterface
{
 public:
  // clang-format off
  enum class Endpoint
  {
    kRx0 = 0, kTx0,
    kRx1, kTx1,
    kRx2, kTx2,
    kRx3, kTx3,
    kRx4, kTx4,
    kRx5, kTx5,
    kRx6, kTx6,
    kRx7, kTx7,
    kRx8, kTx8,
    kRx9, kTx9,
    kRx10, kTx10,
    kRx11, kTx11,
    kRx12, kTx12,
    kRx13, kTx13,
    kRx14, kTx14,
    kRx15, kTx15,
  };
  // clang-format on
  virtual void Initialize()                                               = 0;
  virtual void RealizeEndpoint(Endpoint endpoint)                         = 0;
  virtual void SetEndpointSize(Endpoint endpoint, uint16_t endpoint_size) = 0;
  virtual size_t ReadEndpoint(Endpoint endpoint, uint8_t * buffer,
                              size_t length)                              = 0;
  virtual void WriteEndpoint(Endpoint endpoint, uint8_t * buffer,
                             size_t length)                               = 0;
};

class Usb final : public UsbInterface, protected Lpc40xxSystemController
{
 public:
  inline static LPC_USB_TypeDef * usb_register = LPC_USB;
  void Initialize() override
  {
    PowerUpPeripheral(Lpc40xxSystemController::Peripherals::kUsb);
    // usb_register->PORT_SELECT = 0;
    // usb_register->
  }
  void RealizeEndpoint(Endpoint endpoint) override {}
  void SetEndpointSize(Endpoint endpoint, uint16_t endpoint_size) override {}
  size_t ReadEndpoint(Endpoint endpoint, uint8_t * buffer,
                      size_t length) override
  {
  }
  void WriteEndpoint(Endpoint endpoint, uint8_t * buffer, size_t length) {}

 private:
  void SetEndpointPointer(Endpoint endpoint) {}
};
