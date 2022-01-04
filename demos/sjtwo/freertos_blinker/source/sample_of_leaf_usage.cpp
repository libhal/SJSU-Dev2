void ButtonReader([[maybe_unused]] void * parameters)
{
  sjsu::LogInfo("Setting up task...");
  sjsu::LogInfo("Initializing SW3...");
  auto combo_number = sjsu::rtos::RetrieveParameter(parameters);

  int press_count = 0;

  if (combo_number == 3)
  {
    sjsu::lpc40xx::Gpio & button_gpio3 = sjsu::lpc40xx::GetGpio<0, 29>();
    sjsu::Button switch3(button_gpio3);
    sjtwo::led3.Initialize();
    switch3.Initialize();

    sjtwo::led3.SetAsOutput();
    sjtwo::led3.SetLow();

    sjsu::LogInfo("SW3 Initialized...");
    sjsu::LogInfo("Press and release SW3 to toggle LED3 state...");

    // Loop detects when the button has been released and changes the LED state
    // accordingly.
    while (true)
    {
      boost::leaf::try_handle_all(
          [&switch3, &press_count]() -> boost::leaf::result<void>
          {
            if (switch3.Released())
            {
              sjtwo::led3.Toggle();
              return boost::leaf::new_error(press_count++);
            }
            return {};
          },
          [](int press_count_value)
          { sjsu::LogInfo("number of presses = %d", press_count_value); },
          []() {

          });

      vTaskDelay(50);
    }
  }
  else
  {
    sjsu::Button switch2(sjtwo::button2);
    sjtwo::led2.Initialize();
    switch2.Initialize();

    sjtwo::led2.SetAsOutput();
    sjtwo::led2.SetLow();

    sjsu::LogInfo("SW3 Initialized...");
    sjsu::LogInfo("Press and release SW3 to toggle LED2 state...");

    // Loop detects when the button has been released and changes the LED state
    // accordingly.
    while (true)
    {
      boost::leaf::try_handle_all(
          [&switch2, &press_count]() -> boost::leaf::result<void>
          {
            if (switch2.Released())
            {
              sjtwo::led2.Toggle();
              return boost::leaf::new_error(press_count++);
            }
            return {};
          },
          [](int press_count_value)
          { sjsu::LogInfo("number of presses = %d", press_count_value); },
          []() {

          });

      vTaskDelay(50);
    }
  }
}