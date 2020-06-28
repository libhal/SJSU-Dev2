#pragma once

#include <cinttypes>

#include "third_party/doctest/doctest/doctest.h"
// Quick and dirty way to have sub sections from Catch2 translate to SUBCASE in
// doctest.
#define SECTION SUBCASE
#include "third_party/fff/fff.h"

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
