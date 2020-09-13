# Seven Segment Display on OLED

# Location
L3_Application
# Type
Implementation
# Background
The `SevenSegmentDisplay` class implements a retro 7-segment display on the built-in SSD1306 OLED.
# Overview
The `SevenSegmentDisplay` class is responsible for enabling the user to draw numbers 0 - 99 onto the OLED.
It can also let the user clear the value display on the OLED at will.
# Detailed Design
## API
```c++
class SevenSegmentDisplay
{
    public:
      SevenSegmentDisplay(sjsu::Graphics &graphics)
      : graphics_(graphics)
      {

      }

      /// Takes a character input and draws it to a 
      /// display in 7-segment form.
      ///
      /// @param num - character to be drawn to 7-segment
      void SetCharacter(uint8_t character)
      {
        if(character < 10)
        {
          for(uint8_t i = 0; i < 8; i++)
          {
            if(bipmap_array[character] & (1 << i))
            {
              if(i == 0 || i == 3 || i == 6)
              {
                DrawHorizontalSegment(segment_references_right[i][0], segment_references_right[i][1]);
              }
              else
              {
                DrawVerticalSegment(segment_references_right[i][0], segment_references_right[i][1]);
              }
            }
          }
        }
        if(character >= 10)
        {
          uint8_t first  = 0;
          uint8_t second = 0;
          if(character == 10)
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
            if(bipmap_array[first] & (1 << i))
            {
              if(i == 0 || i == 3 || i == 6)
              {
                DrawHorizontalSegment(segment_references_left[i][0], segment_references_left[i][1]);
              }
              else
              {
                DrawVerticalSegment(segment_references_left[i][0], segment_references_left[i][1]);
              }
            }
            if(bipmap_array[second] & (1 << i))
            {
              if(i == 0 || i == 3 || i == 6)
              {
                DrawHorizontalSegment(segment_references_right[i][0], segment_references_right[i][1]);
              }
              else
              {
                DrawVerticalSegment(segment_references_right[i][0], segment_references_right[i][1]);
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
        for(int i=0; i<22; i++){
            if(i==0 || i==21){
              graphics_.DrawPixel(x+i, y);
            }
            else if(i==1 || i==20){
              graphics_.DrawPixel(x+i, y-1);
              graphics_.DrawPixel(x+i, y);
              graphics_.DrawPixel(x+i, y+1);
            } 
            else if(i>=2 && i<=19){
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
        for(int i=0; i<22; i++){
            if(i==0 || i==21){
              graphics_.DrawPixel(x,   y+i);
            }
            else if(i==1 || i==20){
              graphics_.DrawPixel(x-1, y+i);
              graphics_.DrawPixel(x,   y+i);
              graphics_.DrawPixel(x+1, y+i);
            } 
            else if(i>=2 && i<=19){
              graphics_.DrawPixel(x-2, y+i);
              graphics_.DrawPixel(x-1, y+i);
              graphics_.DrawPixel(x,   y+i);
              graphics_.DrawPixel(x+1, y+i);
              graphics_.DrawPixel(x+2, y+i);
            }
          }
      }
      
      ///reference points for segment beginnings for the left 7-segment
      inline static const uint8_t segment_references_left[8][2] =
      {
        {21, 7},   //segment 0, horizontal
        {44, 9},   //segment 1, vertical
        {44, 34},  //segment 2, vertical
        {21, 57},  //segment 3, horizontal
        {19, 34},  //segment 4, vertical
        {19, 9},   //segment 5, vertical
        {21, 32},  //segment 6, horizontal
        {55, 54}   //period following after
      };
      ///reference points for segment beginnings for the right 7-segment
      inline static const uint8_t segment_references_right[8][2] =
      {
        {86, 7},   //segment 0, horizontal
        {109, 9},  //segment 1, vertical
        {109, 34}, //segment 2, vertical
        {86, 57},  //segment 3, horizontal
        {84, 34},  //segment 4, vertical
        {84, 9},   //segment 5, vertical
        {86, 32},  //segment 6, horizontal
        {120, 54}  //period following after
      };
      ///bitmap array for numbers 0 - 9
      static constexpr uint8_t bipmap_array[10] = 
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
      ///reference points for middle circles on 7-segment
      // static constexpr uint8_t middle_circles[2][2] = 
      // {
      //   {64, 19},
      //   {64, 44}
      // };

};
```
The main highlight of this class is the `SetCharacter` method. This method allows the user to pass in numbers 0 - 99
to be display on the OLED. It takes in a uint8_t, determines if the number is greater than or equal to 10, or if the 
number is less than 10. Based on its determination, it will use a bitmap named `bitmap_array` that uses `uint8_t`s
to represent set or cleared bits of the parameter `character`. By checking if a bit is set by looping through the
bits, the method is able to draw the correct segment for a given number. To draw this segment, 2 helper functions
named `DrawHorizontalSegment` and `DrawVerticalSegment` are responsible for procedurally drawing pixels to the OLED.
Since some segments are horizontal and others are vertical, `segment_references_left` and `segment_references_right`
map the starting `(x, y)` coordinates of each segment. `DrawHorizontalSegment` and `DrawVerticalSegment` use these
reference points to begin drawing pixels. The pixel drawing itself is handled by the `DrawPixel` method from the 
`Graphics` interface.
# Caveats
Due to the nature of the reference points and bitmap, the space they consume is required by the class to function
correctly.
# Future Advancements
In the future, I plan on figuring out a way to scale the `SevenSegmentDisplay` smaller or larger depending on the need.
Additionally, it would be great if the user had the ability to move the 7-segments to a different location to be drawn.
Due to not having a larger screen to test this functionality, this feature will be put on hold.
# Testing plan

TBD.
