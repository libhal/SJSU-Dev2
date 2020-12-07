#pragma once

#include <algorithm>
#include <array>
#include <atomic>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <initializer_list>
#include <optional>
#include <span>
#include <unordered_map>
#include <utility>

#include "L1_Peripheral/inactive.hpp"
#include "module.hpp"
#include "utility/allocator.hpp"
#include "utility/log.hpp"

namespace sjsu
{
/// The common interface for the CANBUS peripherals.
/// @ingroup l1_peripheral
class Can : public Module
{
 public:
  // ===========================================================================
  // Interface Defintions
  // ===========================================================================

  /// This struct represents a transmit message based on the BOSCH CAN
  /// spec 2.0B.
  struct Message_t
  {
    /// The format of the can message
    enum class Format : uint8_t
    {
      /// Use 11-bit ID message
      kStandard = 0,
      /// Use 29-bit ID message
      kExtended = 1,
      kNumberOfFormats,
    };

    /// CAN message ID
    uint32_t id;

    /// Is this message a remote request message. If so the contents of payload
    /// are ignored. Length shall have the length of requested data to get back
    /// from the device responsible for message id.
    bool is_remote_request = false;

    /// Length of the payload
    uint8_t length = 0;

    /// ID format
    Format format = Format::kStandard;

    /// Set the time in which this message was received. This value is default
    /// constructed and should not be changed.
    std::chrono::nanoseconds uptime = Uptime();

    /// Container of the payload contents
    std::array<uint8_t, 8> payload;

    void SetPayload(std::span<const uint8_t> data) noexcept
    {
      length = static_cast<uint8_t>(std::min(payload.size(), data.size()));
      std::copy_n(data.begin(), length, payload.begin());
    }
  };

  using ReceiveHandler = std::function<void(sjsu::Can &)>;

  inline static ReceiveHandler kDisableHandler = nullptr;

  /// Standard baud rate for most CANBUS networks
  static constexpr units::frequency::hertz_t kStandardBaudRate = 100'000_Hz;

  // ===========================================================================
  // Interface Methods
  // ===========================================================================

  // ---------------------------------------------------------------------------
  // Configuration Methods
  // ---------------------------------------------------------------------------

  /// @param baud - the baud rate to set the CANBUS communication frequency to.
  virtual void ConfigureBaudRate(
      units::frequency::hertz_t baud = kStandardBaudRate) = 0;

  /// Filter out CANBUS messages based on their ID. Without such a filter, every
  /// single message on the CANBUS will be received, even those not intended or
  /// useful for this target device. Without a filter, software will need to
  /// perform the work to throw away CAN messages that are irrelevant. This
  /// method will utilize the CAN peripheral hardware to perform message
  /// filtering.
  ///
  /// NOTE: ConfigureAcceptanceFilter(true) must be called for the filter to be
  /// taken into effect.
  ///
  /// @param id - the ID to accept
  /// @return true - if the filter was able to be installed successfully
  /// @return false - if the filter was NOT able to be installed successfully.
  ///         This typically occurres when the number of hardware filters runs
  ///         out or if the CAN driver or peripheral does not support filtering.
  virtual bool ConfigureFilter(uint32_t id) = 0;

  /// Enables and disables the acceptance filter. When disabled all messages
  /// will be received by the CAN peripheral. When enabled, only the messages
  /// with the correct ID set by ConfigureFilter() will be allowed.
  ///
  /// @param enable - set to true to enable enable filter, false to disable.
  /// @return true - if the new state was accepted. False otherwise.
  virtual bool ConfigureAcceptanceFilter(bool enable) = 0;

  /// Enable interrupts for receiving CAN messages and set the handler to the
  /// one provided here.
  ///
  /// @param handler - Action to be taken when a new can message arrives. Set to
  /// nullptr in order to disable this interrupt.
  virtual void ConfigureReceiveHandler(ReceiveHandler handler) = 0;

  // ---------------------------------------------------------------------------
  // Usage Methods
  // ---------------------------------------------------------------------------

  /// Send a message via CANBUS to the designated device with the supplied ID
  ///
  /// @param message - Message containing the CANBUS contents.
  virtual void Send(const Message_t & message) = 0;

  /// Receive a CANBUS message from the queue.
  ///
  /// @return Message_t - messages
  virtual Message_t Receive() = 0;

  /// Checks if there is a message available for this channel.
  ///
  /// @returns true a message was received.
  virtual bool HasData() = 0;

  /// Determine if you can communicate over the bus.
  ///
  /// @param id - device you want to ping to determine if you can communicate on
  ///             the bus.
  /// @return true - on success
  /// @return false - on failure
  virtual bool SelfTest(uint32_t id) = 0;

