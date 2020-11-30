// This project configuration disables as many features  or minimizes the memory
#pragma once

#define SJ2_LOG_LEVEL SJ2_LOG_LEVEL_ERROR
#define SJ2_PRINTF_SUPPORT_FLOAT false
#define SJ2_PRINTF_SUPPORT_PRECISION_FLOAT false
#define SJ2_PRINTF_SUPPORT_LONG_LONG false
#define SJ2_PRINTF_SUPPORT_PTRDIFF_T false
#define SJ2_INCLUDE_BACKTRACE false
#define SJ2_ENABLE_ANSI_CODES false
#define SJ2_INCLUDE_VSNPRINTF false
#define SJ2_STORE_ERROR_CODE_STRING false

#include "config.hpp"
