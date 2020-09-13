#include "L3_Application/display_components/7seg_display/source/7_segment.hpp"

using namespace sjsu;
int main()
{
  sjsu::lpc40xx::Spi spi1(sjsu::lpc40xx::Spi::Bus::kSpi1);
  sjsu::lpc40xx::Gpio cs(1, 22);
  sjsu::lpc40xx::Gpio dc(1, 25);

  sjsu::Ssd1306 disp_test(spi1, cs, dc, sjsu::GetInactive<sjsu::Gpio>());
  disp_test.Initialize();
  sjsu::Delay(1000ms);
  sjsu::Graphics graph(disp_test);

  sjsu::SevenSegmentDisplay test(graph);
  test.Initialize();
  test.ClearValue();
  for(int i=0; i<100; i++){
    test.SetCharacter(i);
    test.Draw();
    sjsu::Delay(100ms);
    test.ClearValue();
  }
  return 0;
}