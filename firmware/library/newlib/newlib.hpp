#pragma once

#include <cstring>

#include "utility/macros.hpp"

using Stdout = int (*)(int);
extern Stdout out;

using Stdin = int (*)();
extern Stdin in;
