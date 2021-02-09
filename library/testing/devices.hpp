#pragma once

#include <string>

#include "testing/testing_frameworks.hpp"
#include "utility/debug.hpp"

namespace sjsu
{
template <class ModuleInterface>
class AutoVerifyModule
{
 public:
  using Settings_t = ModuleInterface::SettingsType_t;

  AutoVerifyModule(const char * name, Settings_t expected_settings)
      : name_(name), kExpectedSettings(expected_settings)
  {
    // Make a default constructed settings object
    const Settings_t kDefaultSettings = {};

    // Save the defaulted constructed settings into the mock object's settings.
    mock_pin.get().settings = kDefaultSettings;
  }

  auto & GetPin()
  {
    return mock_pin.get();
  }

  auto & GetMock()
  {
    return mock_pin;
  }

  void Arm()
  {
    check_on_destruction = true;
    Fake(Method(mock_pin, ModuleInitialize));
  }

  void Check()
  {
    if (check_on_destruction)
    {
      try
      {
        Verify(Method(mock_pin, ModuleInitialize)).Once();
      }
      catch (...)
      {
        REQUIRE_MESSAGE(false, "Initialize() was not called on pin: " << name_);
      }
      CHECK(kExpectedSettings == mock_pin.get().CurrentSettings());
    }
  }

  ~AutoVerifyModule() {}

 protected:
  Mock<ModuleInterface> mock_pin;
  const char * name_                 = "";
  const Settings_t kExpectedSettings = {};
  bool check_on_destruction          = false;
};

//
class MockPin
{
 public:
  MockPin(const char * pin_name, sjsu::PinSettings_t expected_settings)
      : pin_name_(pin_name), kExpectedSettings(expected_settings)
  {
    mock_pin.get().settings = sjsu::PinSettings_t{};
  }
  auto & GetPin()
  {
    return mock_pin.get();
  }
  auto & GetMock()
  {
    return mock_pin;
  }
  void Arm()
  {
    check_on_destruction = true;
    Fake(Method(mock_pin, ModuleInitialize));
  }
  ~MockPin()
  {
    if (check_on_destruction)
    {
      try
      {
        Verify(Method(mock_pin, ModuleInitialize)).Once();
      }
      catch (...)
      {
        REQUIRE_MESSAGE(false,
                        "Initialize() was not called on pin: " << pin_name_);
      }
      CHECK(kExpectedSettings == mock_pin.get().CurrentSettings());
    }
  }

 protected:
  Mock<sjsu::Pin> mock_pin;
  const char * pin_name_                      = "";
  const sjsu::PinSettings_t kExpectedSettings = {};
  bool check_on_destruction                   = false;
};
}  // namespace sjsu

namespace doctest
{
}  // namespace doctest
