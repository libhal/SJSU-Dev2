// Digital-to-Analog Demoonstration Main, Designed so that
// The DAC driver can be demonstrated on an Oscilloscope.
#include "L1_Peripheral/lpc40xx/dac.hpp"
#include "utility/log.hpp"
#include "utility/time.hpp"
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

void Sine(sjsu::Dac & dac, std::chrono::nanoseconds delay)
{
  for (uint8_t i = 0; i < 255; i++)
  {
    dac.Write(kSineLookup[i]);
    sjsu::Delay(delay);
  }
}

void Sawtooth(sjsu::Dac & dac, std::chrono::nanoseconds delay)
{
  int sawtooth = 0;
  for (uint8_t i = 0; i < 255; i++)
  {
    uint16_t conversion = static_cast<uint16_t>(sawtooth);
    dac.Write(conversion);
    sawtooth = sawtooth + 4;
    sjsu::Delay(delay);
  }
}

void Triangle(sjsu::Dac & dac, std::chrono::nanoseconds delay)
{
  int triangle = 0;
  for (uint8_t i = 0; i < 127; i++)
  {
    uint16_t conversion = static_cast<uint16_t>(triangle);
    dac.Write(conversion);
    triangle = triangle + 8;
    sjsu::Delay(delay);
  }

  for (uint8_t i = 127; i < 255; i++)
  {
    uint16_t conversion = static_cast<uint16_t>(triangle);
    dac.Write(conversion);
    triangle = triangle - 8;
    sjsu::Delay(delay);
  }
}

void Serrated(sjsu::Dac & dac, std::chrono::nanoseconds delay)
{
  int serrated = 0;

  for (uint8_t i = 0; i < 127; i++)
  {
    uint16_t conversion = static_cast<uint16_t>(serrated);
    dac.Write(conversion);
    serrated = serrated + 8;
    sjsu::Delay(delay);
  }

  serrated = 0;

  for (uint8_t i = 127; i < 253; i++)
  {
    uint16_t conversion = static_cast<uint16_t>(serrated);
    dac.Write(conversion);
    serrated = serrated + 4;
    sjsu::Delay(delay);
  }
}

void StartDemo(sjsu::Dac & dac, std::chrono::nanoseconds input_cycles)
{
  static constexpr int kCycleCount = 1000;

  sjsu::LogInfo("Serrated...");
  for (int i = 0; i < kCycleCount; i++)
  {
    Serrated(dac, input_cycles);
  }

  sjsu::LogInfo("Triangle...");
  for (int i = 0; i < kCycleCount; i++)
  {
    Triangle(dac, input_cycles);
  }

  sjsu::LogInfo("Sawtooth...");
  for (int i = 0; i < kCycleCount; i++)
  {
    Sawtooth(dac, input_cycles);
  }

  sjsu::LogInfo("Sine...");
  for (int i = 0; i < kCycleCount; i++)
  {
    Sine(dac, input_cycles);
  }
}

int main()
{
  sjsu::lpc40xx::Dac dac;

  dac.Initialize();
  dac.Enable();

  sjsu::LogInfo("Hook up pin p0.26 to an oscilloscope to test if it works!");
  sjsu::LogInfo("Starting Output of waves...");

  StartDemo(dac, 10us);

  sjsu::LogInfo("Demo exiting. To restart, reset device. ...");
  return 0;
}
