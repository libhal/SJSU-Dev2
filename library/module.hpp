#pragma once

#include <cstdint>

namespace sjsu
{
/// Module is the basis interface for all other SJSU-Dev2 interfaces. It
/// incorporates an API for initialization and setup as well as a state to know
/// which stage the module is in.
///
/// The module usage flow:
///
///  State: kReset
///     All modules start off with state kReset. At this point the only method
///     that should be called is Initialize(). Calling any other method is
///     undefined behavior. The work of Initialize() must NOT be done in the
///     constructor of a Module derived class as the order of initialization is
///     not deterministic and can also result in undefined behavior.
///
///  State: kInitialized
///     When Initialize() is called, the state of the module is transitioned to
///     kInitialized, if, and only if, the state of the module is in kReset
///     beforehand. Otherwise, no work is done by Initialize(). This allows
///     Initialize() to be called multiple times without performing the same
///     task multiple times. This is helpful when a module is used by two or
///     more other modules and both modules call Initialize().
///
///     After Initialize() has been executed without an exception being thrown,
///     the module can now be configured. SJSU-Dev2 convention for derived
///     modules APIs is to name methods that configure a system with the name
///     ConfigureX(), where X can be ClockRate, MaxRange, Frequency, etc. For
///     example, at this stage one can run ConfigureBaudRate(9600) on the Uart
///     driver. Only the Configuration methods shall be called at this stage.
///
/// State: kEnabled
///    Once the module has been appropriately configured for the usage of the
///    application. The module can be Enabled(). Once enabled, the module can
///    use the runtime usage method. These methods can be identified as they are
///    the methods of an API that does not have the "Configure-" prefix. These
///    usage APIs interact with hardware, such as SetDutyCycle() in the Pwm
///    driver or Transfer() for the SPI class. Configuration methods should not
///    be called when in this state. Doing so is undefined behavior.
///
/// State: kDisabled
///    In the event the Configuration options need to be change, the module must
///    first be disabled by running the Enabled(false) method with the input
///    parameter false. This will disable the module and allow usage of the
///    Configuration() methods. Using the Configuration() methods outside of the
///    kInitialized and kDisabled state is undefined behavior.
///
/// State: kCritical
///    If a module attempted to perform some task and it was unable to in an
///    exceptional way, the module will set its state to kCritical using the
///    SetState() method provided to all Modules. This indicates that the module
///    can no longer be used as normal. The only way out of this would be for
///    the system to call SetState() force the state back to kReset, and run
///    through the initialization sequence again. As a means to recover, a
///    system may consider changing the Configuration settings if these led to
///    the exception being called. Another alternative would be to run exit and
///    reset the system.
///
/// These rules of usage must be followed for proper operation by all module
/// within SJSU-Dev2 as well as external modules using the SJSU-Dev2 modules.
class Module
{
 public:
  // All of the possible states that a module can be in.
  enum class State : uint8_t
  {
    kReset,
    kInitialized,
    kEnabled,
    kDisabled,
    kCritical,
  };

  /// Initialize module by putting system into a known state and prepare it to
  /// be configured and enabled. In general this typically means resetting the
  /// device to be driven.
  ///
  /// THIS METHOD MUST BE CALLED FIRST BEFORE ANY OTHER METHOD OF THIS OR
  /// DERIVED OBJECTS. Failure to call this first will result in undefined
  /// behavior. Not doing this can result in hard faults when accessing
  /// peripherals.
  ///
  /// Once successfully initialized, the state of the module will transition to
  /// State::kInitialized, and subsequent calls to initialize will simply return
  /// without performing any work.
  ///
  /// Protocol for Initialization:
  ///
  /// Step 1) Initialize, configure, and enable all sub-modules.
  /// Step 2) Power on/reset device/peripheral such that the device can be
  ///         configured and then later enabled.
  void Initialize()
  {
    if (IsReset())
    {
      ModuleInitialize();
      SetState(State::kInitialized);
    }
  }

