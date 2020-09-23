#include "L1_Peripheral/lpc40xx/gpio.hpp"
#include "L1_Peripheral/lpc40xx/spi.hpp"
#include "L2_HAL/displays/oled/ssd1306.hpp"
#include "L3_Application/graphics.hpp"

namespace sjsu
{
class SevenSegmentDisplay
{
     public:
      explicit SevenSegmentDisplay(sjsu::Graphics &graphics)
      : graphics_(graphics)
      {
      }
      /// Takes a character input and draws it to a
      /// display in 7-segment form.
      ///
      /// @param num - character to be drawn to 7-segment
      void SetCharacter(uint8_t character)
      {
        if (character < 10)
        {
          for(uint8_t i = 0; i < 8; i++)
          {
            if (kBitmap[character] & (1 << i))
            {
              if (i == 0 || i == 3 || i == 6)
              {
                DrawHorizontalSegment(kSegmentReferencesRight[i][0], kSegmentReferencesRight[i][1]);
              }
              else
              {
                DrawVerticalSegment(kSegmentReferencesRight[i][0], kSegmentReferencesRight[i][1]);
              }
            }
          }
        }
        if (character >= 10)
        {
          uint8_t first  = 0;
          uint8_t second = 0;
          if (character == 10)
          {
            first = 1;
            second = 0;
          }
          else
          {
            second = character%10;
            first = (character-first)/10;
          }
          for(uint8_t i = 0; i < 8; i++)
          {
            if (kBitmap[first] & (1 << i))
            {
              if (i == 0 || i == 3 || i == 6)
              {
                DrawHorizontalSegment(kSegmentReferencesLeft[i][0], kSegmentReferencesLeft[i][1]);
              }
              else
              {
                DrawVerticalSegment(kSegmentReferencesLeft[i][0], kSegmentReferencesLeft[i][1]);
              }
            }
            if (kBitmap[second] & (1 << i))
            {
              if (i == 0 || i == 3 || i == 6)
              {
                DrawHorizontalSegment(kSegmentReferencesRight[i][0], kSegmentReferencesRight[i][1]);
              }
              else
              {
                DrawVerticalSegment(kSegmentReferencesRight[i][0], kSegmentReferencesRight[i][1]);
              }
            }
          }
        }
      }

      /// Initializes the 7-segment display.
      void Initialize()
      {
        graphics_.Initialize();
        sjsu::Delay(1000ms);
      }

      /// Clears the currently displayed value
      void ClearValue()
      {
        graphics_.Clear();
      }

      /// Draws new changes made to the display.
      void Draw()
      {
        graphics_.Update();
      }
     private:
      sjsu::Graphics &graphics_;

      void DrawHorizontalSegment(uint8_t x, uint8_t y)
      {
        for(int i = 0; i < 22; i++){
            if (i == 0 || i == 21){
              graphics_.DrawPixel(x+i, y);
            }
            else if (i == 1 || i == 20){
              graphics_.DrawPixel(x+i, y-1);
              graphics_.DrawPixel(x+i, y);
              graphics_.DrawPixel(x+i, y+1);
            } 
            else if (i >= 2 && i <= 19){
              graphics_.DrawPixel(x+i, y-2);
              graphics_.DrawPixel(x+i, y-1);
              graphics_.DrawPixel(x+i, y);
              graphics_.DrawPixel(x+i, y+1);
              graphics_.DrawPixel(x+i, y+2);
            }
          }
      }

      void DrawVerticalSegment(uint8_t x, uint8_t y)
      {
        for(int i = 0; i < 22; i++){
            if (i == 0 || i == 21){
              graphics_.DrawPixel(x,   y+i);
            }
            else if (i == 1 || i == 20){
              graphics_.DrawPixel(x-1, y+i);
              graphics_.DrawPixel(x,   y+i);
              graphics_.DrawPixel(x+1, y+i);
            } 
            else if (i >= 2 && i <= 19){
              graphics_.DrawPixel(x-2, y+i);
              graphics_.DrawPixel(x-1, y+i);
              graphics_.DrawPixel(x,   y+i);
              graphics_.DrawPixel(x+1, y+i);
              graphics_.DrawPixel(x+2, y+i);
            }
          }
      }
      /// reference points for segment beginnings for the left 7-segment
      inline static const uint8_t kSegmentReferencesLeft[8][2] =
      {
        {21, 7},   // segment 0, horizontal
        {44, 9},   // segment 1, vertical
        {44, 34},  // segment 2, vertical
        {21, 57},  // segment 3, horizontal
        {19, 34},  // segment 4, vertical
        {19, 9},   // segment 5, vertical
        {21, 32},  // segment 6, horizontal
        {55, 54}   // period following after
      };
      /// reference points for segment beginnings for the right 7-segment
      inline static const uint8_t kSegmentReferencesRight[8][2] =
      {
        {86, 7},    // segment 0, horizontal
        {109, 9},   // segment 1, vertical
        {109, 34},  // segment 2, vertical
        {86, 57},   // segment 3, horizontal
        {84, 34},   // segment 4, vertical
        {84, 9},    // segment 5, vertical
        {86, 32},   // segment 6, horizontal
        {120, 54}   // period following after
      };
      /// kBitmap array for numbers 0 - 9
      static constexpr uint8_t kBitmap[10] =
      {
        0b00111111,
        0b00000110,
        0b01011011,
        0b01001111,
        0b01100110,
        0b01101101,
        0b01111101,
        0b00000111,
        0b01111111,
        0b01100111
      };
      /// reference points for middle circles on 7-segment
      // static constexpr uint8_t middle_circles[2][2] =
      // {
      //   {64, 19},
      //   {64, 44}
      // };
};
} //namespace sjsu
