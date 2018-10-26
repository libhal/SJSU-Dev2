#pragma once

#include <cstdint>

enum class Status : uint8_t
{
  kSuccess,
  kTimedOut,
  kBusError,
  kDeviceNotFound
};
