#include "pwm.hpp"

volatile uint32_t * Pwm::match[7] = { &pwm1->MR0, &pwm1->MR1, &pwm1->MR2,
                                      &pwm1->MR3, &pwm1->MR4, &pwm1->MR5,
                                      &pwm1->MR6 };

LPC_PWM_TypeDef * Pwm::pwm1 = LPC_PWM1;
LPC_SC_TypeDef * Pwm::sc    = LPC_SC;
