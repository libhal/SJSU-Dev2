#include "peripherals/lpc40xx/timer.hpp"
#include "utility/log.hpp"
#include "utility/time/time.hpp"
#include "utility/math/units.hpp"

// Put whatever functions you want to be triggered within these ISR's
// and you can use the timer to trigger whatever event you want.
void Timer0ISR()
{
  sjsu::LogInfo("Successfully triggered interrupt from timer 0");
}

void Timer1ISR()
{
  sjsu::LogInfo("Successfully triggered interrupt from timer 1");
}

void Timer2ISR()
{
  sjsu::LogInfo("Successfully triggered interrupt from timer 2");
}

void Timer3ISR()
{
  sjsu::LogInfo("Successfully triggered interrupt from timer 3");
}

// In this example all timers are initialized to 1 MHz, or 1 micro second per
// timer register count.
int main()
{
  sjsu::LogInfo("Timer Application Starting...");

  sjsu::lpc40xx::Timer timer0(sjsu::lpc40xx::Timer::Peripheral::kTimer0);
  sjsu::lpc40xx::Timer timer1(sjsu::lpc40xx::Timer::Peripheral::kTimer1);
  sjsu::lpc40xx::Timer timer2(sjsu::lpc40xx::Timer::Peripheral::kTimer2);
  sjsu::lpc40xx::Timer timer3(sjsu::lpc40xx::Timer::Peripheral::kTimer3);

  timer0.Initialize(1_MHz, Timer0ISR);
  timer1.Initialize(1_MHz, Timer1ISR);
  timer2.Initialize(1_MHz, Timer2ISR);
  timer3.Initialize(1_MHz, Timer3ISR);

  timer0.SetMatchBehavior(1'000'000,
                          sjsu::Timer::MatchAction::kInterruptRestart);
  timer1.SetMatchBehavior(3'000'000,
                          sjsu::Timer::MatchAction::kInterruptRestart);
  timer2.SetMatchBehavior(5'000'000,
                          sjsu::Timer::MatchAction::kInterruptRestart);
  timer3.SetMatchBehavior(10'000'000,
                          sjsu::Timer::MatchAction::kInterruptRestart);

  timer0.Start();
  timer1.Start();
  timer2.Start();
  timer3.Start();

  sjsu::Halt();
  return 0;
}
