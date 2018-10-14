// Digital-to-Analog Demoonstration Main, Designed so that
// The DAC driver can be demonstrated on an Oscilloscope.
#include "L0_LowLevel/delay.hpp"
#include "L0_LowLevel/LPC40xx.h"
#include "L1_Drivers/dac.hpp"
#include "L2_Utilities/debug_print.hpp"
// Input Number for starting demo

const uint16_t kSineLookup[256] = {
  500, 512, 525, 537,  549,  561,  573, 585, 598, 610, 621, 633, 645, 657, 668,
  680, 691, 703, 714,  725,  736,  746, 757, 767, 778, 788, 798, 808, 817, 827,
  836, 845, 854, 862,  870,  879,  887, 894, 902, 909, 916, 922, 929, 935, 941,
  947, 952, 957, 962,  966,  971,  975, 978, 982, 985, 988, 990, 993, 995, 996,
  998, 999, 999, 1000, 1000, 1000, 999, 999, 998, 996, 995, 993, 990, 988, 985,
  982, 978, 975, 971,  966,  962,  957, 952, 947, 941, 935, 929, 922, 916, 909,
  902, 894, 887, 879,  870,  862,  854, 845, 836, 827, 817, 808, 798, 788, 778,
  767, 757, 746, 736,  725,  714,  703, 691, 680, 668, 657, 645, 633, 621, 610,
  598, 585, 573, 561,  549,  537,  525, 512, 500, 488, 475, 463, 451, 439, 427,
  415, 402, 390, 379,  367,  355,  343, 332, 320, 309, 297, 286, 275, 264, 254,
  243, 233, 222, 212,  202,  192,  183, 173, 164, 155, 146, 138, 130, 121, 113,
  106, 98,  91,  84,   78,   71,   65,  59,  53,  48,  43,  38,  34,  29,  25,
  22,  18,  15,  12,   10,   7,    5,   4,   2,   1,   1,   0,   0,   0,   1,
  1,   2,   4,   5,    7,    10,   12,  15,  18,  22,  25,  29,  34,  38,  43,
  48,  53,  59,  65,   71,   78,   84,  91,  98,  106, 113, 121, 130, 138, 146,
  155, 164, 173, 183,  192,  202,  212, 222, 233, 243, 254, 264, 275, 286, 297,
  309, 320, 332, 343,  355,  367,  379, 390, 402, 415, 427, 439, 451, 463, 475,
  488
};

void SineDac(Dac * dac_demo, uint32_t delay)
{
  SJ2_ASSERT_FATAL(delay < 500,
                   "Input frequency out of range! Frequency "
                   "input needs to be between 0 and 500 Hz.");
  for (uint8_t i = 0; i < 255; i++)
  {
    dac_demo->WriteDac(kSineLookup[i]);
    Delay(delay);
  }
}
void SawtoothDac(Dac * dac_demo, uint32_t delay)
{
  SJ2_ASSERT_FATAL(delay < 500,
                   "Input delay out of range! Delay "
                   "input needs to be between 0 and 500 Hz.");
  int sawtooth = 0;
  for (uint8_t i = 0; i < 255; i++)
  {
    uint16_t conversion = static_cast<uint16_t>(sawtooth);
    dac_demo->WriteDac(conversion);
    sawtooth = sawtooth + 4;
    Delay(delay);
  }
}
void TriangleDac(Dac * dac_demo, uint32_t delay)
{
  SJ2_ASSERT_FATAL(delay < 500,
                   "Input delay out of range! Delay "
                   "input needs to be between 0 and 500 Hz.");
  int triangle = 0;
  for (uint8_t i = 0; i < 127; i++)
  {
    uint16_t conversion = static_cast<uint16_t>(triangle);
    dac_demo->WriteDac(conversion);
    triangle = triangle + 8;
    Delay(delay);
  }
  for (uint8_t i = 127; i < 255; i++)
  {
    uint16_t conversion = static_cast<uint16_t>(triangle);
    dac_demo->WriteDac(conversion);
    triangle = triangle - 8;
    Delay(delay);
  }
}
void SerratedDac(Dac * dac_demo, uint32_t delay)
{
  SJ2_ASSERT_FATAL(delay < 500,
                   "Input delay out of range! Delay "
                   "input needs to be between 0 and 500.");
  int serrated = 0;
  for (uint8_t i = 0; i < 127; i++)
  {
    uint16_t conversion = static_cast<uint16_t>(serrated);
    dac_demo->WriteDac(conversion);
    serrated = serrated + 8;
    Delay(delay);
  }
  serrated = 0;
  for (uint8_t i = 127; i < 253; i++)
  {
    uint16_t conversion = static_cast<uint16_t>(serrated);
    dac_demo->WriteDac(conversion);
    serrated = serrated + 4;
    Delay(delay);
  }
}
void (*dac_controllers[])(Dac *, uint32_t) = { SerratedDac, TriangleDac,
                                               SawtoothDac, SineDac };

void StartDemo(Dac * dac_demo, uint32_t input_cycles)
{
  for (uint32_t i = 0; i < 4; i++)
  {
    for (int j = 0; j < 30; j++)
    {
      dac_controllers[i](dac_demo, input_cycles);
    }
  }
}

int main(void)
{
  Dac dac;
  dac.Initialize();
  DEBUG_PRINT("Hook up pin p0.26 to an oscilloscope to test if it works!\n");
  DEBUG_PRINT("Starting Output of waves...\n");
  while (1)
  {
    StartDemo(&dac, 5);
  }
}
