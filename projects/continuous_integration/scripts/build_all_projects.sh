#!/bin/bash

BUILD_CAPTURE=0

# Get base path
COMMON_SCRIPT_DIRECTORY=$(dirname "$0")
SJBASE=$(cd "${COMMON_SCRIPT_DIRECTORY}/../../../" ; pwd -P)

. ${COMMON_SCRIPT_DIRECTORY}/common.sh

####################################
#    All Projects Build Check      #
####################################
print_divider "Checking that all projects build"

printf "$YELLOW    Building hello_world Project $RESET"
# Change to the hello_world project
cd "$SJBASE/projects/hello_world"
# Purge repository of all application and framework build files and start
# building from scratch
SILENCE=$(make purge)
# Check if the system can build without any warnings!
SILENCE=$(make -s -j4 application WARNING_BECOME_ERRORS=-Werror)
# Set build capture to return code from the build
BUILD_CAPTURE=$?
print_status $BUILD_CAPTURE
echo ""

printf "$YELLOW    Building Starter Project $RESET"
# Change to the Hyperload project
cd "$SJBASE/projects/starter"
# Clean the build and start building from scratch
SILENCE=$(make clean)
# Check if the system can build without any warnings!
SILENCE=$(make -s -j4 application WARNING_BECOME_ERRORS=-Werror)
# Set build capture to return code from the build
SPECIFIC_BUILD_CAPTURE=$?
BUILD_CAPTURE=$(($BUILD_CAPTURE + $SPECIFIC_BUILD_CAPTURE))
print_status $SPECIFIC_BUILD_CAPTURE
echo ""

printf "$YELLOW    Building Barebones Project $RESET"
# Change to the Hyperload project
cd "$SJBASE/projects/barebones"
# Clean the build and start building from scratch
SILENCE=$(make clean)
# Check if the system can build without any warnings!
SILENCE=$(make -s -j4 application WARNING_BECOME_ERRORS=-Werror)
# Set build capture to return code from the build
SPECIFIC_BUILD_CAPTURE=$?
BUILD_CAPTURE=$(($BUILD_CAPTURE + $SPECIFIC_BUILD_CAPTURE))
print_status $SPECIFIC_BUILD_CAPTURE
echo ""

# Build all example projects
cd $SJBASE/demos/
# Get all demos with makefiles but ignore multiplatform projects
LIST_OF_PROJECT=$(find $SJBASE/demos/ -name "makefile" | \
                  grep -v "multiplatform")

for d in $LIST_OF_PROJECT
do
  PROJECT_PATH=$(dirname $d)
  cd "$PROJECT_PATH"
  printf "$YELLOW    Building Demo $PROJECT_PATH $RESET"
  # Clean the build and start building from scratch
  SILENCE=$(make clean)
  # Check if the system can build without any warnings!
  SILENCE=$(make -j4 application WARNING_BECOME_ERRORS=-Werror)
  # Add the return codes of the previous build capture. None zero means that at
  # least one of the captures failed.
  SPECIFIC_BUILD_CAPTURE=$?
  BUILD_CAPTURE=$(($BUILD_CAPTURE + $SPECIFIC_BUILD_CAPTURE))
  print_status $SPECIFIC_BUILD_CAPTURE
  echo ""
done

# Get all multiplatform projects
LIST_OF_PROJECT=$(find $SJBASE/demos/multiplatform -name "makefile")
LIST_OF_PLATFORMS=(lpc40xx lpc17xx stm32f4xx stm32f10x msp432p401r)

for d in $LIST_OF_PROJECT
do
  for p in "${LIST_OF_PLATFORMS[@]}"
  do
    PROJECT_PATH=$(dirname $d)
    cd "$PROJECT_PATH"
    printf "$YELLOW"
    printf "    Building Multiplatform Demo ($p)\n"
    printf "        $PROJECT_PATH"
    printf "$RESET "
    # Clean the build and start building from scratch
    SILENCE=$(make clean)
    # Check if the system can build without any warnings!
    SILENCE=$(make -j4 application PLATFORM=$p WARNING_BECOME_ERRORS=-Werror)
    # Add the return codes of the previous build capture. None zero means that
    # at least one of the captures failed.
    SPECIFIC_BUILD_CAPTURE=$?
    BUILD_CAPTURE=$(($BUILD_CAPTURE + $SPECIFIC_BUILD_CAPTURE))
    print_status $SPECIFIC_BUILD_CAPTURE
    echo ""
  done
done

exit $BUILD_CAPTURE
