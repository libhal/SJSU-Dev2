# Motor Controller Design Document

- [Motor Controller Design Document](#motor-controller-design-document)
  - [Objective](#objective)
  - [Background](#background)
  - [Overview](#overview)
  - [Detailed Design](#detailed-design)
    - [MotorController Interface](#motorcontroller-interface)
    - [API](#api)
    - [Initialization](#initialization)
    - [Setting Motor Speed (Polling)](#setting-motor-speed-polling)
    - [Setting Motor Speed (Interrupts)](#setting-motor-speed-interrupts)
  - [Caveats](#caveats)
  - [Test Plan](#test-plan)
    - [Unit Testing Scheme](#unit-testing-scheme)
    - [Integration Testing](#integration-testing)
    - [Demonstration Project](#demonstration-project)

## Objective
To develop a generic motor controller capable of using a motor's driver to perform more advaced functions

## Background
The motor controler is being developed with the intention of leveragin the motor driver API to perform fucntions such as setting the motor's speed, RPM, etc. Certain functionalities will be limited based on the hardware, for example if current sense is present or not. //TODO: make more detailed and provide info


## Overview
The motor controller will interface with the hardware that drives the motor, the motor driver which controlls basic on/ off functionality, various peripherals such as current sensor and rotary encoder to assit with motor control and the MCU in order to create a simple interface to control the motor with more advanced functions. The intial objective is to provide a set speed function. 

## Detailed Design

### MotorController Interface
```c++
namespace sjsu
{
class MotorController
{
 public:
  virtual Status Initialize() const              = 0;
  virtual uint32_t SetSpeed(uint32_t speed, bool foward) const = 0;
}; 
}
```

### API
```c++
namespace sjsu
{
class MotorController final : public MotorPWM
{
public:
  constexpr explicit MotorController(uint32_t motor_number, uint32_t speed);
  void Initialize() const override;
  void SetSpeed(uint32_t speed, bool foward) const override;
private:
  const uint32_t kMotorNumber;
  const uint32_t kSpeed;
};
}
```

### Initialization
```c++
Status Initialize() const override;
```
The following sequence is performed to initialize the driver:
1. Initialize the GPIO and PWM pins associated with the motor number.
2. Return the initialization `Status`.

### Setting Motor Speed (Polling)
```c++
void SetSpeed(uint32_t speed, bool foward) const override;
```
`SetSpeed` sets the speed in RPM of the associated motor. If the boolean foward is set true, the motor will rotate foward, otherwise it will rotate backwards

### Setting Motor Speed (Interrupts)
To be added...

## Caveats
The number of usable motors is unknown to the function and won't have an error handling for invalid motor numbers. 

## Test Plan

### Unit Testing Scheme
The UART driver shall be mocked and stubbed to inject test inputs for the Tsop752 driver.

|  Function             |  Expected Result                                    |
|:---------------------:|:---------------------------------------------------:|
| SetSpeed              | Should set the motor to the appropriate rpm and direction |

### Integration Testing
To be added...

### Demonstration Project
To be added...