  /// Enable/Disable module such that the module can be used at run time.
  ///
  /// A successful Enable() call will change the state to kEnabled and will
  /// allow the runtime usage methods to be called without any undefined
  /// behaviour.
  ///
  /// A successful Enable(false) call will change the state to kDisabled and
  /// will allow the Configuration methods to be called again, and will disable
  /// operation of the system. The exact behavior of the system when
  /// transitioning from kEnabled to kDisabled is module specific. For example,
  /// the lpc40xx driver will set its duty cycle to 0% when disabled. The
  /// disable/enable transition information should be documented for the
  /// specific driver if it has an significant change to the state. For example
  /// most GPIOs do not change state when disabled, and thus may have a comment
  /// stating:
  ///
  /// "Transitions from enabled to disabled have no effect on the output signal"
  ///
  /// Protocol for Enable:
  ///
  ///   Step 1) If applicable, if the configuration settings were stored within
  ///           the module's memory and not stored within the memory of the
  ///           device itself, then it is the job of Enable() to write the
  ///           configuration information to the device.
  ///           THIS STEP IS AN EXCEPTION NOT THE RULE.
  ///           Always, refrain from caching configuration information if it can
  ///           be stored in the device's memory directly, such that enable can
  ///           reduce the work it needs to perform.
  ///
  ///   Step 2) Run any procedures required to enable this module for usage.
  ///           Sometimes this as simple as setting a bit in a register, in
  ///           other cases this is a more involved sequence of steps.
  void Enable(bool enable = true)
  {
    // Transition to enabled if the current state is State::kInitialized.
    const bool kToEnabled = (enable && (IsDisabled() || IsInitialized()));

    // Transition to disabled if the current state is State::kEnabled.
    const bool kToDisabled = (!enable && IsEnabled());

    if (kToEnabled || kToDisabled)
    {
      // Execute the derived class's implementation of enable. This shall throw
      // if attempting to enable or disable the module could not be completed.
      // This will cause the scope_fail to execute and the following lines below
      // that set the state to State::kEnabled and State::kDisabled to be
      // skipped.
      ModuleEnable(enable);

      // We are now past the point of exceptions being thrown, we can now set
      // the state to operation or disabled.
      if (enable)
      {
        SetState(State::kEnabled);
      }
      else
      {
        SetState(State::kDisabled);
      }
    }
  }

  /// @return the module's state
  State GetState() const
  {
    return state_;
  }

  /// Set the state of the module. In general this is called by the derived
  /// class in order to indicate that the module has changed state.
  ///
  /// In general, derived modules should only change the state of the module if
  /// something goes wrong, thus typically, derived modules will only move the
  /// state of the module to kCritical and thats it.
  ///
  /// Normally, this method should not be called directly from outside of a
  /// derived module, but can be used in order to revert or force the state of
  /// the module into some other state.
  ///
  /// @param new_state - The new state to set the module to
  void SetState(State new_state)
  {
    state_ = new_state;
  }

  /// Helper function that returns true if the state is in reset.
  ///
  /// @return true if state is equal to kReset.
  bool IsReset() const
  {
    return state_ == State::kReset;
  }

  /// Helper function that indicates if this module has not been fully
  /// configured and enabled. This will return false if the state of the module
  /// is equal to or beyond an enabled state, such as being Enabled, Disabled,
  /// or Critical.
  ///
  /// @return true - if the module has yet to be enabled.
  /// @return false - if the state is enabled or beyond.
  bool RequiresConfiguration() const
  {
    return state_ == State::kReset || state_ == State::kInitialized;
  }

  /// Helper function that returns true if the state is in Initialized.
  ///
  /// @return true if state is equal to kInitialized.
  bool IsInitialized() const
  {
    return state_ == State::kInitialized;
  }

  /// Helper function that returns true if the state is in Enabled.
  ///
  /// @return true if state is equal to kEnabled.
  bool IsEnabled() const
  {
    return state_ == State::kEnabled;
  }

  /// Helper function that returns true if the state is in Disabled.
  ///
  /// @return true if state is equal to kDisabled.
  bool IsDisabled() const
  {
    return state_ == State::kDisabled;
  }

  /// Helper function that returns true if the state is in Critical.
  ///
  /// @return true if state is equal to kCritical.
  bool IsCritical() const
  {
    return state_ == State::kCritical;
  }

  /// Helper function that sets the state of this module to Reset.
  ///
  /// @return true if state is equal to kReset.
  void SetStateToReset()
  {
    state_ = State::kReset;
  }

  /// Helper function that sets the state of this module to Initialized.
  ///
  /// @return true if state is equal to kInitialized.
  void SetStateToInitialized()
  {
    state_ = State::kInitialized;
  }

  /// Helper function that sets the state of this module to Enabled.
  ///
  /// @return true if state is equal to kEnabled.
  void SetStateToEnabled()
  {
    state_ = State::kEnabled;
  }

  /// Helper function that sets the state of this module to Disabled.
  ///
  /// @return true if state is equal to kDisabled.
  void SetStateToDisabled()
  {
    state_ = State::kDisabled;
  }

  /// Helper function that sets the state of this module to Critical.
  ///
  /// @return true if state is equal to kCritical.
  void SetStateToCritical()
  {
    state_ = State::kCritical;
  }

  /// Destructed of this base class calls the destructors of derived classes.
  virtual ~Module() = default;

  /// Module's implementation of Initialize. See the documentation for
  /// Initialize() for more details about what this should do.
  ///
  /// NOTE: that this implementation should not use SetState(), but should allow
  /// the Module::Interface() method to handle that.
  virtual void ModuleInitialize() = 0;

  /// Module Implementation of Enable.
  ///
  /// @param enable - if true, will enable the module, if false, will disabled
  virtual void ModuleEnable(bool enable) = 0;

 protected:
  /// Stores the state of the module. Default state of the module is kReset.
  State state_ = State::kReset;
};
}  // namespace sjsu
