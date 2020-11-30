/// @file bit.hpp
/// Stand alone bit manipulation library to make performing bit
/// manipulation on target registers and values easy, expressive, and readable.
#pragma once

#include <array>
#include <cstdint>
#include <cstddef>
#include <limits>
#include <type_traits>

namespace sjsu
{
/// The Endianess of the system
enum class Endian
{
  kLittle,
  kBig
};

namespace bit
{
/// Definition of a bit mask. Can be used in various bit manipulation functions
/// to specify which bits to be modified.
struct Mask  // NOLINT
{
  /// Starting position of the bitfield
  uint32_t position;
  /// Number of bits of the bitfield
  uint32_t width;

  /// Comparison operator for the mask structure
  constexpr bool operator==(const Mask & rhs) const
  {
    return (position == rhs.position && width == rhs.width);
  }

  /// Comparison operator for the mask structure
  constexpr bool operator==(const Mask && rhs) const
  {
    return (position == rhs.position && width == rhs.width);
  }

  /// Move the position of the mask down by the shift amount
  ///
  /// @param shift_amount - amount to reduce the position by
  constexpr Mask operator>>(int shift_amount) const
  {
    Mask new_mask     = *this;
    new_mask.position = (position - shift_amount);
    return new_mask;
  }

  /// Move the position of the mask up by the shift amount
  ///
  /// @param shift_amount - amount to increase the position by
  constexpr Mask operator<<(int shift_amount) const
  {
    Mask new_mask     = *this;
    new_mask.position = (position + shift_amount);
    return new_mask;
  }
};

/// @param low_bit_position - the starting bit of the bit field.
/// @param high_bit_position - the last bit of the bit field.
/// @return constexpr Mask from the low bit position to the high bit position.
///         If the low_bit_position > high_bit_position, the result is
///         undefined.
constexpr Mask MaskFromRange(uint32_t low_bit_position,
                             uint32_t high_bit_position)
{
  return Mask({
      .position = low_bit_position,
      .width    = (1 + (high_bit_position - low_bit_position)),
  });
}

/// @param bit_position - bit field composed of a single bit with bit width 1.
/// @return a bit mask with of width 1 and position = bit_position.
constexpr Mask MaskFromRange(uint32_t bit_position)
{
  return Mask({
      .position = bit_position,
      .width    = 1,
  });
}

/// Extract a set of contiguous bits from a target value.
///
///     target   =        0x00FE'DCBA
///                                ^
///                               /
///                              /
///     value    = 4 -----------+
///     width    = 8
///
///     return   = 0xCB
///
/// @param target the target containing the bits to be extracted.
/// @param position the starting position of the bits to extracted.
/// @param width the number of bits from the starting position to be extracted.
template <typename T>
[[nodiscard]] constexpr T Extract(T target,
                                  uint32_t position,
                                  uint32_t width = 1)
{
  // Check the types at compile time
  static_assert(std::numeric_limits<T>::is_integer,
                "Extract only accepts intergers.");
  // Need to use an unsigned version of the type T for the mask to make sure
  // that the shift right doesn't result in a sign extended shift.
  using UnsignedT = typename std::make_unsigned<T>::type;
  // At compile time, generate variable containing all 1s with the size of the
  // target parameter.
  constexpr UnsignedT kFieldOfOnes = std::numeric_limits<UnsignedT>::max();
  // At compile time calculate the number of bits in the target parameter.
  constexpr size_t kTargetWidth = sizeof(T) * 8;
  // Create mask by shifting the set of 1s down so that the number of 1s from
  // bit position 0 is equal to the width parameter.
  UnsignedT mask =
      static_cast<UnsignedT>(kFieldOfOnes >> (kTargetWidth - width));
  // Shift target down to the right to get the bit position in the 0th bit
  // location.
  // Mask the value to clear any bit after the width's amount of bits.
  return static_cast<T>((target >> position) & mask);
}

template <typename T>
[[nodiscard]] constexpr T Extract(T target, Mask bitmask)
{
  return Extract(target, bitmask.position, bitmask.width);
}

/// Extract value and sign extend it.
///
/// @tparam T - return type container.
/// @tparam U - type of the input target value.
/// @param target - Value to extract the signed data from.
/// @param bitmask - The location of the signed data to extract from the target.
/// @return constexpr T - extracted and sign extended value.
template <typename T, typename U>
[[nodiscard]] constexpr T SignedExtract(U target, Mask bitmask)
{
  // Check the types at compile time
  static_assert(std::numeric_limits<U>::is_integer,
                "Extract only accepts intergers.");
  static_assert(std::numeric_limits<T>::is_integer,
                "Extract only accepts intergers.");
  static_assert(std::is_signed_v<T>, "Return value must be a signed.");

  constexpr size_t kBits    = sizeof(T) * 8;
  const size_t kShiftAmount = (kBits - bitmask.width) + 1;

  T value = static_cast<T>(Extract(target, bitmask));
  value <<= kShiftAmount;
  value = value >> kShiftAmount;

  return value;
}

/// Insert a set of contiguous bits into a target value.
///
///     target   =        0xXXXX'XXXX
///                            ^
///                           /
///                          /
///     value    = 0xABCD --+
///     position = 16
///     width    = 16
///
///     return   =        0xABCD'XXXX
///
/// @param target the target that will have bits inserted into it.
/// @param value the bits to be inserted into the target
/// @param position the position in the target to insert the value of bits.
/// @param width the length of bits that will be overwritten in the target.
template <typename T, typename U>
[[nodiscard]] constexpr T Insert(T target,
                                 U value,
                                 uint32_t position,
                                 uint32_t width = 1)
{
  // Check the types at compile time
  static_assert(
      std::numeric_limits<T>::is_integer,
      "1st parameter (target) of Insert function must be an interger.");
  static_assert(
      std::numeric_limits<U>::is_integer,
      "2nd parameter (value) of Insert function must be an interger.");
  static_assert(sizeof(T) >= sizeof(U),
                "2nd parameter (value) must be smaller than or equal to the "
                "size of the target value.");
  // Need to use an unsigned version of the type T for the mask to make sure
  // that the shift right doesn't result in a sign extended shift.
  using UnsignedT = typename std::make_unsigned<T>::type;
  // At compile time, generate variable containing all 1s with the size of the
  // target parameter.
  constexpr UnsignedT kFieldOfOnes = std::numeric_limits<UnsignedT>::max();
  // At compile time calculate the number of bits in the target parameter.
  constexpr size_t kTargetWidth = sizeof(T) * 8;
  // Create mask by shifting the set of 1s down so that the number of 1s from
  // bit position 0 is equal to the width parameter.
  UnsignedT mask =
      static_cast<UnsignedT>(kFieldOfOnes >> (kTargetWidth - width));
  // Clear width's number of bits in the target value at the bit position
  // specified.
  target = static_cast<UnsignedT>(target & ~(mask << position));
  // AND value with mask to remove any bits beyond the specified width.
  // Shift masked value into bit position and OR with target value.
  target = static_cast<UnsignedT>(target | (value & mask) << position);
  return static_cast<T>(target);
}

template <typename T, typename U>
[[nodiscard]] constexpr T Insert(T target, U value, Mask bitmask)
{
  return Insert(target, value, bitmask.position, bitmask.width);
}

/// Set a bit in the target value at the position specifed to a 1 and return
///
///    target   =        0b0000'1001
///                           ^
///                          /
///                         /
///    position = 4 -------+
///
///    return   =        0b0001'1001
///
/// @param target the value you want to change
/// @param position the position of the bit you would like to change to 1
template <typename T>
[[nodiscard]] constexpr T Set(T target, uint32_t position)
{
  static_assert(std::numeric_limits<T>::is_integer,
                "Set only accepts intergers.");
  T mask = static_cast<T>(1);
  mask   = static_cast<T>(mask << position);
  return static_cast<T>(target | mask);
}

/// Set a bit in the target value at the position specifed to a 0 and return
///
///     target   =        0b0000'1001
///                              ^
///                             /
///                            /
///     position = 3 ---------+
///
///     return   =        0b0000'0001
///
/// @param target the value you want to change
/// @param position the position of the bit you would like to change to 0
template <typename T>
[[nodiscard]] constexpr T Clear(T target, uint32_t position)
{
  static_assert(std::numeric_limits<T>::is_integer,
                "Clear only accepts intergers.");
  T mask = static_cast<T>(1);
  mask   = static_cast<T>(mask << position);
  return static_cast<T>(target & ~mask);
}

/// Toggle a bit in the target value at the position specifed.
/// If the bit was a 1, it will be changed to a 0.
/// If the bit was a 0, it will be changed to a 1.
///
///     target   =        0b0000'1001
///                              ^
///                             /
///                            /
///     position = 3 ---------+
///
///     return   =        0b0000'0001
///
/// @param target the value you want to change
/// @param position the position of the bit you would like to toggle
template <typename T>
[[nodiscard]] constexpr T Toggle(T target, uint32_t position)
{
  static_assert(std::numeric_limits<T>::is_integer,
                "Toggle only accepts intergers.");
  T mask = static_cast<T>(1);
  mask   = static_cast<T>(mask << position);
  return static_cast<T>(target ^ mask);
}

/// Read a bit from the target value at the position specifed.
/// If the bit is 1 at the position given, return true.
/// If the bit is 0 at the position given, return false.
///
///     target   =        0b0000'1001
///                              |
///                             /|
///                            / |
///     position = 3 ---------+  |
///                             \|/
///     return   =              true
///
/// @param target the value you want to change
/// @param position the position of the bit you would like to toggle
template <typename T>
[[nodiscard]] constexpr bool Read(T target, uint32_t position)
{
  static_assert(std::numeric_limits<T>::is_integer,
                "Read only accepts intergers.");
  T mask = static_cast<T>(1);
  mask   = static_cast<T>(mask << position);
  return static_cast<bool>(target & mask);
}

/// @returns a value that is the target value with the bit set to a 1 at the bit
/// position within the bitmask. For example if your bitmask has field position
/// set to 5, then this function will return the target value with the 5th bits
/// set to a 1.
template <typename T>
[[nodiscard]] constexpr T Set(T target, Mask bitmask)
{
  return Set(target, bitmask.position);
}

/// Operates the same way as the Set() function except it clears the bit (set
/// the bit's value to zero).
template <typename T>
[[nodiscard]] constexpr T Clear(T target, Mask bitmask)
{
  return Clear(target, bitmask.position);
}

/// Operates the same way as the Set() function except it toggles the bit.
template <typename T>
[[nodiscard]] constexpr bool Toggle(T target, Mask bitmask)
{
  return Toggle(target, bitmask.position);
}

/// @returns the bit in the value at the "position" field of the bitmask. For
/// example, if the passed bitmask has position set to 5, then this function
/// will return the 5th bits value, regardless of the "width" field is.
template <typename T>
[[nodiscard]] constexpr bool Read(T target, Mask bitmask)
{
  return Read(target, bitmask.position);
}

/// Extract a set of contiguous bits from an array of bytes. The byte stream and
/// the result are both assumed to be the same endianness.
///
/// Example (Little Endian bit aligned):
///
///    uint8_t array[] = { 0xAB, 0xCD, 0xEF };
///    uint16_t result = StreamExtract<uint16_t>(array,
///                                              sizeof(array),
///                                              { .position = 0, width = 16 },
///                                              Endian::kLittle);
///    result => 0xCDEF
///
/// Example (Big Endian bit aligned):
///
///    uint8_t array[] = { 0xAB, 0xCD, 0xEF };
///    uint16_t result = StreamExtract<uint16_t>(array,
///                                              sizeof(array),
///                                              { .position = 0, width = 16 },
///                                              Endian::kBig);
///    result => 0xCDAB
///
/// Example (Little Endian bit unaligned):
///
///    uint8_t array[] = { 0xAB, 0xCD, 0xEF };
///    uint16_t result = StreamExtract<uint16_t>(array,
///                                              sizeof(array),
///                                              { .position = 4, width = 16 },
///                                              Endian::kLittle);
///    result => 0xBCDE
///
/// Example (Big Endian bit unaligned):
///
///    uint8_t array[] = { 0xAB, 0xCD, 0xEF };
///    uint16_t result = StreamExtract<uint16_t>(array,
///                                              sizeof(array),
///                                              { .position = 4, width = 16 },
///                                              Endian::kBig);
///    result => 0xFCDB
///
/// @tparam T - numeric type to be returned. Can only be uint8_t, 16, 32 and 64.
/// @param stream - byte array containing the bits to be extracted
/// @param size - number of bytes of the byte array
/// @param mask - the mask containing the location of the bits to be extracted.
/// @param endian - indicates if the value is little or big endian.
/// @return constexpr T - bits extracted from the stream.
template <typename T>
constexpr T StreamExtract(const uint8_t * stream,
                          size_t size,
                          Mask mask,
                          sjsu::Endian endian = sjsu::Endian::kLittle)
{
  static_assert(
      std::is_same<T, uint8_t>::value || std::is_same<T, uint16_t>::value ||
          std::is_same<T, uint32_t>::value || std::is_same<T, uint64_t>::value,
      "only unsigned integers are allowed to be used in "
      "sjsu::bit::StreamExtract().");

  // Diving the starting position by 8 will give you the starting byte position.
  // The fractional part is truncated, which gives the correct answer. If it
  // were rounded, the answer would be incorrect and would require the use of
  // the floor() function.
  const size_t kStartByte = mask.position / 8;
  // We need to figure out if the bit position is not byte aligned. We can do
  // this by taking the modulus of the mask's bit starting position.
  const size_t kStartingBitOffset = mask.position % 8;
  // We need to figure out if the bit position of the last bits not byte
  // aligned. This is simply to determine if we need to consume an extra byte
  // from the stream.
  const size_t kEndingBitOffset = (mask.position + mask.width) % 8;

  // Contains the total number of bytes that will need to extracted from the
  // byte stream. The number of bytes can be calculated by taking the bit width
  // desired and dividing it by the size of a byte (8-bits).
  size_t total_bytes = mask.width / 8;

  if (kStartingBitOffset != 0)
  {
    total_bytes++;
  }

  if (kEndingBitOffset != 0)
  {
    total_bytes++;
  }

  T result = 0;

  // A pointer to the starting byte of the sequence to pull bytes from.
  const uint8_t * next_byte_ptr = &stream[kStartByte];
  // The direction in which we will pull bytes. This is assumed BIG Endian,
  // thus, the LSB is the first byte. Going forward will pull out more
  // significant bytes.
  int scan_direction = 1;

  if (endian == sjsu::Endian::kLittle)
  {
    // Little Endian has its LSB at the end of the array, thus we will pull
    // bytes from the end of the byte array.
    next_byte_ptr = &stream[(size - 1) - kStartByte];
    // To get the more significate bytes, we need to progress through the array
    // backwards, so we need to increase backwards.
    scan_direction = -1;
  }

  for (uint32_t i = 0; i < total_bytes && kStartByte + i < size; i++)
  {
    const int kShiftAmount  = (8 * i) - kStartingBitOffset;
    const uint8_t kNextByte = *next_byte_ptr;
    next_byte_ptr += scan_direction;
    T finished_byte = 0;

    if (kShiftAmount > 0)
    {
      finished_byte = kNextByte << kShiftAmount;
    }
    else
    {
      finished_byte = kNextByte >> -kShiftAmount;
    }
    result |= finished_byte;
  }

  Mask trimming_mask     = mask;
  trimming_mask.position = 0;
  result                 = Extract(result, trimming_mask);

  return result;
}

/// Extract a set of contiguous bits from a std::array of bytes. See
/// StreamExtract() above for further details.
///
/// @tparam T - numeric type to be returned. Can only be uint8_t, 16, 32 and 64.
/// @tparam size - deduced size of the std::array that was passed in.
/// @param stream - byte array containing the bits to be extracted
/// @param mask - the mask containing the location of the bits to be extracted.
/// @param endian - indicates if the value is little or big endian.
/// @return constexpr T - bits extracted from the stream.
template <typename T, size_t size>
constexpr T StreamExtract(const std::array<uint8_t, size> & stream,
                          Mask mask,
                          Endian endian = Endian::kLittle)
{
  return StreamExtract<T>(stream.data(), stream.size(), mask, endian);
}

// TODO(#1173): Add unit tests for this class.
///
/// @tparam T - the numeric type of the register. This should not be explicitly
///             defined.
template <typename T>
class Register
{
 public:
  /// @param reg - address of the register to manipulate the bits of.
  explicit constexpr Register(volatile T * reg) : reg_(reg), value_(*reg) {}

