#pragma once

#include <cstdint>

class UartInterface
{
 public:
  virtual void SetBaudRate(uint32_t baud_rate)                 = 0;
  virtual bool Initialize(uint32_t baud_rate, uint32_t mode)   = 0;
  virtual void Send(char out, uint32_t time_limit)             = 0;
  virtual bool Receive(char * char_input, uint32_t time_limit) = 0;
};
