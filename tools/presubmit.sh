#!/bin/bash

STATUS_CAPTURE=0
BUILD_CAPTURE=0
LINT_CAPTURE=0
TIDY_CAPTURE=0
TEST_CAPTURE=0

if [ -z ${SJBASE} ]
then
    printf "\n"
    printf "\e[0;31m ========================================= \n"
    printf "\e[0;31m| SJSU-Dev2 environment variables not set |\n"
    printf "\e[0;31m|      PLEASE run 'source env.sh'         |\n"
    printf "\e[0;31m ========================================= \n"
    printf "\e[0m\n"
    exit 1
fi

function print_status
{
    if [ $1 -ne 0 ]
    then
        printf "\e[31m✘\e[0;31m"
    else
		printf "\e[32m✔\e[0;31m"
    fi
}

function check
{
    if [ $1 -ne 0 ]
    then
        lint="$(print_status $LINT_CAPTURE)"
        commit="$(print_status $STATUS_CAPTURE)"
        test="$(print_status $TEST_CAPTURE)"
        tidy="$(print_status $TIDY_CAPTURE)"
        build="$(print_status $BUILD_CAPTURE)"
        printf "\e[0;31m ================================ \e[0m\n"
        printf "\e[1;31m|        None of this!           |\e[0m\n"
        printf "\e[1;31m|                                |\e[0m\n"
        printf "\e[1;31m|        (╯°□ °)╯︵ ┻━┻          |\e[0m\n"
        printf "\e[1;31m|                                |\e[0m\n"
        printf "\e[1;31m|      Don't commit this         |\e[0m\n"
        printf "\e[0;31m ================================ \e[0m\n"
        printf "\e[0;31m|                                |\e[0m\n"
        printf "\e[0;31m| Code style must be lint free %b |\e[0m\n" $lint
        printf "\e[0;31m|    Code style must be tidy %b   |\e[0m\n" $tidy
        printf "\e[0;31m|     Commit must be clean %b     |\e[0m\n" $commit
        printf "\e[0;31m|       Tests must pass %b        |\e[0m\n" $test
        printf "\e[0;31m|       Code must build %b        |\e[0m\n" $build
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
        printf "\e[0;32m|      You may commit        |\e[0m\n"
        printf "\e[0;32m ============================ \e[0m\n"
        exit 0
    fi
}
####################################
#    Clean Git Repository Check    #
####################################
git diff-index --quiet HEAD --
STATUS_CAPTURE=$?
printf "\e[1;33m======================================================= \e[0m\n"
####################################
#    All Projects Build Check      #
####################################
# Change to the HelloWorld project
cd ${SJBASE}/firmware/HelloWorld
# Clean the build and start building from scratch
make clean
# Check if the system can build without any warnings!
make build WARNINGS_ARE_ERRORS=-Werror
# Set build capture to return code from the build
BUILD_CAPTURE=$?
# Build all example projects
cd ${SJBASE}/firmware/examples
for d in */; do
printf "\e[0;33m======================================================= \e[0m\n"
cd "${SJBASE}/firmware/examples/$d"
# Clean the build and start building from scratch
make clean
# Check if the system can build without any warnings!
make build WARNINGS_ARE_ERRORS=-Werror
# Add the return codes of the previous build capture. None zero means that at
# least one of the captures failed.
BUILD_CAPTURE=$(($BUILD_CAPTURE + $?))
done

cd ${SJBASE}/firmware/HelloWorld
####################################
#           Lint Check             #
####################################
printf "\e[1;33m======================================================= \e[0m\n"
make lint
LINT_CAPTURE=$?
printf "\e[1;33m======================================================= \e[0m\n"
####################################
#         Clang Tidy Check         #
####################################
make tidy
TIDY_CAPTURE=$?
printf "\e[1;33m======================================================= \e[0m\n"
####################################
#         Unit Test Check          #
####################################
make test -j8
TEST_CAPTURE=$?
# Check if there were any errors. For this to succeed, this value should be 0
check $(($STATUS_CAPTURE+$BUILD_CAPTURE+$LINT_CAPTURE+$TIDY_CAPTURE+$TEST_CAPTURE))
