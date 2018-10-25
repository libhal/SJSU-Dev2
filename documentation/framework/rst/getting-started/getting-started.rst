Getting Started
=================

Prerequisites For Windows Users
---------------------------------
**Note:** this is for Windows build 16215 or later

1. Open PowerShell as Administrator and run

    :code:`Enable-WindowsOptionalFeature -Online -FeatureName Microsoft-Windows-Subsystem-Linux`

2. Restart computer when prompted.

3. Open Windows Store and choose **Ubuntu** as Linux distro

    Ubuntu (https://www.microsoft.com/store/p/ubuntu/9nblggh4msv6)

4. Select "Get"

    If you get any errors go here:
	https://docs.microsoft.com/en-us/windows/wsl/install-win10

5. After download completes select “Launch”

    1. Create UNIX user account
    2. If you get any errors go here:
	https://docs.microsoft.com/en-us/windows/wsl/install-win10

After this point, you should be able to run :code:`bash` from the start menu.

Prerequisites
---------------
Need a running version of Ubuntu 16.04 LTS or above, or Mac OS X (OS X Yosemite
 and above), or Windows 10 with WSL installed.

Installation
-------------

**Step 0**
	Open a terminal and go to a directory where you want to download SJSU-Dev2.

**Step 0**
	Download the repository.
	Go to https://github.com/kammce/SJSU-Dev2 and click
	on the "Clone or download" drop down button.
	Click the "Download Zip" and download it to the folder you are in.
	Unzip the .zip file.

	You may also clone the repository if you already have Git installed.
	.. code-block:: bash

		git clone https://github.com/kammce/SJSU-Dev.git

**Step 1**
	Change directory into **SJSU-Dev2**

	.. code-block:: bash

		cd SJSU-Dev2

**Step 2**
	Run :code:`setup` script.

	.. code-block:: bash

		./setup

**Step 3**
	Edit the :code:`env.sh` script. Change the line :code:`SJDEV=/dev/ttyUSB0`
	to equal what you have on your system.

	**How to find your serial device on Ubuntu**

	You probably do not have to change anything.

	If no other devices are connected to your machine, then it will be
	:code:`/dev/ttyUSB0`. It is recommended to keep it at this value, because
	when you add more devices, it will increment to :code:`/dev/ttyUSB1`. Once
	you remove your devices and replace them, the value will reset.

	**How to find your serial device on Mac OS X**

		1. Remove the SJ-One from your computer if it is connected.
		2. List the files in the :code:`/dev` folder by running the following
			:code:`ls /dev/`.
		3. Plug it into your computer and run :code:`ls /dev/`.
		4. Observe the new file that was created.
		5. On mac, the path should look something like the following
			:code:`/dev/tty.cumodemfd1337`.
		6. If so, change the line in :code:`env.sh` to that file path from
			:code:`SJDEV=/dev/ttyUSB0` -> :code:`SJDEV=/dev/tty.`

	**How to find your serial device on Windows Linux Subsystem**

	On Windows it should be :code:`/dev/ttyS3`. Check your device manager to see
	what number COM device your device. The number after COM is the number after
	the **S** in the :code:`/dev/ttyS` path. That is your device. Replace the
	line :code:`SJDEV=/dev/ttyUSB0` -> :code:`SJDEV=/dev/ttyS`. For example, if,
	when you plug in the SJTwo board and COM5 appears, then you would want to
	change :code:`SJDEV=/dev/ttyUSB0` -> :code:`SJDEV=/dev/ttyS5`.

Building and Loading Hello World Application
----------------------------------------------

**Step 0**
	From the root of the repository

	.. code-block:: bash

		cd firmware/HelloWorld

**Step 1**
	Source the :code:`env.sh`. You only need to do this once for each terminal
	session. After sourcing, the necessary environment variables will be added
	to your shell.

	.. code-block:: bash

		source env.sh

**Step 2**
	Run :code:`make build` within the HelloWorld folder to compile it into a HEX
	file located in the :code:`build/binaries` folder.

	.. code-block:: bash

		make build

	.. note::
		use the :code:`make` by itself to get additional information on how to
		use the build script.

**Step 3**
	To load the hex file into your SJTwo, run the following make command:

	.. code-block:: bash

		make flash

	.. danger::
		As of right now, this doesn't work. Please use FlashMagic with wine,
		to program your device.

	.. note::
		If you run this command without first building, this command will build
		your project and then flash it. So you can skip the step above if you
		like.

	.. note::
		If the device port of your SJOne did not appear as the device set in
		your :code:`env.sh` file, then run
		:code:`make flash SJDEV=<PATH-TO-YOUR-PORT>` and that will change your
		port path for that instant.

**Step 4**
	To view serial output, and interact with the board, run the following make
	command:

	.. code-block:: bash

		make telemetry

	.. note::
		The interface should pop up in your default browser on launch, unless
		you are using Windows. If the browser doesn't open automatically enter
		this into your browsers address bar :code:`http://localhost:5001`.

**Step 5**
	Done!!

Creating your own Project
--------------------------
DO NOT MODIFY the HelloWorld project. Copy and rename HelloWorld to make a new
project.
