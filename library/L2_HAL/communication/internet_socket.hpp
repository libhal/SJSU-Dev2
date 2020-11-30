#pragma once

#include <array>
#include <cstddef>
#include <span>
#include <string_view>

#include "module.hpp"
#include "utility/error_handling.hpp"
#include "utility/units.hpp"

namespace sjsu
{
/// @defgroup communication Communication Interfaces

/// A network socket is an internal endpoint for sending or receiving data from
/// within computer network. It is an endpoint in networking software.
/// Modeled after Berkley Sockets (POSIX sockets)
/// @ingroup communication
class InternetSocket : public Module
{
 public:
  /// Which Internet Protocol to use for communicating with the remote host
  /// server.
  enum class Protocol
  {
    kTCP,
    kUDP,
  };

  /// Establishes a direct communication link to a specific remote host
  /// identified by its address, port, and the communication protocol.
  ///
  /// @param protocol - Using TCP or UDP
  /// @param address - URL of the device you want to connect to.
  /// @param port - The port you want to use to connect to the host.
  /// @param timeout - Amount of time before this function should gives up.
  /// @return true if the connection with the address was successful.
  virtual bool Connect(Protocol protocol,
                       std::string_view address,
                       uint16_t port,
                       std::chrono::nanoseconds timeout) = 0;

  /// Send data to the connected host. Must have used Connect() before using
  /// this.
  ///
  /// @param data - data to write to socket
  /// @param timeout - Amount of time before this function should gives up.
  virtual void Write(std::span<const uint8_t> data,
                     std::chrono::nanoseconds timeout) = 0;

  /// Read data received from the connected host. Must have used Connect()
  /// before using this.
  ///
  /// @param buffer - location to read information from socket
  /// @param timeout - Amount of time before this function should gives up.
  ///
  /// @return
  virtual size_t Read(std::span<uint8_t> buffer,
                      std::chrono::nanoseconds timeout) = 0;

  /// Closes the connection established by the Connect() method.
  virtual void Close() = 0;
};

/// An interface for devices that can communicate wirelessly via the Wifi
/// protocol. This interface is used for connecting a device to a Wifi hotspot
/// (client).
/// @ingroup communication
class WiFi : public Module
{
 public:
  /// Contains network connection information such as IP address, netmask,
  /// gateway, and mac address.
  struct NetworkConnection_t
  {
    /// IPv4 IP address
    std::array<uint8_t, 4> ip;
    /// IPv4 IP address netmask
    std::array<uint8_t, 4> netmask;
    /// IPv4 gateway IP address
    std::array<uint8_t, 4> gateway;
    /// Device mac address
    std::array<uint8_t, 6> mac;
  };

  /// The type of password security used for the access point.
  enum class AccessPointSecurity
  {
    kOpen       = 0,
    kWep        = 1,
    kWpaPsk     = 2,
    kWpa2Psk    = 3,
    kWpaWpa2Psk = 4,
  };

  /// The type of mode to put the device into.
  enum class Mode
  {
    kClient = 1,
    kAccessPoint,
    kBoth,
  };

  /// @return true - if this Wifi instance is connected to an access point
  /// @return false - it this Wifi instance is not connected to an access point
  virtual bool IsConnected() = 0;

  /// Requests to connects to an access point.
  ///
  /// @param ssid - SSID of the access point you would like to connect to.
  /// @param password - the password to the access point.
  /// @param timeout - Amount of time before this function should gives up.
  /// @return true if connection to access point was successful.
  virtual bool ConnectToAccessPoint(std::string_view ssid,
                                    std::string_view password,
                                    std::chrono::nanoseconds timeout) = 0;

  /// Disconnect from the access point.
  virtual void DisconnectFromAccessPoint() = 0;

  /// @return NetworkConnection_t - Get connection information such as IP
  ///         address.
  virtual NetworkConnection_t GetNetworkConnectionInfo() = 0;
};

}  // namespace sjsu
