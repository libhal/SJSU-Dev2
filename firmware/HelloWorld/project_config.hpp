// This file overrides the default configuratoins in the library/config.hpp file
//
// Check out the library/config.hpp file to see which configuration options
// you can change.
#pragma once
#include "L2_Utilities/macros.hpp"

#if !defined(SJ2_CONFIG_FILE) || SJ2_CONFIG_FILE == false
#pragma GCC error \
"Do not include this file directly. Include 'config.hpp' instead."
#endif // !defined(SJ2_CONFIG_FILE)

#define SJ2_ENABLE_ANSI_CODES true

#define SJ2_SYSTEM_CLOCK 48'000'000
