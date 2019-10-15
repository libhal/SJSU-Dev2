#include <inttypes.h>
#include <cstdint>
#include "utility/log.hpp"
#include "utility/time.hpp"

int main()
{
  LOG_INFO("STDIN Application Starting...");

  LOG_INFO(
      "This project demonstrates the use of scanf with an embedded processor.");

  int number = 0;
  printf("Type in a number: ");
  scanf("%d", &number);
  printf("The number you typed was %d\n", number);

  LOG_INFO("End of program.");
  return 0;
}
