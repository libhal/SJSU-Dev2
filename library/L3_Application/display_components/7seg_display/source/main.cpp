// #include "L3_Application/display_components/7seg_display/source/7_segment.hpp"
// #include "L3_Application/display_components/7seg_display/source/test_pixel_display.hpp"
// using namespace sjsu;
// int main()
// {
// //   sjsu::lpc40xx::Spi spi1(sjsu::lpc40xx::Spi::Bus::kSpi1);
// //   sjsu::lpc40xx::Gpio cs(1, 22);
// //   sjsu::lpc40xx::Gpio dc(1, 25);

// //   sjsu::Ssd1306 disp_test(spi1, cs, dc, sjsu::GetInactive<sjsu::Gpio>());
// //   disp_test.Initialize();
// //   sjsu::Delay(1000ms);
// //   sjsu::Graphics graph(disp_test);

// //   sjsu::SevenSegmentDisplay test(graph);
// //   test.Initialize();
// //   test.ClearValue();
// //   for(int i=0; i<100; i++){
// //     test.SetCharacter(i);
// //     test.Draw();
// //     sjsu::Delay(100ms);
// //     test.ClearValue();
// //   }
//   TestPixelDisplay t;
//   Graphics graphics(t);
//   SevenSegmentDisplay test_subject(graphics);

  
  
//   for(size_t i=0; i<t.pixel_test.size(); i++)
//   {
//       for(size_t j=0; j<t.pixel_test[0].size(); j++)
//       {
//           t.pixel_test[i][j] = 0;
//       }
//   }
//   test_subject.SetCharacter(8);
// //   for(size_t i=0; i<t.pixel_test[0].size(); i++)
// //   {
// //       printf("[");
// //       for(size_t j=0; j<t.pixel_test.size(); j++)
// //       {
// //           if(j+1 == t.pixel_test.size())
// //           {
// //               printf("%i", t.pixel_test[j][i]);
// //           }
// //           else
// //           {
// //               printf("%i,", t.pixel_test[j][i]);
// //           }
// //       }
// //       printf("]\n");
// //   }
//   return 0;
// }