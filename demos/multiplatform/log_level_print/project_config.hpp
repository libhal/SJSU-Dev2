// This file overrides the default configuration options in the
// library/config.hpp file. Open library/config.hpp to see which configuration
// options you can change.
#pragma once
#include "log_levels.hpp"

// Be sure to enable ANSI color for the terminal. Disable this if your terminal
// application does not support colored text.
#define SJ2_ENABLE_ANSI_CODES true
// Setting this to NONESET to allow types of log level message to appear
#define SJ2_LOG_LEVEL SJ2_LOG_LEVEL_NONESET

#include "config.hpp"
