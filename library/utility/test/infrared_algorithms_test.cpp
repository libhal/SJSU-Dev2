#include "testing/testing_frameworks.hpp"
#include "utility/infrared_algorithms.hpp"

namespace sjsu
{
namespace infrared
{
TEST_CASE("Testing Infrared Algorithms")
{
  SECTION("IsDurationWithinTolerance")
  {
    struct TestInput_t
    {
      /// Duration in microseconds.
      uint16_t duration;
      std::chrono::microseconds expected_duration;
      float tolerance;

      bool expect_within_tolerance;
    };

    constexpr std::array kTestInputs = {
      TestInput_t{
          .duration                = 97,
          .expected_duration       = 100us,
          .tolerance               = 0.05,
          .expect_within_tolerance = true,
      },
      TestInput_t{
          .duration                = 103,
          .expected_duration       = 100us,
          .tolerance               = 0.05,
          .expect_within_tolerance = true,
      },
      TestInput_t{
          .duration                = 995,
          .expected_duration       = 10us,
          .tolerance               = 0.1,
          .expect_within_tolerance = false,
      },
      TestInput_t{
          .duration                = 1095,
          .expected_duration       = 10us,
          .tolerance               = 0.1,
          .expect_within_tolerance = false,
      },
    };

    for (size_t i = 0; i < kTestInputs.size(); i++)
    {
      // Setup
      auto duration          = kTestInputs[i].duration;
      auto expected_duration = kTestInputs[i].expected_duration;
      auto tolerance         = kTestInputs[i].tolerance;

      INFO("index: " << i);
      INFO("duration: " << kTestInputs[i].duration);
      INFO("expected_duration: " << kTestInputs[i].expected_duration.count());
      INFO("tolerance: " << kTestInputs[i].tolerance);

      // Exercise
      auto result =
          IsDurationWithinTolerance(duration, expected_duration, tolerance);

      // Verify
      CHECK(result == kTestInputs[i].expect_within_tolerance);
    }
  }

  SECTION("Decode")
  {
    PulseDurationConfiguration_t pulse_config;
    DataFrame_t frame;

    SECTION("Valid Frame")
    {
      pulse_config.header_mark_duration  = 10000us;
      pulse_config.header_space_duration = 5000us;
      pulse_config.data_duration         = 1000us;
      pulse_config.logic_high_duration   = 500us;
      pulse_config.logic_low_duration    = 500us;
      pulse_config.tolerance             = 0.1;

      // Using length of 4 to only test the head mark, header space, first
      // data mark, and first data space.
      frame.pulse_buffer_length = 4;
      frame.pulse_buffer[0]     = 9990;
      frame.pulse_buffer[1]     = 5010;

      SECTION("PulseDurationType = kDistance")
      {
        pulse_config.encoding_type = PulseDurationType::kDistance;
        frame.pulse_buffer[2]      = 1010;
        frame.pulse_buffer[3]      = 510;
      }

      SECTION("PulseDurationType = kLength")
      {
        pulse_config.encoding_type = PulseDurationType::kLength;
        frame.pulse_buffer[2]      = 499;
        frame.pulse_buffer[3]      = 910;
      }

      // Exercise & Verify
      CHECK(Decode(&frame, pulse_config).is_valid == true);
    }

    SECTION("Invalid Frame")
    {
      SECTION("When frame is too short")
      {
        // Setup
        frame.pulse_buffer_length = 2;

        // Exercise & Verify
        CHECK(Decode(&frame, pulse_config).is_valid == false);
      }

      SECTION("When header mark is not valid")
      {
        // Setup
        pulse_config.header_mark_duration = 10us;
        pulse_config.tolerance            = 0.1;

        frame.pulse_buffer_length = 4;
        frame.pulse_buffer[0]     = 900;

        // Exercise & Verify
        CHECK(Decode(&frame, pulse_config).is_valid == false);
      }

      SECTION("When header space is not valid")
      {
        // Setup
        pulse_config.header_mark_duration = 100us;
        pulse_config.tolerance            = 0.1;

        frame.pulse_buffer_length = 4;
        frame.pulse_buffer[0]     = 100;
        frame.pulse_buffer[1]     = 800;

        // Exercise & Verify
        CHECK(Decode(&frame, pulse_config).is_valid == false);
      }

      SECTION("When data is not valid")
      {
        // Setup
        pulse_config.header_mark_duration  = 10000us;
        pulse_config.header_space_duration = 5000us;
        pulse_config.data_duration         = 100us;
        pulse_config.tolerance             = 0.1;

        // Using length of 4 to only test the head mark, header space, first
        // data mark, and first data space.
        frame.pulse_buffer_length = 4;
        frame.pulse_buffer[0]     = 10000;
        frame.pulse_buffer[1]     = 5000;

        SECTION("Data duration is not valid")
        {
          SECTION("PulseDurationType = kDistance")
          {
            pulse_config.encoding_type = PulseDurationType::kDistance;
            frame.pulse_buffer[2]      = 300;
          }

          SECTION("PulseDurationType = kLength")
          {
            pulse_config.encoding_type = PulseDurationType::kLength;
            frame.pulse_buffer[3]      = 300;
          }
        }

        SECTION("Logic level duration is not valid")
        {
          pulse_config.logic_high_duration = 500us;
          pulse_config.logic_low_duration  = 500us;

          SECTION("PulseDurationType = kDistance")
          {
            pulse_config.encoding_type = PulseDurationType::kDistance;
            frame.pulse_buffer[2]      = 100;
            frame.pulse_buffer[3]      = 300;
          }

          SECTION("PulseDurationType = kLength")
          {
            pulse_config.encoding_type = PulseDurationType::kLength;
            frame.pulse_buffer[2]      = 300;
            frame.pulse_buffer[3]      = 100;
          }
        }

        // Exercise & Verify
        CHECK(Decode(&frame, pulse_config).is_valid == false);
      }
    }
  }
}
}  // namespace infrared
}  // namespace sjsu
