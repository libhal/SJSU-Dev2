#include <cstdint>
#include <array>

#include "utility/log.hpp"

int main()
{
  LOG_INFO("STDIN Application Starting...\n");

  int number = 0;
  printf("Type in a number: ");
  scanf("%d", &number);
  printf("The number you typed was %d\n\n", number);

  LOG_INFO("End of program.");
  return 0;
}
