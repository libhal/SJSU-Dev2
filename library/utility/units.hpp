#pragma once

// This file configures and pulls in the units library.
#undef _U
#undef _L
#undef _N
#undef _S
#undef _P
#undef _C
#undef _X
#undef _B
#define BUILD_TESTS OFF
#define UNIT_LIB_DISABLE_IOSTREAM
#define UNIT_LIB_DEFAULT_TYPE float

#define DISABLE_PREDEFINED_UNITS
#define ENABLE_PREDEFINED_LENGTH_UNITS
#define ENABLE_PREDEFINED_FREQUENCY_UNITS
#define ENABLE_PREDEFINED_ANGLE_UNITS

#include "third_party/units/units.h"
#include <chrono>  // NOLINT

using namespace std::chrono_literals;  // NOLINT
using namespace units::literals;  // NOLINT
