#include <stdio.h>
#include "L0-LowLevel/uart0.min.hpp"

int main(void)
{
    uart0_puts("Hello, World\n");
    return 0;
}