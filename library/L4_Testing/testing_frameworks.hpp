#pragma once

#include <cinttypes>
#include <span>

#include "third_party/doctest/doctest/doctest.h"
// Quick and dirty way to have sub sections from Catch2 translate to SUBCASE in
// doctest.
#define SECTION SUBCASE
#include "third_party/fakeit/fakeit.hpp"
#include "third_party/fff/fff.h"

using namespace fakeit;  // NOLINT

int HostWrite(const char * payload, size_t length);
int HostRead(char * payload, size_t length);

/// This magical method does a lot of tricks to convince the compiler to emit
/// as many of the class methods as possible for methods of a function that may
/// only exist in a header file.
#define EMIT_ALL_METHODS(class_name)                         \
  namespace                                                  \
  {                                                          \
  template <unsigned test_parameter>                         \
  using class_name##_Template = class_name;                  \
  int __LetsGo_##class_name(class_name##_Template<0> * obj)  \
  {                                                          \
    bool never_true = false;                                 \
    bool * ptr      = &never_true;                           \
    /* Emit destructors */                                   \
    if (*ptr)                                                \
    {                                                        \
      class_name##_Template<0> emitter = *obj;               \
      (void)emitter;                                         \
      return 1;                                              \
    }                                                        \
    return 0;                                                \
  }                                                          \
  auto __full_##class_name = __LetsGo_##class_name(nullptr); \
  }  // namespace



template <typename T>
struct Reflection
{
  static constexpr const char * name()
  {
    return "?";
  }
};

template <>
struct Reflection<uint8_t>
{
  static constexpr const char * name()
  {
    return "uint8_t";
  }
};

template <>
struct Reflection<const uint8_t>
{
  static constexpr const char * name()
  {
    return "const uint8_t";
  }
};

template <>
struct Reflection<uint16_t>
{
  static constexpr const char * name()
  {
    return "uint16_t";
  }
};

template <>
struct Reflection<uint32_t>
{
  static constexpr const char * name()
  {
    return "uint32_t";
  }
};

template <>
struct Reflection<uint64_t>
{
  static constexpr const char * name()
  {
    return "uint64_t";
  }
};

template <>
struct Reflection<int8_t>
{
  static constexpr const char * name()
  {
    return "int8_t";
  }
};

template <>
struct Reflection<int16_t>
{
  static constexpr const char * name()
  {
    return "int16_t";
  }
};

template <>
struct Reflection<int32_t>
{
  static constexpr const char * name()
  {
    return "int32_t";
  }
};

template <>
struct Reflection<int64_t>
{
  static constexpr const char * name()
  {
    return "int64_t";
  }
};

namespace doctest
{
template <typename T, size_t N>
struct StringMaker<std::array<T, N>>
{
  static String convert(const std::array<T, N> & array)
  {
    std::string str;

    str += "std::array<";
    str += Reflection<T>::name();
    str += ", ";
    str += std::to_string(N);
    str += ">{";

    size_t i = 0;
    for (i = 0; i < N - 1; i++)
    {
      str += std::to_string(array[i]) + ", ";
    }

    str += std::to_string(array[i]);

    str += " }";

    return str.c_str();
  }
};

template <typename T>
struct StringMaker<std::span<T>>
{
  static String convert(const std::span<T> & span)
  {
    std::string str;

    str += "std::span<";
    str += Reflection<T>::name();
    str += ", ";
    str += std::to_string(span.size());
    str += ">{ ";

    size_t i = 0;
    for (i = 0; i < span.size() - 1; i++)
    {
      str += std::to_string(span[i]) + ", ";
    }

    str += std::to_string(span[i]);

    str += " }";

    return str.c_str();
  }
};
}  // namespace doctest

namespace sjsu::testing
{
/// Best practice way to clear a structure of its contents to all zeros for
/// testing. Test developers should not call memset directly in their code. This
/// will handle deducing the size of the structure which an lead to problems if
/// there is a typo.
///
/// @tparam T - type of the structure (used to deduce the size of the type)
/// @param data_structure - data structure to set to all zeros.
template <class T>
inline void ClearStructure(T * data_structure)
{
  memset(data_structure, 0, sizeof(*data_structure));
}
}  // namespace sjsu::testing
