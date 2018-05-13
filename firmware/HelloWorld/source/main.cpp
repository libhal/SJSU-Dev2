#include <stdio.h>
#include "L0-LowLevel/uart0.min.h"

int main(void)
{
    char buffer[128];
    sprintf(buffer, "Hello, World %f\n", static_cast<double>(5.2));
    puts(buffer);
    return 0;
}