  /// Set bit in the bitmask starting position. Value will not be
  /// saved to the register until the `Save()` method has been called.
  ///
  /// @param mask - the location of the bit to be set.
  /// @return constexpr Value& - return reference to itself to allow for method
  ///                            chaining.
  constexpr Register & Set(Mask mask)
  {
    value_ = sjsu::bit::Set(value_, mask);
    return *this;
  }

  /// Clear bit in the bitmask starting position. Value will not be
  /// saved to the register until the `Save()` method has been called.
  ///
  /// @param mask - the location of the bit to be cleared.
  /// @return constexpr Value& - return reference to itself to allow for method
  ///                            chaining.
  constexpr Register & Clear(Mask mask)
  {
    value_ = sjsu::bit::Clear(value_, mask);
    return *this;
  }

  /// Insert the value into the bitmask location of the value. Value will not be
  /// saved to the register until the `Save()` method has been called.
  ///
  /// @param value - value to insert into the bitmask location
  /// @param bitmask - the location to insert the value into
  /// @return constexpr Value& - return reference to itself to allow for method
  ///                            chaining.
  constexpr Register & Insert(T value, Mask bitmask)
  {
    value_ = sjsu::bit::Insert<T>(value_, static_cast<T>(value), bitmask);
    return *this;
  }

