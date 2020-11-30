#!/bin/bash

DEVICE_GDB=$1
GDBINIT_PATH=$2
PLATFORM=$3
EXECUTABLE=$4
OPENOCD=$5
DEBUG_DEVICE=$6
OPENOCD_CONFIG=$7

GDB_ARGS=""

if [ $PLATFORM == "linux" ]
then
  $DEVICE_GDB $EXECUTABLE -ex "source $GDBINIT_PATH"
else # For all other platforms
  $OPENOCD/bin/openocd  -s $OPENOCD/scripts/ \
    -c "source [find interface/$DEBUG_DEVICE.cfg]" -f $OPENOCD_CONFIG &

  # Capture the pid of the background OpenOCD process
  OPENOCD_PID=$(echo $!)
  # Wait for OpenOCD to continue or quit
  sleep 2
  # Use kill to check if OpenOCD is running
  kill -0 $OPENOCD_PID &> /dev/null
  # Capture success or failure of check above
  OPENOCD_IS_RUNNING=$?
  if [ $OPENOCD_IS_RUNNING -ne 0 ]
  then
    RED="\x1B[31;1m"
    RESET="\x1B[0m"
    echo
    echo -e "$RED OpenOCD failed to start, exiting $RESET"
    echo
    exit 1
  fi
  # When the GDB session closes, kill openocd below
  $DEVICE_GDB $EXECUTABLE \
      -ex "source $GDBINIT_PATH" \
      -ex "target remote :3333" \
      -ex "monitor arm semihosting enable" \
      $GDB_ARGS

  echo "Killing OpenOCD PID: $OPENOCD_PID"
  kill $OPENOCD_PID
fi
