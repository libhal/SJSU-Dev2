#include "utility/status.hpp"
#include "utility/log.hpp"

namespace
{
using sjsu::Returns;
using sjsu::Status;
}  // namespace

// Example code that will return an error if invalid input is given.
Returns<int> CalculatePllCode(int frequency)
{
  int code = 0;

  switch (frequency)
  {
    case 1: code = 0b001; break;
    case 2: code = 0b100; break;
    case 4: code = 0b011; break;
    case 8: code = 0b111; break;
    case 16: code = 0b101; break;
    case 32: code = 0b010; break;
    default:
      // When creating an error give it a reasonable status and a helpful
      // message to the user.
      return Error(Status::kInvalidParameters,
                   "Frequency must be 1, 2, 4, 8, 16, 32 megahertz.");
  }

  return code * 2;
}

// Just a demonstration, even though this never returns an error.
// Refrain from using `Returns<T>` objects if the function never
// returns an error
Returns<const char *> GetName()
{
  return "this is a test string to demonstrate nesting!";
}

// A function that returns a void type. In this case you simply need to return
// an empty object by doing this `return {}`.
Returns<void> SetPllCode(int code)
{
  // Dummy example of setting a register to the correct code
  int reg = code;

  // this code doesn't do anyting meaningful, but will fail if this code was
  // detected.
  if (reg == (0b101 * 2))
  {
    return Error(Status::kTimedOut, "CPU speed could not stabilize in time.");
  }

  return {};
}

// This function will attempt to set the cpu of a system, but will return
// early with an error if it detects at any stage there is one. The idea
// here is that this function does not concern itself with handlling the
// error, but simply doing its job and reporting problems if they occur. Now
// its up to the calling function to decide what to do at that stage.
Returns<int> GetPllCodeForFrequency(int frequency)
{
  // Will return early if there is an error resulting from the
  // CalculatePllCode() function call.
  int pll_code = SJ2_RETURN_ON_ERROR(CalculatePllCode(frequency));

  // Give result to another function and verify that it complets
  SJ2_RETURN_ON_ERROR(SetPllCode(pll_code));

  // Can be nested within other statements and still return early if
  // the function returns an error.
  sjsu::LogDebug("Nested result = %s", SJ2_RETURN_ON_ERROR(GetName()));
  sjsu::LogDebug("Successfully Got Pll Code!");

  return pll_code;
}

int main()
{
  // Run a function that could possibly return an error
  // NOTE: You cannot use `SJ2_RETURN_ON_ERROR()` macro here because `main()`
  // has an `int` return type but not a `Returns<int>` object, thus using it
  // here will generate an error.

  // Change the input to 4, to see this work without any errors.
  // Change the input to 5, to see this application return an error.
  // Change the input to 16, to see the void function fail.
  auto result = GetPllCodeForFrequency(5);

  // Check if the result has an error
  if (!result)
  {
    sjsu::LogError("Getting PLL Code Failed!");
    result.error()->Print();
    // Error handling would go here...
    return 111;
  }

  sjsu::LogInfo("Success! Final PLL Code = %d\n", result.value());
  return 0;
}