  /// Save the value into the register
  ///
  /// @return constexpr Value& - return reference to itself to allow for method
  ///                            chaining.
  constexpr Register & Save()
  {
    *reg_ = value_;
    return *this;
  }

  /// Extract bits from register
  ///
  /// @param bitmask - location in the register to extract bits from.
  /// @return constexpr T - the extracted bits from the register.
  constexpr T Extract(Mask bitmask)
  {
    return sjsu::bit::Extract(*reg_, bitmask);
  }

  /// @return constexpr T - return the value of the register.
  constexpr T Get()
  {
    return *reg_;
  }

  /// Read a bit from the target value at the position specifed.
  ///
  /// @param mask - location in the register to read from.
  /// @return true - if the starting position of the mask is set high.
  constexpr bool Read(Mask mask)
  {
    return sjsu::bit::Read(*reg_, mask);
  }

 private:
  volatile T * reg_;
  T value_;
};

// TODO(#1173): Add unit tests for this class.
/// @tparam T - the size of the bit::Value register. Defaults to uint32_t.
template <typename T = uint32_t>
class Value
{
 public:
  /// Construct value with its value default initialized to zero
  constexpr Value() : value_(0) {}

  /// Construct value with its value default initalized to `initial_value`
  ///
  /// @param initial_value - the initial value of this class.
  constexpr explicit Value(T initial_value) : value_(initial_value) {}