  /// @return true - if the device is "bus Off"
  /// @return false - if the device is NOT "bus off"
  virtual bool IsBusOff() = 0;

  // ===========================================================================
  // Utility Methods
  // ===========================================================================

  /// Send a message via CANBUS to the designated device with the supplied ID
  ///
  /// @param id - ID to send the data to.
  /// @param payload - array literal payload to send to the device with ID
  /// @return true - on success
  /// @return false - on failure
  void Send(uint32_t id, std::initializer_list<uint8_t> payload)
  {
    Message_t message{};

    message.id = id;
    message.SetPayload(payload);

    return Send(message);
  }

  /// Send a message via CANBUS to the designated device with the supplied ID
  ///
  /// @param id - ID to send the data to.
  /// @param payload - uint8_t span payload to send to the device with ID
  /// @return true - on success
  /// @return false - on failure
  void Send(uint32_t id, std::span<const uint8_t> payload)
  {
    Message_t message{};

    message.id = id;
    message.SetPayload(payload);

    return Send(message);
  }
};

/// Template specialization that generates an inactive sjsu::Can.
template <>
inline sjsu::Can & GetInactive<sjsu::Can>()
{
  class InactiveCan : public sjsu::Can
  {
   public:
    void ModuleInitialize() override {}
    void ModuleEnable(bool = true) override {}

    void ConfigureBaudRate(
        units::frequency::hertz_t = kStandardBaudRate) override
    {
    }
    bool ConfigureFilter(uint32_t) override
    {
      return true;
    }
    bool ConfigureAcceptanceFilter(bool) override
    {
      return true;
    }
    void ConfigureReceiveHandler(ReceiveHandler) override {}
    void Send(const Message_t &) override {}
    Message_t Receive() override
    {
      return {};
    }
    bool HasData() override
    {
      return false;
    }
    bool SelfTest(uint32_t) override
    {
      return true;
    }
    bool IsBusOff() override
    {
      return false;
    }
  };

  static InactiveCan inactive_can;
  return inactive_can;
}

/// CanNetwork is a canbus message receiver handler and
class CanNetwork : public sjsu::Module
{
 public:
  /// The node stored in the CanNetwork map. Holds the latest CAN message and
  /// contains methods for updating and retreiving can messages in a thread-safe
  /// manner that does not invoke OS locks.
  ///
  /// Updating the CAN message data is completely lock free. Retrieving
  /// data is NOT lock free, but instead uses atomics to poll for when the
  /// Update() function has completed in some other thread. This asymmetry in
  /// locking is to reduce latency for write case rather than than read case.
  /// Storing a CAN message is typically done via an interrupt service routine
  /// or a thread that MUST NOT block in anyway or the system can lock up. Where
  /// as reading data typically is done by a userspace thread which can
  /// typically wait a few cycles to get its data.
  class Node_t
  {
   public:
    Node_t() noexcept {}

    Node_t & operator=(const Node_t & node) noexcept
    {
      data = node.data;
      access_counter.store(access_counter.load());
      return *this;
    }

    Node_t(const Node_t & node) noexcept
    {
      *this = node;
    }

    /// Return a CAN message, but only do so if the CAN message of this node is
    /// not currently be modified by another thread that is using the Update()
    /// method.
    Can::Message_t SecureGet()
    {
      // Continuously check if the received CAN message is valid. NOTE: that, in
      // general, the looping logic for this function almost never occurs as
      // copies from RAM/cache are fast between ReceiveHandler and GetMessage()
      while (true)
      {
        // First, atomically copy the access_counter to a variable.
        const auto kReadMessageStart = access_counter.load();

        // Copy the contents of the mesage into the kCanMessage variable.
        const Can::Message_t kCanMessage = data;

        // Finally, atomically copy the access_counter again such that the value
        // between the start and finish can be compared.
        const auto kReadMessageFinish = access_counter.load();

        // If and only if these two values are the same, meaning that the
        // ReceiveHandler isn't currently modifying the Node_t's data,
        // can the message be considered valid.
        if (kReadMessageStart == kReadMessageFinish)
        {
          return kCanMessage;
        }
      }

      return {};
    };

   private:
    friend CanNetwork;

    /// Updates the can message in a lock-free way. Can only be accessed by the
    /// CanNetwork class.
    ///
    /// @param new_data - New CAN message to store
    void Update(const Can::Message_t & new_data)
    {
      // Atomic increment of the access counter to notify any threads that are
      // using GetMesage() that the value of this node is changing.
      access_counter++;

      // Copy the contents of the new message into the map.
      data = new_data;

      // Another atomic increment of the access counter to notify any thread
      // using GetMessage() that the value of this node has finished changing.
      // For GetMessage to be valid, the function must save the access_counter,
      // copy the message data, then check the access_counter again to make sure
      // it has not changed. If it has changed, then GetMessage() should
      // continue polling the map node until access_counter is the same between
      // copies.
      access_counter++;
    }

