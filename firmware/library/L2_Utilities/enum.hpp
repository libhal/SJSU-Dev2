// Enum.hpp includes enhancements to the "enum class" types.
#pragma once

#include <type_traits>

namespace util
{
// Returns the value of the enum class. This should be used in place of
// static_cast<some_numeric_type>(some_variable).
//
// @example
//
//      enum class SomeType : uint32_t { kValue1 = 1, kValue2 = 2 };
//      SomeType some_variable = SomeType::kValue1;
//      uint32_t numeric_variable = util::Value(some_variable);
//
// @param enum_type_value variable you would like to get the value of.
// @return the value of the enum class type variable of with the underlying
//         type of the enum class.
template <typename Enum, typename Type = typename std::underlying_type_t<Enum>>
constexpr Type Value(Enum enum_type_value)
{
  return static_cast<Type>(enum_type_value);
}
}  // namespace util

//
// Below is the set of bitwise operator overloads for enum class types
//

// This struct is used in the template evaluation to determine if bitwise
// operators are enabled.
// This generic instance will match with all enumeration types that do not have
// their own template specializations. This will disable bit mask operations for
// all types. See the comments for SJ2_ENABLE_BITMASK_OPERATIONS for more
// details for the template specialization.
template <typename Enum>
struct EnableBitMaskOperators_t
{
  static constexpr bool kEnable = false;
};
// This macro, when used on an enum class type, will create a specialized
// version of the "EnableBitMaskOperators_t" that enables that enum class
// to use bitwise operators without the need of static_cast.
//
// Example from within a class:
//
//      class SomeClass
//      {
//          enum class SomeEnum : int32_t { kValue1 = -1, kValue2 = 2 };
//      };
//      SJ2_ENABLE_BITMASK_OPERATORS(SomeClass::SomeEnum);
//
#define SJ2_ENABLE_BITMASK_OPERATORS(x)   \
  template <>                             \
  struct EnableBitMaskOperators_t<x>      \
  {                                       \
    static constexpr bool kEnable = true; \
  }

// @tparam Enum is the type used in this operator overload
// @tparam class= is used as a select. The compiler will use this
//                implementation of the | operator if that type has a
//                EnableBitMaskOperators_t<> specialization of the Enum type
//                or, in other words, the SJ2_ENABLE_BITMASK_OPERATORS was used
//                on it.
// The following is the same for all operators beyond this point.
template <typename Enum, class = typename std::enable_if_t<
                             EnableBitMaskOperators_t<Enum>::kEnable, Enum>>
constexpr Enum operator|(Enum lhs, Enum rhs)
{
  using underlying = typename std::underlying_type<Enum>::type;
  return static_cast<Enum>(static_cast<underlying>(lhs) |
                           static_cast<underlying>(rhs));
}

template <typename Enum, class = typename std::enable_if_t<
                             EnableBitMaskOperators_t<Enum>::kEnable, Enum>>
constexpr Enum operator&(Enum lhs, Enum rhs)
{
  using underlying = typename std::underlying_type<Enum>::type;
  return static_cast<Enum>(static_cast<underlying>(lhs) &
                           static_cast<underlying>(rhs));
}

template <typename Enum, class = typename std::enable_if_t<
                             EnableBitMaskOperators_t<Enum>::kEnable, Enum>>
constexpr Enum operator^(Enum lhs, Enum rhs)
{
  using underlying = typename std::underlying_type<Enum>::type;
  return static_cast<Enum>(static_cast<underlying>(lhs) ^
                           static_cast<underlying>(rhs));
}

template <typename Enum, class = typename std::enable_if_t<
                             EnableBitMaskOperators_t<Enum>::kEnable, Enum>>
constexpr Enum operator~(Enum rhs)
{
  using underlying = typename std::underlying_type<Enum>::type;
  return static_cast<Enum>(~static_cast<underlying>(rhs));
}

template <typename Enum, class = typename std::enable_if_t<
                             EnableBitMaskOperators_t<Enum>::kEnable, Enum>>
constexpr Enum & operator|=(Enum & lhs, Enum rhs)
{
  using underlying = typename std::underlying_type<Enum>::type;
  lhs              = static_cast<Enum>(static_cast<underlying>(lhs) |
                          static_cast<underlying>(rhs));
  return lhs;
}

template <typename Enum, class = typename std::enable_if_t<
                             EnableBitMaskOperators_t<Enum>::kEnable, Enum>>
constexpr Enum & operator&=(Enum & lhs, Enum rhs)
{
  using underlying = typename std::underlying_type<Enum>::type;
  lhs              = static_cast<Enum>(static_cast<underlying>(lhs) &
                          static_cast<underlying>(rhs));
  return lhs;
}

template <typename Enum, class = typename std::enable_if_t<
                             EnableBitMaskOperators_t<Enum>::kEnable, Enum>>
constexpr Enum & operator^=(Enum & lhs, Enum rhs)
{
  using underlying = typename std::underlying_type<Enum>::type;
  lhs              = static_cast<Enum>(static_cast<underlying>(lhs) ^
                          static_cast<underlying>(rhs));
  return lhs;
}
