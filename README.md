# SJSU-Dev2

[![Build Status](https://travis-ci.com/kammce/SJSU-Dev2.svg?branch=master)](https://travis-ci.com/kammce/SJSU-Dev2)
[![Documentation Status](https://readthedocs.org/projects/sjsu-dev/badge/?version=latest)](http://sjsu-dev2.readthedocs.io/en/latest)
[![Coverage Status](https://coveralls.io/repos/github/kammce/SJSU-Dev2/badge.svg)](https://coveralls.io/github/kammce/SJSU-Dev2)
[![Codacy Badge](https://api.codacy.com/project/badge/Grade/6f004895337c42459f881db938e84885)](https://www.codacy.com/app/kammce/SJSU-Dev2?utm_source=github.com&amp;utm_medium=referral&amp;utm_content=kammce/SJSU-Dev2&amp;utm_campaign=Badge_Grade)
[![GitHub stars](https://img.shields.io/github/stars/kammce/SJSU-Dev2.svg)](https://github.com/kammce/SJSU-Dev2/stargazers)
[![GitHub forks](https://img.shields.io/github/forks/kammce/SJSU-Dev2.svg)](https://github.com/kammce/SJSU-Dev2/network)
[![GitHub issues](https://img.shields.io/github/issues/kammce/SJSU-Dev2.svg)](https://github.com/kammce/SJSU-Dev2/issues)
[![Slack Chat](https://img.shields.io/badge/join-slack-purple.svg?logo=slack&longCache=true&style=flat)](https://slofile.com/slack/sjsu-dev2)

Cross platform firmware framework written by students, alumni, and faculty of
San Jose State University. Designed for the original purpose of helping students
develop firmware for the SJTwo board.

## Operating System Supported

<p align="center">
<img
src="https://assets.ubuntu.com/v1/29985a98-ubuntu-logo32.png"
height="100px"/>
&nbsp;&nbsp;&nbsp;&nbsp;
<img
src="http://cdn.osxdaily.com/wp-content/uploads/2010/10/giant-apple-logo-bw.png"
height="100px" />
&nbsp;&nbsp;&nbsp;&nbsp;
<img
src="https://cdn.worldvectorlogo.com/logos/microsoft-windows-22.svg"
height="100px" />
</p>

Built for **Ubuntu (16.04 or above)**, **Mac OSX (High Sierra or above)**, and
**Windows 10 WSL**.

## Full Documentation and Installation Guide

For full guide follow this link:
**[documentation](http://sjsu-dev2.readthedocs.io/en/latest/?badge=latest)**

## Code Reference and APIs

See **[APIs](https://kammce.github.io/SJSU-Dev2/api/html/)**

## Using a Prebuilt Virtual Machine
One of the easiest ways to get started with SJSU-Dev2 is to use a VM with all of
the software installed.

Steps to install virtual box and the virtual machine are listed below:

1. Download and install Virtualbox and then the "VirtualBox Oracle VM VirtualBox
   Extension Pack" on your computer from this link:
   https://www.virtualbox.org/wiki/Downloads
2. Download the prebuilt virtual machine:
   **[Ubuntu SJSU-Dev.ova](https://drive.google.com/file/d/1SNUkQY07GViJBu7H4jGsOoMN5gbs7kBa/view)**
3. Open "Ubuntu SJSU-Dev.ova" and import into VirtualBox.

### Using the Virtual Machine
1. Open the "Ubuntu SJSU-Dev" Virtual Machine on the left hand side.
2. Login by entering the password "osboxes.org"
3. To program your board, you will need to connect it to your host machine and
   bring it into the virtual machine by using the top menu:
   `Devices > USB > CP2102n...`
4. At this point you can run commands like `make application` and `make flash`
   from within the SJSU-Dev2 folder which is located `/home/osboxes/SJSU-Dev2`

## Quick Start Install on host machine

### Setting up the environment
If you are using Windows, follow these steps to
**[install WSL](https://docs.microsoft.com/en-us/windows/wsl/install-win10)**
and make sure to install the Ubuntu as the linux distro. Once you have installed
WSL, all instructions below for Linux should work for Windows.

To download and setup the environment, simply copy and paste this into a
terminal:

```
git clone https://github.com/kammce/SJSU-Dev2.git
cd SJSU-Dev2
./setup
```

If you find that git is not installed on your machine follow these steps to
**[install GIT](https://git-scm.com/book/en/v2/Getting-Started-Installing-Git)**.

### Building a Project
The starter `hello_world` project can be found in the `projects/` folder along
with the the `demos/` folder which is full of examples you can run
on your board. To build `hello_world` :

    cd project/hello_world
    make application

### Programming a board
From within a project, run `make flash`.

If `make flash` is not available for your platform, then you can try
`make jtag-flash DEBUG_DEVICE=<jlink|stlink|etc> PLATFORM=<insert platform here>`
if you have a JTAG or SWD debugger.

### Viewing Serial Output
The preferred method for communicating with a serial device is via Google
Chrome, using the online serial terminal tool,
**[Telemetry](https://kammce.github.io/Telemetry)**.
You can also open this up on your browser using the `make telemetry` command in
a project directory.


## Supported Processors
| Processor/OS   | InterruptController | SystemTimer | CycleCounter |
|----------------|---------------------|-------------|--------------|
| RISCV 32I      | ☒                   | ☒           | ☒            |
| Arm Cortex M4F | ☑                   | ☑           | ☑            |
| Arm Cortex M3  | ☑                   | ☑           | ☑            |
| Embedded Linux | -                   | -           | -            |

## Supported Platforms
* ☑ = Supported
* ◯ = Partially supported
* ☒ = Not supported but available
* - = Not available on microcontroller

| Processor/OS   | MCU/SOC   | SystemController | Gpio | Pin | Uart | Adc | Pwm | I2c | Spi | Dac | Timer | Can | Eeprom | Flash | PulseCapture | Watchdog |
|----------------|-----------|------------------|------|-----|------|-----|-----|-----|-----|-----|-------|-----|--------|-------|--------------|----------|
| Arm Cortex M4F | lpc40xx   | ☑                | ☑    | ☑   | ☑    | ☑   | ☑   | ☑   | ☑   | ☑   | ☑     | ◯   | ☑      | ☒     | ☑            | ☑        |
| Arm Cortex M3  | lpc17xx   | ☑                | ☑    | ☑   | ☑    | ☑   | ☑   | ☑   | ☑   | ☑   | ☑     | ◯   | -      | ☒     | ☑            | ☑        |
| Arm Cortex M3  | stm32f10x | ◯                | ☒    | ☒   | ☒    | ☒   | ☒   | ☒   | ☒   | ☒   | ☒     | ☒   | -      | ☒     | ☒            | ☒        |
| Arm Cortex M4F | stm32f4xx | ◯                | ☑    | ☑   | ☒    | ☒   | ☒   | ☒   | ☒   | ☒   | ☒     | ☒   | -      | ☒     | ☒            | ☒        |
| RISCV 32IMAC   | gd32v10x  | ☒                | ☒    | ☒   | ☒    | ☒   | ☒   | ☒   | ☒   | ☒   | ☒     | ☒   | -      | ☒     | ☒            | ☒        |
| RISCV 32IMAC   | gd32v10x  | ☒                | ☒    | ☒   | ☒    | ☒   | ☒   | ☒   | ☒   | ☒   | ☒     | ☒   | -      | ☒     | ☒            | ☒        |
| Linux          | any       | ☒                | ☒    | ☒   | ☒    | ☒   | ☒   | ☒   | ☒   | ☒   | ☒     | ☒   | -      | ☒     | ☒            | ☒        |

## Future Goals of SJSU-Dev2
- [x] Integrate user-defined literals for SI units
- [ ] Platform Additions
  - [ ] Support for Raspberry Pi and other SBCs such as the BeagleBone Black
  - [x] Support for host side application development
  - [x] Support of STM32 series of MCUs
  - [ ] Support of TI series of MCUs
  - [ ] Support of RISC-V
- [x] Multi Threading Portability
  - [x] Add FreeRTOS wrapper of POSIX calls
    - [x] Allows Linux platforms to work with code that makes calls to FreeRTOS.
- [ ] Move from **Return-Error-Codes** to **C++ Exceptions**
  - [ ] Make a version of C++ exceptions that is more optimal for embedded

## Contributors
* [Khalil Estell](http://kammce.io): Creator and maintainer of the SJSU-Dev2.
* David Huang
* Matthew Boyd
* Since this repository borrows heavily form
[SJSU-Dev repo](https://github.com/kammce/SJSU-Dev), the people in that contrib
list are also contributors to this repository.
  * SJSU-Dev was based on SJDev by Preet Kang.

## Special Credits
* **Mikko Bayabo**: Windows surface destructive testing
* **WSL testing**: Sameer Azer, Aaron Moffit, Ryan Lucus, Onyema Ude
