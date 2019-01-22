// Hardware Timer Driver example that provides example of
// how to use the hardware timer driver.
#include "L0_LowLevel/interrupt.hpp"
#include "L0_LowLevel/LPC40xx.h"
#include "L1_Drivers/timer.hpp"
#include "utility/log.hpp"
#include "utility/time.hpp"

Timer timer0(TimerInterface::kTimer0);
Timer timer1(TimerInterface::kTimer1);
Timer timer2(TimerInterface::kTimer2);
Timer timer3(TimerInterface::kTimer3);

// Put whatever functions you want to be triggered within these ISR's
// and you can use the timer to trigger whatever event you want.
void Timer0ISR(void)
{
  printf("Successfully triggered interrupt from timer 0\n\n");
  timer0.ClearInterrupts();
}
void Timer1ISR(void)
{
  printf("Successfully triggered interrupt from timer 1.\n\n");
  timer1.ClearInterrupts();
}
void Timer2ISR(void)
{
  printf("Successfully triggered interrupt from timer 2\n\n");
  timer2.ClearInterrupts();
}
void Timer3ISR(void)
{
  printf("Successfully triggered interrupt from timer 3\n\n");
  timer3.ClearInterrupts();
}

void StartDemo(Timer * timer_demo0, Timer * timer_demo1, Timer * timer_demo2,
               Timer * timer_demo3)
{
  timer_demo0->SetTimer(1000000, Timer::kMat0, Timer::kInterruptRestart);
  timer_demo1->SetTimer(2000000, Timer::kMat1, Timer::kInterruptRestart);
  timer_demo2->SetTimer(3000000, Timer::kMat2, Timer::kInterruptRestart);
  timer_demo3->SetTimer(4000000, Timer::kMat3, Timer::kInterruptRestart);
}

// In this example all timers are initilialized to 1 micro second per tick
int main(void)
{
  timer0.Initialize(Timer::TimerPort::kTimer0, 1, Timer0ISR, 1);
  timer1.Initialize(Timer::TimerPort::kTimer1, 1, Timer1ISR, 1);
  timer2.Initialize(Timer::TimerPort::kTimer2, 1, Timer2ISR, 1);
  timer3.Initialize(Timer::TimerPort::kTimer3, 1, Timer3ISR, 1);

  LOG_INFO("\n");
  LOG_INFO("Start Timer Demo\n");
  while (true)
  {
    StartDemo(&timer0, &timer1, &timer2, &timer3);
  }
}
