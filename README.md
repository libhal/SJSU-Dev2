# SJSU-Dev2

[![Slack Chat](https://img.shields.io/badge/join-slack-purple.svg?logo=slack&longCache=true&style=flat)](https://slofile.com/slack/sjsu-dev2)
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

A good project to start with would be the `hello_world` project can be found in
the `projects/` folder. The `demos/` folder which is full of examples you can
run on your board.

To build `hello_world`:

```bash
cd project/hello_world
make application
```

## 📥 Programming a board

From within a project, run `make flash`.

[![asciicast](https://asciinema.org/a/314699.svg)](https://asciinema.org/a/314699)

If `make flash` is not available for your platform, then you can try
`make jtag-flash DEBUG_DEVICE=<jlink|stlink|etc> PLATFORM=<insert platform here>`
if you have a JTAG or SWD debugger.

## 📦 Using a Prebuilt Virtual Machine

One of the easiest ways to get started with SJSU-Dev2 is to use a VM with all of
the software installed.

Steps to install virtual box and the virtual machine are listed below:

1. Download and install Virtualbox and then the "VirtualBox Oracle VM VirtualBox
   Extension Pack" on your computer from this link:
   [VirtualBox Download](https://www.virtualbox.org/wiki/Downloads)
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

### Viewing Serial Output

The preferred method for communicating with a serial device is via Google
Chrome, using the online serial terminal tool,
**[Telemetry](https://SJSU-Dev2.github.io/Telemetry)**.
You can also open this up on your browser using the `make telemetry` command in
a project directory.

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
