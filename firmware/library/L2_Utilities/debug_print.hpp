#include <cstdio>

// Printf style logging with filename, function name, and line number
#define DEBUG_PRINT(format, ...)                                        \
    printf("%s:%s:%d> " format, __FILE__, __PRETTY_FUNCTION__, __LINE__, \
           ##__VA_ARGS__);
