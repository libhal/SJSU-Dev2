# FreeRTOS Periodic Scheduler

- [FreeRTOS Periodic Scheduler](#freertos-periodic-scheduler)
- [Location](#location)
- [Type](#type)
- [Background](#background)
- [Overview](#overview)
- [Detailed Design](#detailed-design)
  - [API](#api)
  - [PeriodicTask and TaskFunction](#periodictask-and-taskfunction)
    - [using TaskFunction = std::function<void(uint32_t count)>;](#using-taskfunction--stdfunctionvoiduint32t-count)
    - [explicit PeriodicTask(const char *, Priority, TaskFunction *, TaskSchedulerInterface &)](#explicit-periodictaskconst-char--priority-taskfunction--taskschedulerinterface)
    - [bool Run() override](#bool-run-override)
    - [TaskFunction * GetTaskFunction() const override](#taskfunction--gettaskfunction-const-override)
    - [SemaphoreHandle_t GetPeriodicSemaphore() const override](#semaphorehandlet-getperiodicsemaphore-const-override)
    - [uint32_t GetRunCount() const override](#uint32t-getruncount-const-override)
  - [PeriodicScheduler](#periodicscheduler)
    - [explicit PeriodicScheduler(TaskSchedulerInterface &, const char *)](#explicit-periodicschedulertaskschedulerinterface--const-char)
  - [Managing Task Execution](#managing-task-execution)
    - [void SetTask(PeriodicTaskInterface * task, Frequency frequency) override](#void-settaskperiodictaskinterface--task-frequency-frequency-override)
    - [PeriodicTaskInterface * GetTask(Frequency frequency) const](#periodictaskinterface--gettaskfrequency-frequency-const)
    - [static void HandlePeriodicTimer(TimerHandle_t timer)](#static-void-handleperiodictimertimerhandlet-timer)
- [Caveats](#caveats)
- [Future Advancements](#future-advancements)
- [Testing Plan](#testing-plan)
  - [Unit Testing Scheme](#unit-testing-scheme)
    - [PeriodicTask](#periodictask)
      - [GetPeriodicSemaphore()](#getperiodicsemaphore)
      - [GetTaskFunction()](#gettaskfunction)
    - [PeriodicScheduler](#periodicscheduler-1)
      - [PeriodicScheduler constructor](#periodicscheduler-constructor)
      - [GetTask()](#gettask)
  - [Demonstration Project](#demonstration-project)

# Location
`systems`

# Type
Implementation

# Background
The `PeriodicScheduler` and `PeriodicTask` classes inherit the
[`Task`](/documentation/design_docs/L3/task_interface.md) interface and provides
an abstract layer for managing FreeRTOS tasks that are expected to execute at
periodic intervals.

# Overview
The `PeriodicScheduler` is responsible for managing `PeriodicTask`s. A
`PeriodicTask` is an individual task object containing a `TaskFunction` of
the desired code that will be executed at a fixed frequency of 1Hz, 10Hz, 100Hz,
or 1000Hz. The scheduler manages the execution of each task through the use of
[FreeRTOS software timers](https://www.freertos.org/RTOS-software-timer.html).

The FreeRTOS tick period must be set to 1ms per 1 tick.

# Detailed Design
## API
```c++
class PeriodicTaskInterface
{
 public:
  using TaskFunction = std::function<void(uint32_t count)>;

  virtual TaskFunction * GetTaskFunction() const = 0;
  virtual SemaphoreHandle_t GetPeriodicSemaphore() = 0;
  virtual uint32_t GetRunCount() = 0;
};

template <size_t kTaskStackSize>
class PeriodicTask final : public Task<kTaskStackSize>,
                           public virtual PeriodicTaskInterface
{
 public:
  explicit PeriodicTask(const char * name,
                        Priority priority,
                        TaskFunction * task_function,
                        TaskSchedulerInterface & task_scheduler);

  bool Run() override;
  TaskFunction * GetTaskFunction() const override;
  SemaphoreHandle_t GetPeriodicSemaphore() override;
  uint32_t GetRunCount() const override;

 protected:
  TaskFunction * task_function_;
  StaticSemaphore_t semaphore_buffer_;
  SemaphoreHandle_t semaphore_;
  uint32_t run_count_;
};

class PeriodicScheduler final : public Task<512>
{
 public:
  enum class Frequency : uint8_t
  {
    k1Hz = 0,
    k10Hz,
    k100Hz,
    k1000Hz,
    kCount
  };

  static constexpr size_t kMaxTaskCount = Value(Frequency::kCount);

  explicit PeriodicScheduler(const char * name,
                             TaskSchedulerInterface & task_scheduler);
  bool Run() override;
  void SetTask(PeriodicTaskInterface * task, Frequency frequency);
  PeriodicTaskInterface * GetTask(Frequency frequency) const;

 private:
  TimerHandle_t timers_[kMaxTaskCount];
  StaticTimer_t timer_buffers_[kMaxTaskCount];
  PeriodicTaskInterface * task_list_[kMaxTaskCount];

  static void HandlePeriodicTimer(TimerHandle_t timer);
};
```

## PeriodicTask and TaskFunction

### using TaskFunction = std::function<void(uint32_t count)>;
A `TaskFunction` should not contain any blocking code. For example,
`vTaskDelay()` and `vTaskDelayUntil()` should not be used within the function.

### explicit PeriodicTask(const char *, Priority, TaskFunction *, TaskSchedulerInterface &)
Although a `PeriodicTask` are scheduled by the `PeriodicScheduler`, each task
must first be created and initialized by the specified `task_scheduler`.

The following sequence is performed when the object is constructed:
1. Assert a fatal error if the task_function is a `nullptr`.
2. Statically create the `semaphore_` that will be used by the
   `PeriodicScheduler` to control when the task is executed.
3. Set the initial `run_count_` to 0.

### bool Run() override
When the FreeRTOS scheduler is started, the task shall attempt to wait until the
`semaphore_` is given by the `PeriodicScheduler` before executing the
`task_function_`. This is repeated each time the task function is executed.

### TaskFunction * GetTaskFunction() const override
Returns the task function that is executed periodically.

### SemaphoreHandle_t GetPeriodicSemaphore() const override
Returns the FreeRTOS semaphore used by the PeriodicScheduler to manage the
execution of the `task_function_`.

### uint32_t GetRunCount() const override
Returns the number of times the task function has successfully executed.

## PeriodicScheduler

### explicit PeriodicScheduler(TaskSchedulerInterface &, const char *)
The following is performed when the task is constructed:
1. Sets pointers of `task_list_` to `nullptr`.
2. Statically initializes a FreeRTOS software timer for each of the following
   frequencies: 1Hz, 10Hz, 100Hz, and 1000Hz. Each timer is set to auto-reload
   and `HandlePeriodicTimer()` is set as the callback handler.

## Managing Task Execution

### void SetTask(PeriodicTaskInterface * task, Frequency frequency) override
The `task_list_` holds the 1Hz, 10Hz, 100Hz, and 1000Hz tasks starting with the
1Hz task at index 0. Tasks are scheduled using the `SetTask()` method. When a
task is scheduled, the pointer of the task is stored as the timer's `pvTimerID`
in order to be able to identify which task should be handled in the
`HandlePeriodicTimer()` callback.

### PeriodicTaskInterface * GetTask(Frequency frequency) const
Returns the scheduled task for a specified frequency. If no task has been
scheduled then a `nullptr` is returned.

### static void HandlePeriodicTimer(TimerHandle_t timer)
Each of the software timers are set to trigger this callback method. Since this
callback handles multiple timers, the pointer of scheduled task is stored as the
timer’s `pvTimerID` to serve as an identifier to determine which task to execute
when the timer expires. When the callback is invoked, the `pvTimerID` will be
examined to determine whether a task has been assigned, if the value is not a
`nullptr`, the task’s semaphore will be given to allow it to execute.

# Caveats
N/A

# Future Advancements
N/A

# Testing Plan
## Unit Testing Scheme
The `PeriodicTask` and `PeriodicScheduler` classes are to be tested.

### PeriodicTask
The `TaskScheduler` shall be mocked and injected to the `PeriodicTask`.

The following functions of the `PeriodicTask` are tested:

#### GetPeriodicSemaphore()

1. `xQueueGenericCreateStatic()` shall be faked to return a pointer reference
   to a dummy semaphore.
   - The function should return the pointer to the dummy semaphore.

#### GetTaskFunction()

1. The function should return the pointer of the mock task function that is
   assigned when the `PeriodicTask` was constructed.

### PeriodicScheduler
Mock functions shall be declared for each frequency option. Additionally, the
following methods from the FreeRTOS library shall be faked:

- `xTimerCreateStatic()`
- `xTimerGenericCommand()`
- `vTimerSetTimerID()`

The following functions of the `PeriodicScheduler` are tested:

#### PeriodicScheduler constructor

1. When a task is constructed, it should be added to the scheduler and the task
   count should be incremented by 1.

#### GetTask()

1. The function should return a `nullptr` if a task was not scheduled for the
   specified frequency.
2. A mock task shall be set and stubbed for each frequency option.
   - The function should return the pointer of the task that is scheduled for
     the specified frequency.

## Demonstration Project
An example project containing a two printer tasks are used to demonstrate the
use of the PeriodicScheduler. Two TaskFunctions are defined with each scheduled
to be executed at 1Hz and 10Hz respectively. Each task outputs its counter that
is incremented each time the task is executed. The 1Hz task should print every 1
second while the 10Hz task prints every 1 millisecond.

The project can be found
[here](/demos/multiplatform/periodic_scheduler/source/main.cpp).