  /// Set bit in the bitmask starting position
  ///
  /// @param mask - the location of the bit to be set.
  /// @return constexpr Value& - return reference to itself to allow for method
  ///                            chaining.
  constexpr Value & Set(Mask mask)
  {
    value_ = sjsu::bit::Set(value_, mask);
    return *this;
  }

  /// Clear bit in the bitmask starting position.
  ///
  /// @param mask - the location of the bit to be cleared.
  /// @return constexpr Value& - return reference to itself to allow for method
  ///                            chaining.
  constexpr Value & Clear(Mask mask)
  {
    value_ = sjsu::bit::Clear(value_, mask);
    return *this;
  }

  /// Insert the value into the bitmask location of the value.
  ///
  /// @param value - value to insert into the bitmask location
  /// @param bitmask - the location to insert the value into
  /// @return constexpr Value& - return reference to itself to allow for method
  ///                            chaining.
  constexpr Value & Insert(T value, Mask bitmask)
  {
    value_ = sjsu::bit::Insert<T>(value_, static_cast<T>(value), bitmask);
    return *this;
  }

  /// Allows implicit conversion from this type into the integer type.
  /// @return T - the type of this value
  constexpr operator T() const
  {
    return value_;
  }

 private:
  T value_;
};

}  // namespace bit
}  // namespace sjsu
