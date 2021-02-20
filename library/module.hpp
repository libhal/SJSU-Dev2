#pragma once

#include <cstdint>
#include <cstring>
#include <type_traits>

namespace sjsu
{
/// Used to keep the SJSU-Dev2 compile time errors consistent, distinct and
/// easier to spot within the compiler error messages.
#define SJ2_ERROR_MESSAGE_DECORATOR(msg) \
  "\n\n"                                 \
  "SJSU-Dev2 Compile Time Error:\n"      \
  "    " msg                             \
  "\n"                                   \
  "\n"

/// Used for defining static_asserts that should always fail, but only if the
/// static_assert line is hit via `if constexpr` control block.
/// Prefer to NOT use this directly but to use `InvalidOptions` instead
template <auto... options>
struct InvalidOption_t : std::false_type
{
};

/// Helper definition to simplify the usage of InvalidOption_t.
/// @tparam options
template <auto... options>
inline constexpr bool InvalidOption = InvalidOption_t<options...>::value;

/// States of a SJSU-Dev2 Module
enum class State : uint8_t
{
  /// Module has been constructed and is not ready for use.
  /// In this state, the settings structure can be modified to suit the needs of
  /// the application.
  ///
  /// Initialize() must be called in order to use the rest of the class methods.
  /// Calling any other methods other than Initialize() in this state will cause
  /// undefined behaviour, typically leading to a crash.
  kReset = 0,

  /// Module has been initialized and is ready for usage. To reconfiguration the
  /// module with new settings, simply change the settings and run Initialize()
  /// again.
  kInitialized,

  /// Module has been put into low power state. To leave low power, run
  /// Initialize(). Calling any other methods other than Initialize() in
  /// this state will cause undefined behaviour, typically leading to a crash.
  kPowerDown,

  /// If a module attempts to perform some task and it was unable to do so in
  /// some exceptional way, the module will set its state to kCritical using the
  /// EnterCrisis() method provided to all Modules. This indicates that the
  /// module can no longer be used as normal. To exit kCritical, run
  /// Initialize(). Calling any other methods other than Initialize() in
  /// this state will cause undefined behaviour, typically leading to a crash.
  kCritical,
};

/// Gives settings objects a means to be compared at the byte level
///
/// @tparam Settings_t - the settings to allow memory comparison
template <class Settings_t>
struct MemoryEqualOperator_t
{
  /// @param other - the other settings to be compared against this settings
  /// @return true - Settings match byte for byte.
  /// @return false - Settings do not match
  bool operator==(const Settings_t & other) const
  {
    return memcmp(&other, this, sizeof(other)) == 0;
  };
};

/// An empty settings structure used to indicate that a module or interface does
/// not have generic settings.
struct EmptySettings_t
{
};

/// The basis class for all peripheral, device and system drivers in SJSU-Dev2.
template <class Settings_t = EmptySettings_t>
class Module
{
 public:
  /// Provides a publicly accessible name to type definition of the module's
  /// Settings_t.
  using SettingsType_t = Settings_t;

  // Compile time check to ensure that the Settings_t object is copyable,
  // because otherwise the settings after initialization could not be saved.
  static_assert(std::is_copy_constructible_v<Settings_t>,
                SJ2_ERROR_MESSAGE_DECORATOR(
                    "The settings for this module does not allow for copying. "
                    "Modules must have a copyable settings type in order to "
                    "save the settings used after initialization."));

  /// Every module MUST have an initialize for it operate
  virtual void ModuleInitialize() = 0;

  /// It may not always make sense for every driver to have a power down
  /// function so this is not pure virtual. This can be overriden if necessary
  virtual void ModulePowerDown(){};

  /// Apply settings as defined by the Settings_t and enable peripheral, device,
  /// or system.
  ///
  /// NOTE: Initialize() MUST be called before calling any other method in this
  /// class or any classes that derive from this class. THIS OR DERIVED OBJECTS.
  /// Failure to call this first will result in undefined behavior. Not doing
  /// this can result in hard faults when accessing peripherals.
  ///
  /// This method will call ModuleInitialize() which will perform the module
  /// specific initialization sequence. If ModuleInitialize() does not throw an
  /// exception, meaning success, then the state of the module will transition
  /// to State::kInitialized. Once in the initialized state, this function will
  /// not call ModuleInitialize() again unless the state is changed to something
  /// else.
  ///
  /// @return auto& - reference to itself to allow method chaining
  auto & Initialize()
  {
    ModuleInitialize();
    SaveSettings();
    state_ = State::kInitialized;
    return *this;
  }

  /// Will put the module into a powered down state. All submodules used by this
  /// module will have their power down be called well. Just like how
  /// Initialize() calls all Initialize() methods of its submodules.
  ///
  /// To get out of this state, Initialize() must be called. Many
  /// modules do not support a mode of power down and in these cases, a dummy
  /// implemenation is called.
  ///
  /// Once in a powered down state, calls to any other methods besides
  /// Initialize will result in undefined behaviour.
  ///
  /// @return auto& - reference to itself to allow method chaining
  auto & PowerDown()
  {
    if (state_ == State::kInitialized)
    {
      ModulePowerDown();
      state_ = State::kPowerDown;
    }
    return *this;
  }

  /// Set the state of the module as State::kCritical which is a marker that
  /// something went terribly wrong with this module such that it cannot work as
  /// intended without an attempt to run Initialize().
  ///
  /// @return auto& - reference to itself to allow method chaining
  auto & EnterCrisis()
  {
    state_ = State::kCritical;
    return *this;
  }

  /// Set the state of the module to Initialized. Should only be used by unit
  /// tests.
  ///
  /// @return auto& - reference to itself to allow method chaining
  auto & UnitTestEnterInitialized()
  {
    state_ = State::kInitialized;
    return *this;
  }

  /// @return Settings_t - the current operating settings from the latest call
  /// of Initialize()
  Settings_t CurrentSettings() const
  {
    return current_settings_;
  }

  /// @return State - the current state of the module
  State GetState() const
  {
    return state_;
  }

  /// Publically accessible settings structure used for configuring generic
  /// aspects of the module. For example, PWM would have a setting for frequency
  /// and UART would have a setting for baud_rate. When the module is
  /// initialized the settings are saved into a private variable and can be
  /// retrieved via the CurrentSettings() method.
  Settings_t settings;

  /// Add constexpr constructor which will allow derived classes to have
  /// constexpr constructors if possible.
  constexpr Module() noexcept {}

  /// Preventing this module from copy construction. Modules cannot be copied.
  Module(const Module &) = delete;
  /// Preventing this module from copy assignment. Modules cannot be copied.
  Module & operator=(const Module &) = delete;
  /// Preventing this module from move operation. Modules cannot be moved.
  Module(Module &&) = delete;
  /// Preventing this module from move assignment. Modules cannot be moved.
  Module & operator=(Module &&) = delete;

  /// Destructor of this base class calls the destructors of derived classes.
  virtual ~Module() = default;

 private:
  /// Helper function for saving settings.
  void SaveSettings()
  {
    current_settings_ = settings;
  }

  /// Saved settings from running Initialize()
  Settings_t current_settings_ = {};

  /// The current operating state of the module. Default is State::kReset. Once
  /// Initialized, the State::kReset can never be reached again.
  State state_ = State::kReset;
};
}  // namespace sjsu
