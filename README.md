# SJSU-Dev2

[![Build Status](https://travis-ci.org/kammce/SJSU-Dev2.svg?branch=master)](https://travis-ci.org/kammce/SJSU-Dev2)
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
<img src="https://assets.ubuntu.com/v1/29985a98-ubuntu-logo32.png" height="100px"/>
&nbsp;&nbsp;&nbsp;&nbsp;
<img src="http://cdn.osxdaily.com/wp-content/uploads/2010/10/giant-apple-logo-bw.png" height="100px" />
&nbsp;&nbsp;&nbsp;&nbsp;
<img src="https://cdn.worldvectorlogo.com/logos/microsoft-windows-22.svg" height="100px" />
</p>

Built for **Ubuntu**, **Mac OSX**, and **Windows 10 WSL**.

## Documentation and Installation Guide

See **[documentation](http://sjsu-dev2.readthedocs.io/en/latest/?badge=latest)**
for a full tutorial.

## Quick Start

### Setting up the environment
If you are using Windows, follow these steps to
**[install WSL](https://docs.microsoft.com/en-us/windows/wsl/install-win10)**
and make sure to install the Ubuntu as the linux distro. Once you have installed
WSL, all instructions below for Linux should work for Windows.

To download and setup the environment, simply copy and paste this into a
terminal:

    git clone https://github.com/kammce/SJSU-Dev2.git && cd SJSU-Dev2 && ./setup

If you find that git is not installed on your machine follow these steps to
**[install GIT](https://git-scm.com/book/en/v2/Getting-Started-Installing-Git)**.

### Building a Project
The starter `HelloWorld` project can be found in the `firmware/` folder along
with the the `firmware/examples` folder which is full of examples you can run
on your board. To build `HelloWorld`:

    cd firmware/HelloWorld
    make application

### Programming a board
From within a project, run `make flash`.

### Viewing Serial Output
The preferred method for communicating with a serial device is via Google
Chrome, using the online serial terminal tool,
**[Telemetry](https://kammce.github.io/Telemetry)**.
You can also open this up on your browser using the `make telemetry` command in
a project directory.

### Burning the Bootloader (NOT AVAILABLE YET)
The Hyperload bootloader is used to rapidly quickly program the device. SJTwo
and SJOne boards should both have the bootloaders pre-installed on them, but in
case it is not installed, run the following from the root of the SJSU-Dev2
directory:

    cd firmware/Hyperload
    make burn

`make burn` will build the application using `make bootloader` and if that
finishes successfully, then it will burn the bootloader to the first 64kB of the
board, allowing you to use `make flash` later on.

## Contrib
* [Khalil Estell](http://kammce.io): Creator of the SJSU-Dev2.
* Since this repository borrows heavily form
[SJSU-Dev repo](https://github.com/kammce/SJSU-Dev), the people in that contrib
list are also contributors to this repository.

## Special Credits
* **Mikko Bayabo**: Windows surface destructive testing
* **WSL testing**: Sameer Azer, Aaron Moffit, Ryan Lucus, Onyema Ude
