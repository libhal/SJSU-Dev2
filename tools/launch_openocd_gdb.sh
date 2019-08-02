#!/bin/bash

OPENOCD=$1
DEBUG_DEVICE=$2
OPENOCD_CONFIG=$3
DEVICE_GDB=$4
EXECUTABLE=$5

## Uncomment the line below to get a bit of debug information

# echo "$OPENOCD/bin/openocd  -s $OPENOCD/scripts/ \
#-c \"source [find interface/$DEBUG_DEVICE.cfg]\" -f $OPENOCD_CONFIG &"

$OPENOCD/bin/openocd  -s $OPENOCD/scripts/ \
-c "source [find interface/$DEBUG_DEVICE.cfg]" -f $OPENOCD_CONFIG &

# Capture the pid of the background OpenOCD process
OPENOCD_PID=$(echo $!)
# Wait for OpenOCD to continue or quit
sleep .25
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

$DEVICE_GDB -ex "target remote :3333" $EXECUTABLE

echo "Killing OpenOCD PID: $OPENOCD_PID"
kill $OPENOCD_PID
