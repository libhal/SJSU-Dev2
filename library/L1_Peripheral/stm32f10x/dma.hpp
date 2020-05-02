#pragma once

#include "L0_Platform/stm32f10x/stm32f10x.h"
#include "utility/bit.hpp"

namespace sjsu::stm32f10x
{
class Dma
{
 public:
  /// Namespace for the control registers (DMA->CCR) bit masks and predefined
  /// settings constants.
  struct Reg  // NOLINT
  {
    /// Declare this channel for Memory to memory mode
    static constexpr auto kMemoryToMemory = bit::MaskFromRange(14);

    /// Configure the channel priority for this channel.
    /// 0b00: Low
    /// 0b01: Medium
    /// 0b10: High
    /// 0b11: Very high
    static constexpr auto kChannelPriority = bit::MaskFromRange(12, 13);

    /// The size of each element of the memory.
    /// 0b00: 8-bits
    /// 0b01: 16-bits
    /// 0b10: 32-bits
    /// 0b11: Reserved
    static constexpr auto kMemorySize = bit::MaskFromRange(10, 11);

    /// The peripheral register size.
    /// 0b00: 8-bits
    /// 0b01: 16-bits
    /// 0b10: 32-bits
    /// 0b11: Reserved
    static constexpr auto kPeripheralSize = bit::MaskFromRange(8, 9);

    /// Activate memory increment mode, which will increment the memory address
    /// with each transfer
    static constexpr auto kMemoryIncrementEnable = bit::MaskFromRange(7);

    /// Activate memory increment mode, which will increment the peripheral
    /// address with each transfer
    static constexpr auto kPeripheralIncrementEnable = bit::MaskFromRange(6);

    /// DMA will continuous load bytes into the buffer supplied in a circular
    /// buffer manner.
    static constexpr auto kCircularMode = bit::MaskFromRange(5);

    /// Data transfer direction
    /// 0: Read from peripheral
    /// 1: Read from memory
    static constexpr auto kDataTransferDirection = bit::MaskFromRange(4);

    /// Enable interrupt on transfer error
    static constexpr auto kTransferErrorInterruptEnable = bit::MaskFromRange(3);

    /// Enable interrupt on half of data transferred
    static constexpr auto kHalfTransferInterruptEnable = bit::MaskFromRange(2);

    /// Enable interrupt on complete transfer
    static constexpr auto kTransferCompleteInterruptEnable =
        bit::MaskFromRange(1);

    /// Enable this DMA channel
    static constexpr auto kEnable = bit::MaskFromRange(0);
  };
};
}  // namespace sjsu::stm32f10x
