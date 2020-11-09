# Quick Start Guide

Installation on Mac and Linux is easy! To get setup, run the commands below in
your terminal of choice:

```bash
git clone https://github.com/SJSU-Dev2/SJSU-Dev2.git
cd SJSU-Dev2
./setup
```

<script id="asciicast-314726" src="https://asciinema.org/a/314726.js" async>
</script>


To learn how to use SJSU-Dev2, please read the whole
[Introduction](introduction/getting_started_sjtwo.md) series of guides. You can
skip over content you already understand.

## Windows Setup

### Installing WSL for Windows 10 Users

!!! Warning
    You **MUST** update your laptop or computer to the latest version of
    Windows. This process is a bit annoying as Windows 10 has to be upgraded
    serially. Meaning that you cannot update from version 3 to version 10. You
    need to run update 7 times in order to reach the latest update, version 10.

Steps to updating windows:

1. Open the Settings menu and go to `Update & security > Windows Update`
2. Click **Check for updates** to prompt your PC to scan for the latest
    updates. The update will be downloaded and installed automatically.
3. Click **Restart Now** to restart your PC.
4. Now follow steps 1 to 3 until Windows tells you there are no more updates
    left.

With an up to date Windows 10 users must install WSL in order to work with the
development platform.

Follow this link and follow the steps:
[Install WSL on Windows 10](https://docs.microsoft.com/en-us/windows/wsl/install-win10).

### Installing Serial Device Driver for Windows 10

Windows 10 and Mac users will need to install the serial drivers on
their computer to be able to communicate with the SJTwo board. Follow
this link and download the driver for your computer.
[Link to Serial Driver Download](https://www.silabs.com/products/development-tools/software/usb-to-uart-bridge-vcp-drivers)

!!! Warning
    If you are using a Mac Book Pro 2018 and earlier, make sure to install the
    legacy driver from the link above. If you installed the latest driver and it
    did not work, run the uninstall script, and re-install the legacy VCP
    driver.
