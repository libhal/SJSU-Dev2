#pragma once

#include <type_traits>

namespace sjsu
{
/// @defgroup inactives_module Inactive Peripherals
/// @brief The GetInactive() returns an implementation of an L1 peripheral that
/// does nothing. We call these peripherals the inactives. They are meant to do
/// nothing, or return trivial values when created and called. This is meant in
/// cases where a particular module requires a particular peripheral to operate,
/// but you want to substitute it out with a dummy implementation.
/// @{

/// Templated struct with a boolean value field of false for any types that are
/// not specialized like the list below. Used for compile time check usage of
/// GetInactive().
template <typename T>
struct UnsupportedInactivePeripheral_t : std::false_type
{
};

/// Default template behaviour for an attempt to create an inactive sjsu L1
/// interface for an interface that is not yet supported. Will generate a custom
/// compile time error message.
template <typename T>
inline T & GetInactive()
{
  static_assert(UnsupportedInactivePeripheral_t<T>::value,
                "There does not exist an inactive variant of this peripheral");
}
/// @}
}  // namespace sjsu
