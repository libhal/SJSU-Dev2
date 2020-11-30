#include "utility/log.hpp"

int main()
{
  sjsu::LogInfo("STDIN Application Starting...\n");

  int number = 0;
  printf("Type in a number: ");
  scanf("%d", &number);
  printf("\n--> You typed number: %d\n\n", number);

  return 0;
}
