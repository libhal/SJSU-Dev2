# SJSU-Dev2

[![Discord](https://img.shields.io/discord/800515757871726622?color=7389D8&logo=discord&logoColor=ffffff&labelColor=6A7EC2)](https://discord.gg/tSmCgeyM)
[![Github Actions](https://github.com/SJSU-Dev2/SJSU-Dev2/workflows/.github/workflows/presubmit.yml/badge.svg)](https://github.com/SJSU-Dev2/SJSU-Dev2/actions?query=workflow%3A.github%2Fworkflows%2Fpresubmit.yml)
[![codecov](https://codecov.io/gh/SJSU-Dev2/SJSU-Dev2/branch/master/graph/badge.svg?token=E7aAr2o1CT)](https://codecov.io/gh/SJSU-Dev2/SJSU-Dev2)
[![Documentation Status](https://readthedocs.org/projects/sjsu-dev/badge/?version=latest)](http://sjsu-dev2.readthedocs.io/en/latest)
[![Codacy Badge](https://api.codacy.com/project/badge/Grade/6f004895337c42459f881db938e84885)](https://www.codacy.com/app/SJSU-Dev2/SJSU-Dev2?utm_source=github.com&utm_medium=referral&utm_content=SJSU-Dev2/SJSU-Dev2&utm_campaign=Badge_Grade)
[![GitHub stars](https://img.shields.io/github/stars/SJSU-Dev2/SJSU-Dev2.svg)](https://github.com/SJSU-Dev2/SJSU-Dev2/stargazers)
[![GitHub forks](https://img.shields.io/github/forks/SJSU-Dev2/SJSU-Dev2.svg)](https://github.com/SJSU-Dev2/SJSU-Dev2/network)
[![GitHub issues](https://img.shields.io/github/issues/SJSU-Dev2/SJSU-Dev2.svg)](https://github.com/SJSU-Dev2/SJSU-Dev2/issues)

Cross platform firmware framework written by students, alumni, and faculty of
San Jose State University. Designed for the original purpose of helping students
develop firmware for the SJTwo board.

## 🏗️ WARNING: UNDER HEAVY CONSTRUCTION 🚧

Currently Ongoing Breaking Changes:

1. New common API for modules
2. Introduction of Docker Images and Containers to make development consistent
   across computer operating systems.
3. Renaming and reorganization of the library folder

Additional Considered Changes:

1. New build system (maybe meson, or CMake (ick), or our own custom build system
   called cpp-build)
2. Add package management system to separate platform and peripheral
   implementation details from main codebase.


## [📖 Full Documentation & Guides](http://sjsu-dev2.readthedocs.io/en/latest/?badge=latest)

## [⚙️ Software APIs](https://SJSU-Dev2.github.io/SJSU-Dev2/api/html/)

## 💻 Operating System Supported

- **Ubuntu (>= 18.04)**
- **Mac OSX (>= High Sierra)**

## 🚀 Quick Start

To download and setup the environment, simply copy and paste this into a
terminal:

```bash
git clone https://github.com/SJSU-Dev2/SJSU-Dev2.git
cd SJSU-Dev2
./setup
```

[![asciicast](https://asciinema.org/a/314726.svg)](https://asciinema.org/a/314726)

If you find that git is not installed on your machine follow these steps to
**[install GIT](https://git-scm.com/book/en/v2/Getting-Started-Installing-Git)**.

## 🛠️ Building a Project

A good project to start with would be the `hello_world` project. It can be found
in the `projects/` folder. The example projects can be found in the `demos/`
folder.

Building `hello_world`:

```bash
cd project/hello_world
make application
```

## 📥 Programming a Device

From within a project, run

```bash
make flash
```

[![asciicast](https://asciinema.org/a/314699.svg)](https://asciinema.org/a/314699)

Most platforms do not have `make flash` available, and in those cases JTAG/SWD
can be used to program a device. Hook up a JTAG or SWD to your MCU and execute
the following command with the correct JTAG and PLATFORM selected:

```bash
make program JTAG=<jlink|stlink|etc> PLATFORM=<stm32f10x|lpc17xx|etc>
```

### Example using a STLink to program a stm32f10x

```bash
make program JTAG=stlink PLATFORM=stm32f10x
```

## 🖥️ Viewing Serial Output

The preferred method for communicating with a serial devices is via Google
Chrome, using the online serial terminal tool,
**[Telemetry](https://SJSU-Dev2.github.io/Telemetry)**.
You can also open this up on your browser using the `make telemetry` command in
a project directory.

## 🔎 Debugging Device

If you are using a JTAG or SWD device with your MCU, you can debug the device with a
similiar command to programming:

```bash
make debug JTAG=<jlink|stlink|etc> PLATFORM=<stm32f10x|lpc17xx|etc>
```

### Example using a STLink to program a stm32f10x:

```bash
make debug JTAG=stlink PLATFORM=stm32f10x
```

STDOUT and STDIN will be available via the gdb debug monitor.

## 📦 Windows Users: Using a Virtual Machine

One of the easiest ways to get started with SJSU-Dev2 for Windows users is to use
a prebuilt virtual machine.

Steps to install virtual box and the virtual machine are listed below:

1. Download and install Virtualbox and then the "VirtualBox Oracle VM VirtualBox
   Extension Pack" on your computer from this link:
   [VirtualBox Download](https://www.virtualbox.org/wiki/Downloads)
2. Download a prebuilt VirtualBox image for Ubuntu from
   [https://www.osboxes.org/ubuntu/](https://www.osboxes.org/ubuntu/). If you don't
   know which version to choose, use 18.04.3.
3. Open the file with VirtualBox to setup and open the virtual machine.

### Setting up SJSU-Dev2

Now that the VM is up and running, you can use the quick start guide to download and
setup SJSU-Dev2.

### Connecting USB Devices to Virtual Machine

1. Open the Ubuntu Virtual Machine on the left hand side.
2. Login by entering the password "osboxes.org"
3. To program and interact with your board, you will need to connect it to your host
   machine and bring it into the virtual machine by using the top menu:
   `Devices > USB > CP2102n...`
4. At this point you can run commands like `make application` and `make flash`
   from within the SJSU-Dev2 folder.

## 🌌 Future Goals of SJSU-Dev2

- [x] Integrate user-defined literals for SI units
- [ ] Platform Additions
  - [x] Support for host side application development
  - [x] Support of STM32 series of MCUs
  - [x] Support of TI series of MCUs
  - [ ] Support for Raspberry Pi and other SBCs such as the BeagleBone Black
  - [ ] Support of RISC-V
- [x] Multi Threading Portability
  - [x] Add FreeRTOS wrapper of POSIX calls
    - [x] Allows Linux platforms to work with code that makes calls to FreeRTOS.
- [x] Move from **Return-Error-Codes** to **C++ std::expected**
- [x] Move from **C++ std::expected** to **exceptions handling**
- [ ] Package manager for adding SJSU-Dev2 libraries
- [ ] Package manager for adding custom platforms SJSU-Dev2
