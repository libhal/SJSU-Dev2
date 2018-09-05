#include "L0_LowLevel/uart0.hpp"

using Stdout = int (*)(int);
extern Stdout out;

using Stdin = int (*)();
extern Stdin in;

// NOLINTNEXTLINE(readability-identifier-naming)
extern "C" int _write(int file, char * ptr, int length);
