#pragma once

#include <array>
#include <cstdint>

#include "config.hpp"

class Esp8266WifiInterface
{
 public:
  std::array<char, config::kEsp8266BufferSize> default_buffer;
  enum class ConnectionType : uint8_t
  {
    kTcp = 0,
    kUdp = 1
  };
  virtual bool InitializeClientMode()                      = 0;
  virtual bool TestModule()                                = 0;
  virtual void ResetModule()                               = 0;
  virtual void DeepSleep(uint32_t time)                    = 0;
  virtual void GetModuleVersion()                          = 0;
  virtual void GetNetworkConnection()                      = 0;
  virtual bool ConnectToAccessPoint(const char ssid[],
                                    const char password[]) = 0;
  virtual void DisconnectFromAccessPoint()                 = 0;
  virtual bool IsConnectedToAccessPoint()                  = 0;
  virtual bool ConnectToHostname(
      const char hostname[], const char port[],
      ConnectionType type = ConnectionType::kTcp)                      = 0;
  virtual void DisconnectFromHostname()                                = 0;
  virtual bool IsConnectedToHostname()                                 = 0;
  virtual bool SendGetRequest(const char url[], char buffer[],
                              std::size_t buffer_size)                 = 0;
  virtual bool SendPostRequest(const char url[], const char data[],
                               char buffer[], std::size_t buffer_size) = 0;
};
