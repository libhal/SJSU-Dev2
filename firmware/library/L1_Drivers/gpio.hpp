#include <assert.h>
#include <stdint.h>
#include "L0_LowLevel/LPC40xx.h"

class GPIO{
  private:
    const uint8_t portNum;
    const uint8_t pinNum;

    //Used to point to a certain port located in LPC memory map
    constexpr static LPC_GPIO_TypeDef *gpioBase[6] = {LPC_GPIO0, LPC_GPIO1,
      LPC_GPIO2, LPC_GPIO3, LPC_GPIO4, LPC_GPIO5};

  public:

    //For port 0-4, pins 0-31 are available
    //Port 5 only have pins 0-4 available
    GPIO(uint8_t port, uint8_t pin) :
      portNum(port), pinNum(pin)
    {
      if(portNum > 5)
      {
        assert(false);
      }

      if(((portNum == 5) && (pinNum > 4)) || ((portNum != 5) && (pinNum > 31)))
      {
        assert(false);
      }
    };
    ~GPIO() {};

    //Set direction of GPIO pin
    inline void setAsInput(void) { gpioBase[portNum]->DIR &= ~(1 << pinNum); };
    inline void setAsOutput(void) { gpioBase[portNum]->DIR |= (1 << pinNum); };
    inline void setDir(bool input = true) { (input) ? setAsInput() : setAsOutput(); };

    //Output control for GPIO pin
    inline void ctrlOutputHigh(void) { gpioBase[portNum]->SET = (1 << pinNum); };
    inline void ctrlOutputLow(void) { gpioBase[portNum]->CLR = (1 << pinNum); };
    inline void ctrlOutput(bool high = true) { (high) ? ctrlOutputHigh() : ctrlOutputLow(); };

    //Returns true if input or output is high
    inline bool readPin(void) { return ((gpioBase[portNum]->PIN >> pinNum) & 1); };
};
