#include "L3_Application/display_components/7seg_display/source/7_segment.hpp"
#include "L4_Testing/testing_frameworks.hpp"

namespace sjsu
{
    EMIT_ALL_METHODS(SevenSegmentDisplay);

    TEST_CASE("Seven segment display")
    {
        Mock<sjsu::lpc40xx::Gpio> mock_cs;
        Mock<sjsu::lpc40xx::Gpio> mock_dc;
        Mock<sjsu::lpc40xx::Gpio> mock_inactive;
        Mock<sjsu::lpc40xx::Spi> mock_spi1;

        sjsu::Ssd1306 display(mock_spi1.get(), mock_cs.get(), mock_dc.get(), mock_inactive.get());
        sjsu::Graphics graphics(display);

        SevenSegmentDisplay test_subject(graphics);
    }
}