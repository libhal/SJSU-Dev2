#pragma once

// If your memory map uses the standard C integer definitions, you need to
// include them outside of the namespace, otherwise, they will be captured in
// the namespace below. Without this, the namespace of C libraries will break
// any code that includes this file and attempts to use the std ints.
#include <cstdint>

namespace sjsu
{
namespace example  // rename to name of platform
{
#include "example_memory_map_c.h"
}  // namespace example
}  // namespace sjsu
