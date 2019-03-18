#pragma once

enum class Status
{
  // Operation worked as expected.
  kSuccess,
  // Operation could not be completed in the given time.
  kTimedOut,
  // Operation could not be completed due to an error on the communication bus.
  kBusError,
  // Operation to find an external deviced returned.
  kDeviceNotFound,
  // Operations not handled by executing function or method.
  kNotHandled,
};

constexpr const char * Stringify(Status status)
{
  const char * result = "";
  switch (status)
  {
    case Status::kSuccess:
      result = "Success";
      break;
    case Status::kTimedOut:
      result = "Timed Out";
      break;
    case Status::kBusError:
      result = "Bus Error";
      break;
    case Status::kDeviceNotFound:
      result = "Device Not Found";
      break;
    case Status::kNotHandled:
      result = "Operation Not Handled";
      break;
  }
  return result;
}
