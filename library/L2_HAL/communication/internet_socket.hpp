#pragma once

#include <array>
#include <cstddef>
#include <string_view>

#include "utility/status.hpp"
#include "utility/units.hpp"

namespace sjsu
{
/// Modeled after Berkley Sockets (POSIX sockets)
class InternetSocket
{
 public:
  enum class Protocol
  {
    kTCP,
    kUDP,
  };
  virtual Status Connect(Protocol protocol,
                         std::string_view address,
                         uint16_t port,
                         std::chrono::nanoseconds timeout) = 0;

  virtual Status Write(const void * data,
                       size_t size,
                       std::chrono::nanoseconds timeout)   = 0;
  virtual size_t Read(void * buffer,
                      size_t size,
                      std::chrono::nanoseconds timeout)    = 0;
  virtual Status Close()                                   = 0;
};

class WiFi
{
 public:
  struct NetworkConnection_t
  {
    Status status;
    std::array<uint8_t, 4> ip;
    std::array<uint8_t, 4> netmask;
    std::array<uint8_t, 4> gateway;
    std::array<uint8_t, 6> mac;
  };

  virtual Status Initialize()                                           = 0;
  virtual bool IsConnected()                                            = 0;
  virtual Status ConnectToAccessPoint(std::string_view ssid,
                                      std::string_view password,
                                      std::chrono::nanoseconds timeout) = 0;
  virtual Status DisconnectFromAccessPoint()                            = 0;
  virtual NetworkConnection_t GetNetworkConnectionInfo()                = 0;
  virtual InternetSocket & GetInternetSocket()                          = 0;
};

}  // namespace sjsu