    /// Holds the latest received can message;
    Can::Message_t data = {};

    /// Used to indicate when the data field is being accessed
    std::atomic<int> access_counter = 0;
  };

  /// @param can - CAN peripheral to manage the network of.
  /// @param memory_resource - pointer to a memory resource.
  CanNetwork(Can & can, std::pmr::memory_resource * memory_resource) noexcept
      : can_(can), messages_(memory_resource)
  {
  }

  void ModuleInitialize() override
  {
    // Do nothing...
  }

  void ModuleEnable(bool enable = true) override
  {
    if (enable)
    {
      auto receive_handler_lambda = [this](sjsu::Can & can) {
        ReceiveHandler(can);
      };
      can_.ConfigureReceiveHandler(receive_handler_lambda);
    }
    else
    {
      can_.ConfigureReceiveHandler(nullptr);
    }
  }

  /// In order for a CAN message with an associated ID to be stored in the
  /// CanNetwork, it must be declared using this method. For example if you
  /// expect to get the following IDs 0x140, 0x7AA, and 0x561 from the CAN bus,
  /// then this method must be called as such:
  ///
  ///    Node_t * motor_node       = can_network.CaptureMessage(0x140);
  ///    Node_t * encoder_node     = can_network.CaptureMessage(0x561);
  ///    Node_t * temperature_node = can_network.CaptureMessage(0x7AA);
  ///
  /// @param id - Associated ID of messages to be stored.
  /// @throw std::bad_alloc if this static storage allocated for this object is
  /// not enough to hold
  /// @return Node_t* - reference to the CANBUS network Node_t which can be used
  /// at anytime to retreive the latest received message from the CANBUS that is
  /// associated with the set ID.
  [[nodiscard]] Node_t * CaptureMessage(uint32_t id)
  {
    try
    {
      Node_t empty_node;

      // Insert an empty Node_t into the map with the following ID as
      // the key. This is how each ID is remembered when calls to
      // ReceiveHandler() and GetMessage() are performed. ReceiveHandler() will
      // ignore messages with IDs that are not already keys within the map.
      // GetMessage() will throw an exception.
      messages_.emplace(std::make_pair(id, empty_node));

      // Return reference to the newly made Node_t.
      return &messages_[id];
    }
    // will be thrown if memory resource is exhausted.
    catch (std::bad_alloc & e)
    {
      LogDebug("Could not add ID 0x%" PRIX32
               ", memory resource has been exhausted!",
               id);
      throw;
    }
  }

  /// Manually call the receive handler. This is useful for unit testing and for
  /// CANBUS peripherals that do NOT have a receive message interrupt routine.
  /// In the later case, a software (potentially a thread) can perform the
  /// receive call manually to extract messages from the CAN peripheral FIFO.
  /// This method cannot guarantee that data is not lost if the FIFO fills up.
  void ManuallyCallReceiveHandler()
  {
    ReceiveHandler(can_);
  }

  /// Return the CAN peripheral object which can be used to initialize,
  /// configure, and enable the peripheral as well as transmit messages.
  /// Access to this object, if a CanNetwork
  Can & CanBus()
  {
    return can_;
  }

  /// Meant for testing purposes or when direct inspection of the map is useful
  /// in userspace. Should not be used in SJSU-Dev2 libraries.
  const auto & GetInternalMap()
  {
    return messages_;
  }

  ~CanNetwork()
  {
    ModuleEnable(false);
  }

 private:
  void ReceiveHandler(sjsu::Can & can)
  {
    // If there isn't any data available, return early.
    if (!can.HasData())
    {
      return;
    }

    // Pop the latest can message off the queue.
    const auto kMessage = can.Receive();

    // Check if the map already has a value for this ID. This acts as the last
    // stage of the CAN filter for the CANBUS Network module. If the key
    // does NOT exist in the map, then this message will not be saved.
    // Typically, this only happens when the hardware filter has not been setup
    // properly to eliminate can messages that should be ignored.
    //
    // Map lookups can be costly, especially in a interrupt context, so only
    // needing to hash/lookup the ID once is preferred. To prevent multiple
    // lookups, an iterator is stored into the message_node variable.
    auto message_node = messages_.find(kMessage.id);

    // If the ID has an associated value in the map, then the node std::pair<>
    // is returned, otherwise, the ::end() node is returned.
    if (message_node != messages_.end())
    {
      message_node->second.Update(kMessage);
    }
  }

  Can & can_;
  std::pmr::unordered_map<uint32_t, Node_t> messages_;
};
}  // namespace sjsu
