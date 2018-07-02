#include <cstdio>

#define DEBUG_PRINT(str, ...) printf("%s:%s:%d) %s\n", \
     __FILE__, __PRETTY_FUNCTION__, __LINE__, str, ##__VA_ARGS__);