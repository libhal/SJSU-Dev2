#!/bin/bash

BUILD_CAPTURE=0
LINT_CAPTURE=0
TIDY_CAPTURE=0
TEST_CAPTURE=0

# Get base path
SJBASE=$(dirname "$0")
SJBASE=$(cd "$SJBASE/.." ; pwd -P)

. $SJBASE/tools/common.sh

function check
{
  if [ $1 -ne 0 ]
    then
    lint="$(print_status $LINT_CAPTURE)"
    tidy="$(print_status $TIDY_CAPTURE)"
    test="$(print_status $TEST_CAPTURE)"
    build="$(print_status $BUILD_CAPTURE)"
    printf "\e[0;31m ================================ \e[0m\n"
    printf "\e[1;31m|        None of this!           |\e[0m\n"
    printf "\e[1;31m|                                |\e[0m\n"
    printf "\e[1;31m|        (╯°□ °)╯︵ ┻━┻          |\e[0m\n"
    printf "\e[1;31m|                                |\e[0m\n"
    printf "\e[1;31m|      Don't even PUSH!          |\e[0m\n"
    printf "\e[0;31m ================================ \e[0m\n"
    printf "\e[0;31m|                                |\e[0m\n"
    printf "\e[0;31m| Code must be lint free ..... %b |\e[0m\n" $lint
    printf "\e[0;31m| Code must be tidy .......... %b |\e[0m\n" $tidy
    printf "\e[0;31m| Tests must pass ............ %b |\e[0m\n" $test
    printf "\e[0;31m| Code must build ............ %b |\e[0m\n" $build
    printf "\e[0;31m|                                |\e[0m\n"
    printf "\e[0;31m ================================ \e[0m\n"
    exit 1
  else
    printf "\e[0;32m ============================ \e[0m\n"
    printf "\e[0;32m| Everything looks good here |\e[0m\n"
    printf "\e[0;32m|                            |\e[0m\n"
    printf "\e[0;32m|           (•_•)            |\e[0m\r"
    sleep .5
    printf "\e[0;32m|           ( •_•)>⌐▪-▪      |\e[0m\r"
    sleep .5
    printf "\e[0;32m|           (⌐▪_▪)           |\e[0m\n"
    sleep .5
    printf "\e[0;32m|                            |\e[0m\n"
    printf "\e[0;32m|       You may commit       |\e[0m\n"
    printf "\e[0;32m ============================ \e[0m\n"
    exit 0
  fi
}
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
SILENCE=$(make -s application WARNINGS_ARE_ERRORS=-Werror)
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
SILENCE=$(make -s application)
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
SILENCE=$(make -s application)
# Set build capture to return code from the build
SPECIFIC_BUILD_CAPTURE=$?
BUILD_CAPTURE=$(($BUILD_CAPTURE + $SPECIFIC_BUILD_CAPTURE))
print_status $SPECIFIC_BUILD_CAPTURE
echo ""

# Build all example projects
cd $SJBASE/demos/

LIST_OF_PROJECT=$(find ./ -name "makefile")
for d in $LIST_OF_PROJECT
do
PROJECT_PATH=$(dirname $d)
cd "$SJBASE/demos/$PROJECT_PATH"
printf "$YELLOW    Building Example $d $RESET"
# Clean the build and start building from scratch
SILENCE=$(make clean)
# Check if the system can build without any warnings!
SILENCE=$(make application WARNINGS_ARE_ERRORS=-Werror)
# Add the return codes of the previous build capture. None zero means that at
# least one of the captures failed.
SPECIFIC_BUILD_CAPTURE=$?
BUILD_CAPTURE=$(($BUILD_CAPTURE + $SPECIFIC_BUILD_CAPTURE))
print_status $SPECIFIC_BUILD_CAPTURE
echo ""
done

# Return to home project
cd $SJBASE/projects/hello_world
####################################
#           Lint Check             #
####################################
print_divider "Executing 'lint' check"

make -s lint 1> /dev/null
LINT_CAPTURE=$?
print_status $LINT_CAPTURE
echo ""
####################################
#         Clang Tidy Check         #
####################################
print_divider "Executing 'tidy' check"

make -s tidy
TIDY_CAPTURE=$?
print_status $TIDY_CAPTURE
echo ""
####################################
#         Unit Test Check          #
####################################
print_divider "Building and running unit tests"

make -s library-test WARNINGS_ARE_ERRORS=-Werror
TEST_CAPTURE=$?
print_status $TEST_CAPTURE
echo ""

# Check if there were any errors. For this to succeed, this value should be 0
check $(($BUILD_CAPTURE+$LINT_CAPTURE+$TIDY_CAPTURE+$TEST_CAPTURE))
