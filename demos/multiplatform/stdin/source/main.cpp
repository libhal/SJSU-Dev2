#include <cstdint>
#include <array>

#include "utility/log.hpp"

int main()
{
  sjsu::LogInfo("STDIN Application Starting...\n");

  int number = 0;
  printf("Type in a number: ");
  scanf("%d", &number);
  printf("The number you typed was %d\n\n", number);

  sjsu::LogInfo("End of program.");
  return 0;
}
