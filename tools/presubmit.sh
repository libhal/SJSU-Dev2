#!/bin/bash

STATUS_CAPTURE=0
BUILD_CAPTURE=0
LINT_CAPTURE=0
TEST_CAPTURE=0

if [ -z ${SJBASE} ]
then
    printf "\n"
    printf "\e[0;31m ======================================= \n"
    printf "\e[0;31m| SJSUOne environment variables not set |\n"
    printf "\e[0;31m|      PLEASE run 'source env.sh'       |\n"
    printf "\e[0;31m ======================================= \n"
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
        build="$(print_status $BUILD_CAPTURE)"
        printf "\e[0;31m ============================ \e[0m\n"
        printf "\e[1;31m|        None of this!       |\e[0m\n"
        printf "\e[1;31m|                            |\e[0m\n"
        printf "\e[1;31m|        (╯°□ °)╯︵ ┻━┻      |\e[0m\n"
        printf "\e[1;31m|                            |\e[0m\n"
        printf "\e[1;31m|      Don't commit this     |\e[0m\n"
        printf "\e[0;31m ============================ \e[0m\n"
        printf "\e[0;31m|                            |\e[0m\n"
        printf "\e[0;31m| Code style must conform %b  |\e[0m\n" $lint
        printf "\e[0;31m|   Commit must be clean %b   |\e[0m\n" $commit
        printf "\e[0;31m|     Tests must pass %b      |\e[0m\n" $test
        printf "\e[0;31m|     Code must build %b      |\e[0m\n" $build
        printf "\e[0;31m|                            |\e[0m\n"
        printf "\e[0;31m ============================ \e[0m\n"
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

# Change to the HelloWorld basis project
cd ${SJBASE}/firmware/HelloWorld
git status | grep "nothing to commit, working tree clean" &> /dev/null
STATUS_CAPTURE=$?
git status
printf "\e[0;33m======================================================= \e[0m\n"
# Clean the build and start building from scratch
make clean
# Check if the system can build
make build
BUILD_CAPTURE=$?
printf "\e[0;33m======================================================= \e[0m\n"
make lint
LINT_CAPTURE=$?
printf "\e[0;33m======================================================= \e[0m\n"
make test
TEST_CAPTURE=$?
# Check if there were any errors. For this to succeed, this value should be 0
check $((STATUS_CAPTURE+BUILD_CAPTURE+LINT_CAPTURE+TEST_CAPTURE))
