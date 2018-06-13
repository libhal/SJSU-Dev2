#include <stdio.h>

int main(void)
{
    LPC_IOCON->P1_1 &= ~(0b111);
    LPC_GPIO1->DIR |=  (1 << 1);
    LPC_GPIO1->PIN &= ~(1 << 1);
    
    while(1)
    {
        int k = 0;
        for(int i = 0; i < 12'000'000; i++)
        {
            k++;            
        }
        LPC_GPIO1->PIN ^= (1 << 1);
    }
    
    return 0;
}