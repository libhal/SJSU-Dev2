#include <cctype>
#include <cstdint>
#include <cstdio>
#include <cstdlib>

#include "newlib/newlib.hpp"

namespace debug
{
void PrintCharacterRow(uint8_t * bytes, size_t length)
{
    printf("|");
    for (size_t j = 0; j < length; j++)
    {
        if (isprint(bytes[j]))
        {
            printf("%c", bytes[j]);
        }
        else
        {
            printf(".");
        }
    }
    printf("|\n");
}

void PrintHexBytesRow(uint8_t * bytes, size_t length)
{
    for (size_t j = 0; j < 16; j++)
    {
        if (j < length)
        {
            printf("%02X ", bytes[j]);
        }
        else
        {
            printf("   ");
        }
        if (j == 7)
        {
            printf(" ");
        }
    }
    printf(" ");
}

void Hexdump(void * address, size_t length)
{
    uint8_t * bytes = static_cast<uint8_t *>(address);
    size_t i        = 0;
    for (i = 0; i < length; i += 16)
    {
        printf("%08zX  ", i);
        size_t bytes_to_print = (i + 15 > length) ? (i + 15) - length : 16;
        PrintHexBytesRow(&bytes[i], bytes_to_print);
        PrintCharacterRow(&bytes[i], bytes_to_print);
    }
    printf("%08zX  \n", length);
}

}  // namespace debug
