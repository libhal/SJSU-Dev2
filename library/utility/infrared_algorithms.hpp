#pragma once

#include <cstdint>

#include "utility/bit.hpp"
#include "utility/log.hpp"
#include "utility/units.hpp"

namespace sjsu
{
namespace infrared
{
// ==============================
// Infrared Data Frame Structures
// ==============================

/// A structure containing the timestamps for the received de-modulated infrared
/// data frame.
struct DataFrame_t
{
  /// Maximum size of the pulse_buffer. A value of 67 is chosen since formats
  /// using pulse duration modulation consists of a header (2 pulses), up to 32
  /// bits of data (64 pulses), and a stop pulse.
  inline static constexpr size_t kMaxPulseBufferSize = 67;
  /// Buffer containing the pulse edge timestamps.
  uint16_t pulse_buffer[kMaxPulseBufferSize];
  /// Current number of timestamps of pulse edges stored in pulse_buffer.
  size_t pulse_buffer_length;
};

/// A structure containing a decoded infrared data frame.
struct DecodedFrame_t
{
  /// The decoded data. This value is set to kRepeatCode to signify a repeat
  /// command is detected when decoding.
  uint32_t data;
  /// True if the frame was successfully decoded.
  bool is_valid;
  /// True if the decoded frame is a repeat frame.
  bool is_repeat;
};

// ==============================
// Helper Methods for Decoding
// ==============================

/// Helper function to check if a duration is within an acceptable tolerance.
///
/// @param duration The duration, in microseconds, to check.
/// @param expected_duration The expected duration to check against.
/// @param tolerance The acceptable tolerance percentage.
/// @return True if the specified duration is within tolerance of the expected
///         duration.
inline bool IsDurationWithinTolerance(
    uint16_t duration,
    std::chrono::microseconds expected_duration,
    float tolerance)
{
  // Calculate the acceptable minimum and maximum ranges:
  // min = expected_duration * (100% - tolerance)
  // max = expected_duration * (100% + tolerance)
  uint16_t min = static_cast<uint16_t>(
      static_cast<float>(expected_duration.count()) * (1.0f - tolerance));
  uint16_t max = static_cast<uint16_t>(
      static_cast<float>(expected_duration.count()) * (1.0f + tolerance));
  return (min <= duration) && (duration <= max);
}

// ==================================
// Pulse Duration Modulation Decoding
// ==================================

/// Pulse Duration Modulation uses either the "mark" or "space" to determine
/// logic levels of a data bit.
///
///  _____________________                       ___________
/// |                     |                     |
/// ^                     v                     ^
/// |                     |_____________________|
///
/// |<------ mark ------->|<------ space ------>|
enum class PulseDurationType
{
  kLength,    // The mark is used to determine the logic level
  kDistance,  // The space is used to determine the logic level
};

/// A structure containing the configuration settings for infrared data formats
/// that utilize pulse duration modulation.
///
/// @note The tolerance value is important for verifying a captured pulse
///       duration because the captured duration will not be the exact value of
///       the specified duration configurations due to varying latency.
struct PulseDurationConfiguration_t
{
  /// The expected duration of the header mark.
  std::chrono::microseconds header_mark_duration;
  /// The expected duration of the header space.
  std::chrono::microseconds header_space_duration;
  /// The expected fixed duration for a data pulse based on the specified
  /// encoding_type.
  std::chrono::microseconds data_duration;
  /// The expected duration to recognize a data pulse as a logical high.
  std::chrono::microseconds logic_high_duration;
  /// The expected duration to recognize a data pulse as a logical low.
  std::chrono::microseconds logic_low_duration;
  /// The pulse duration encoding type.
  PulseDurationType encoding_type;
  /// The acceptable tolerance level in percentage of each of the duration
  /// configurations. For example, a value of 0.25f means ±25%.
  float tolerance;
  /// True if the encoding uses a repeat frame. That is after the initial frame,
  /// a repeat code is continuously received when a button is held down. For
  /// most protocols this can be determined by the different duration in the
  /// header space.
  bool uses_repeat_frames = false;
  /// This optional value should be set if repeat frames with a different header
  /// space is used.
  std::chrono::microseconds header_repeat_space = 0us;
};

/// Generic decoding function to decode data frames that is encoded using pulse
/// duration modulation.
///
/// @note This function can detect repeat frames for most protocols that uses a
///       repeat frame such as NEC and Samsung by checking the different header
///       space. However, for the JVC protocol only the initial frame can be
///       decoded since the header is removed during the consecutive repeat
///       frames.
///
/// @param data_frame The desired data frame to decode.
/// @param configurations The pulse duration encoding configuration used for
///                       decoding the frame.
/// @return The decoded frame. If the frame was invalid, the is_valid flag will
///         be false. A frame can be invalid if it is corrupt or an invalid
///         configuration is used for decoding.
inline const DecodedFrame_t Decode(
    const DataFrame_t * data_frame,
    const PulseDurationConfiguration_t & configurations)
{
  static constexpr DecodedFrame_t kInvalidFrame = {
    .data      = 0,
    .is_valid  = false,
    .is_repeat = false,
  };
  // Minimum number of pulses to prevent array out of bounds.
  // Additionally, if the encoding protocol uses repeats sequences, there needs
  // to be 3 pulses which consists of a header mark, header space, and a stop
  // mark.
  constexpr uint8_t kMinimumPulseCount = 3;
  if (data_frame->pulse_buffer_length < kMinimumPulseCount)
  {
    sjsu::LogDebug("Invalid frame too short");
    return kInvalidFrame;
  }
  // Validate the frame's header by checking the first two pulses
  const uint16_t kHeaderMark  = data_frame->pulse_buffer[0];
  const uint16_t kHeaderSpace = data_frame->pulse_buffer[1];
  sjsu::LogDebug("%d %d", kHeaderMark, kHeaderSpace);
  const bool kHeaderMarkIsValid =
      IsDurationWithinTolerance(kHeaderMark,
                                configurations.header_mark_duration,
                                configurations.tolerance);
  if (!kHeaderMarkIsValid)
  {
    sjsu::LogDebug("Invalid header mark: %d", kHeaderMark);
    return kInvalidFrame;
  }
  // If the encoding utilizes a repeat frame, check for the repeat duration in
  // the header space before verifying it with the default
  // header_space_duration. This is because the header space is different in a
  // repeat sequence.
  if ((data_frame->pulse_buffer_length == kMinimumPulseCount) &&
      configurations.uses_repeat_frames)
  {
    const uint16_t kStopMark = data_frame->pulse_buffer[2];
    const bool kRepeatSpaceValid =
        IsDurationWithinTolerance(kHeaderSpace,
                                  configurations.header_repeat_space,
                                  configurations.tolerance);
    const bool kStopMarkValid = IsDurationWithinTolerance(
        kStopMark, configurations.data_duration, configurations.tolerance);
    if (kRepeatSpaceValid && kStopMarkValid)
    {
      return DecodedFrame_t({
          .data      = 0,
          .is_valid  = true,
          .is_repeat = true,
      });
    }
  }
  const bool kHeaderSpaceIsValid =
      IsDurationWithinTolerance(kHeaderSpace,
                                configurations.header_space_duration,
                                configurations.tolerance);
  if (!kHeaderSpaceIsValid)
  {
    sjsu::LogDebug("Invalid header space: %d", kHeaderSpace);
    return kInvalidFrame;
  }
  // Set all bits to be logically low and only set to high if needed during
  // decoding.
  uint32_t decoded_data = 0;
  // Start decoding the data pulses from the first data pulse edge and stop at
  // the last post (the stop pulse)
  //
  // The following is performed based on the modulation type:
  //   Pulse Distance Modulation:
  //     1. Verify the fixed mark duration.
  //     2. Verify the space for the logic level and validity.
  //   Pulse Length Modulation:
  //     1. Verify the fixed space duration.
  //     2. Verify the mark for the logic level and validity.
  for (size_t i = 2; i < data_frame->pulse_buffer_length - 1; i += 2)
  {
    const uint16_t kDataMark  = data_frame->pulse_buffer[i];
    const uint16_t kDataSpace = data_frame->pulse_buffer[i + 1];
    sjsu::LogDebug("%d %d", kDataMark, kDataSpace);

    uint16_t fixed_data_duration;
    uint16_t duration_for_logic_level;
    switch (configurations.encoding_type)
    {
      case PulseDurationType::kDistance:
        fixed_data_duration      = kDataMark;
        duration_for_logic_level = kDataSpace;
        break;
      case PulseDurationType::kLength:
        fixed_data_duration      = kDataSpace;
        duration_for_logic_level = kDataMark;
        break;
    }

    if (!IsDurationWithinTolerance(fixed_data_duration,
                                   configurations.data_duration,
                                   configurations.tolerance))
    {
      return kInvalidFrame;
    }

    decoded_data <<= 1;
    if (IsDurationWithinTolerance(duration_for_logic_level,
                                  configurations.logic_high_duration,
                                  configurations.tolerance))
    {
      decoded_data = sjsu::bit::Set(decoded_data, 0);
    }
    else if (!IsDurationWithinTolerance(duration_for_logic_level,
                                        configurations.logic_low_duration,
                                        configurations.tolerance))
    {
      return kInvalidFrame;
    }
    // else do nothing, this pulse is valid but the bit is a logical low
  }
  return DecodedFrame_t({
      .data      = decoded_data,
      .is_valid  = true,
      .is_repeat = false,
  });
}
}  // namespace infrared
}  // namespace sjsu
