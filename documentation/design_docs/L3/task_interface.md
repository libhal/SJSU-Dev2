# FreeRTOS Task Interface

- [FreeRTOS Task Interface Design Document](#freertos-task-interface-design-document)
- [Location](#location)
- [Type](#type)
- [Background](#background)
- [Overview](#overview)
- [Detailed Design](#detailed-design)
  - [API](#api)
  - [Static Allocation of Tasks](#static-allocation-of-tasks)
  - [Initialization](#initialization)
    - [bool DeclaredOnStack()](#bool-declaredonstack)
    - [explicit constexpr Task(const char *, Priority, TaskSchedulerInterface &)](#explicit-constexpr-taskconst-char--priority-taskschedulerinterface)
  - [Inheriting the Task Interface class](#inheriting-the-task-interface-class)
    - [void Setup() override](#void-setup-override)
    - [void PreRun() override](#void-prerun-override)
    - [void Run() override](#void-run-override)
  - [Suspending and Resuming a Task](#suspending-and-resuming-a-task)
    - [void Suspend() const override](#void-suspend-const-override)
    - [void Resume() const override](#void-resume-const-override)
  - [Removing a Task](#removing-a-task)
    - [void Delete() override](#void-delete-override)
  - [Configuring Execution Frequency](#configuring-execution-frequency)
    - [void SetDelayTime(uint32_t time) override](#void-setdelaytimeuint32t-time-override)
    - [const uint32_t GetDelayTime() const override](#const-uint32t-getdelaytime-const-override)
  - [Additional Helper Functions](#additional-utility-methods)
    - [const char * GetName() const override](#const-char--getname-const-override)
    - [Priority GetPriority() const override](#priority-getpriority-const-override)
    - [size_t GetStackSize() const override](#sizet-getstacksize-const-override)
    - [void SetHandle(TaskHandle_t * handle) override](#void-sethandletaskhandlet--handle-override)
    - [TaskHandle_t * GetHandle() override](#taskhandlet--gethandle-override)
    - [StaticTask_t * GetTaskBuffer() override](#statictaskt--gettaskbuffer-override)
    - [StackType_t * GetStack() override](#stacktypet--getstack-override)
- [Caveats](#caveats)
- [Future Advancements](#future-advancements)

# Location
`systems`

# Type
Interface

# Background
The `Task` interface is an abstraction layer for FreeRTOS tasks.

Information regarding FreeRTOS tasks can be found
[here](https://www.freertos.org/a00015.html).

# Overview
Tasks that are scheduled and managed by the
[`TaskScheduler`](/documentation/design_docs/L3/task_scheduler.md).
Task objects must inherit the `Task` interface.

# Detailed Design
## API
```C++
class TaskInterface
{
 public:
  virtual TaskSchedulerInterface & GetTaskScheduler() const = 0;
  virtual bool Setup()                                      = 0;
  virtual bool PreRun()                                     = 0;
  virtual bool Run()                                        = 0;
  virtual void Suspend() const                              = 0;
  virtual void Resume() const                               = 0;
  virtual void Delete() const                               = 0;
  virtual const char * GetName() const                      = 0;
  virtual Priority GetPriority() const                      = 0;
  virtual size_t GetStackSize() const                       = 0;
  virtual void SetHandle(TaskHandle_t * handle)             = 0;
  virtual TaskHandle_t * GetHandle()                        = 0;
  virtual StaticTask_t * GetTaskBuffer()                    = 0;
  virtual StackType_t * GetStack()                          = 0;
  virtual void SetDelayTime(uint32_t time)                  = 0;
  virtual uint32_t GetDelayTime() const                     = 0;
};

template <size_t kStackSize>
class Task : public TaskInterface
{
 public:
  TaskSchedulerInterface & GetTaskScheduler() const override;
  bool Setup() override;
  bool PreRun() override;
  void Suspend() const override;
  void Resume() const override;
  void Delete() const override;
  const char * GetName() const override;
  Priority GetPriority() const override;
  size_t GetStackSize() const override;
  void SetHandle(TaskHandle_t * handle) override;
  TaskHandle_t * GetHandle() override;
  StaticTask_t * GetTaskBuffer() override;
  StackType_t * GetStack() override;
  void SetDelayTime(uint32_t time) override;
  uint32_t GetDelayTime() const override;
  virtual ~Task() {}

 protected:
  explicit constexpr Task(const char * name,
                          Priority priority,
                          TaskSchedulerInterface & task_scheduler);
  bool DeclaredOnStackCheck() const;

  TaskSchedulerInterface & task_scheduler_;
  const char * const kName;
  const Priority kPriority;
  TaskHandle_t handle_;
  uint32_t delay_time_;
  StaticTask_t task_buffer_;
  StackType_t stack_[kStackSize];
};
```

## Static Allocation of Tasks
Tasks are statically allocated and the FreeRTOS task handle is created and
managed by the `TaskScheduler`. To enable the static allocation of tasks, the
`configSUPPORT_STATIC_ALLOCATION` definition is set to 1 in
[FreeRTOSConfig.h](/library/platforms/freertos/FreeRTOSConfig.h) and the
implementation of `vApplicationGetIdleTaskMemory()` must also be provided to
statically allocate memory for the FreeRTOS idle task when
`vTaskStartScheduler()` is invoked.

> **Note:** Static allocation for FreeRTOS is enabled by default in SJSU-Dev2.

```c++
static StaticTask_t idle_task_tcb;
static StackType_t idle_task_stack[configMINIMAL_STACK_SIZE];
extern "C" void vApplicationGetIdleTaskMemory(
    StaticTask_t ** ppx_idle_task_tcb_buffer,
    StackType_t ** ppx_idle_task_stack_buffer,
    uint32_t * pul_idle_task_stack_size)
{
  *ppx_idle_task_tcb_buffer   = &idle_task_tcb;
  *ppx_idle_task_stack_buffer = idle_task_stack;
  *pul_idle_task_stack_size   = std::size(idle_task_stack);
}
```

> More information regarding static allocation of FreeRTOS objects can be found
> [here](https://www.freertos.org/a00110.html#configSUPPORT_STATIC_ALLOCATION).

## Initialization

### bool DeclaredOnStack()
The `DeclaredOnStack()` method checks if the address position of the `Task`
object is within the bounds of the heap or the .bss and data segments. A fatal
error is asserted if the tasks exists on the stack.

### explicit constexpr Task(const char *, Priority, TaskSchedulerInterface &)
The following is performed when the task is constructed:

1. Store the specified name, priority, and designated `TaskScheduler`.
2. Set the initial delay time to 0.
3. Invoke `DeclaredOnStack()` to check the task exists in the heap or is
   declared globally.
4. Add the task to the specified `task_scheduler`. This scheduler will be
   responsible for creating and managing this task.

## Inheriting the Task Interface class
When a task begins, the methods Setup(), PreRun(), and Run() are invoked in
the following order:

1. Setup()
2. PreRun()
3. Run()

These methods should be overridden in the inheriting class to provide any
initialization and/or execution code that are performed by the task.

### void Setup() override
Setup is performed when `Start()` of the TaskScheduler is invoked. If the
`Setup()` of a scheduler task fails to complete, the scheduler will not start
and a fatal error will be asserted. The function should be overridden with any
initialization code that the task requires.

### void PreRun() override
PreRun() is invoked after starting the task scheduler once before `Run()` is
ever called. `PreRun()` is performed after `Setup()` of all scheduled Tasks are
complete. Therefore, this method should be overridden if the task requires a
different `Task` to be setup first.

### void Run() override
Continuously invoked until the task has been temporarily suspended by
`Suspend()` or terminated.

**The `Run()` method MUST be overridden with the desired task execution code.**

## Suspending and Resuming a Task

### void Suspend() const override
Suspends the task until resumed by `Resume()`.

### void Resume() const override
Resumes the task if the task was suspended by `Suspend()`.

## Removing a Task

### void Delete() override
Suspends the task if it is running and invokes the `TaskScheduler`’s
`RemoveTask()` method to remove this task from the scheduler.

## Configuring Execution Frequency
A task can be configured to be executed at a constant desired frequency through
the `SetDelayTime()` function.

### void SetDelayTime(uint32_t time) override
Sets the delay time in FreeRTOS ticks to ensure `Run()` is called at a desired
frequency for periodic tasks. For example, if time = 1000 and the rtos tick
period is 1ms, then `Run()` will be invoked every 1 second.

### const uint32_t GetDelayTime() const override
Returns the delay time that was set by SetDelayTime() in milliseconds.

## Additional Helper Functions
### const char * GetName() const override
Returns the name used to identify the task.

### Priority GetPriority() const override
Returns the priority of the task.

### size_t GetStackSize() const override
Returns the pre-allocated stack size for the task in bytes.

### void SetHandle(TaskHandle_t * handle) override
This method is used by the `TaskScheduler` to set the task FreeRTOS task handle
when the task is created. This handle is used for suspending, resuming, or
deleting the task.

### TaskHandle_t * GetHandle() override
Returns a pointer reference of the task's handle.

### StaticTask_t * GetTaskBuffer() override
Returns a pointer reference to the task's Task Control Block (TCB);

### StackType_t * GetStack() override
Returns a pointer reference of the task's pre-allocated stack.

# Caveats
The `TaskInterface` class is used to provide a generalization for the `Task`
class since the `Task` class takes in a template argument.

# Future Advancements
N/A
