// All
#pragma once
#include <cstdio>
#include "L2_Utilities/ansi_terminal_codes.hpp"
// Printf style logging with filename, function name, and line number
#define DEBUG_PRINT(format, ...)                                \
    printf(SJ2_HI_BLUE "%s:" SJ2_HI_GREEN "%s:" SJ2_HI_YELLOW   \
                       "%d> " SJ2_WHITE format SJ2_COLOR_RESET, \
           __FILE__, __PRETTY_FUNCTION__, __LINE__, ##__VA_ARGS__);
