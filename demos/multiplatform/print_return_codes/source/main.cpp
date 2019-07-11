#include "utility/log.hpp"

int main()
{
  LOG_INFO("Print Return Codes Starting... And then immediately end!");
  // If the returned value is greater than or equal to 0, text is printed green
  // Otherwise, printed text is red.
  //
  // Change the following return value to observe this.
  return -1;
}
