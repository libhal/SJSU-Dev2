#!/bin/bash

if [ -z ${SJBASE} ]
then
    SOURCE_FILE_STRING='\e[1;31msource env.sh\e[0;31m'
    printf "\n"
    printf "\e[0;31m ========================================= \n"
    printf "\e[0;31m| SJSU-Dev2 environment variables not set |\n"
    printf "\e[0;31m|      PLEASE run '%b'         |\n" "$SOURCE_FILE_STRING"
    printf "\e[0;31m|                                         |\n"
    printf "\e[0;31m|  'env.sh' can be found at the root of   |\n"
    printf "\e[0;31m|       the SJSU-Dev2 Project Folder      |\n"
    printf "\e[0;31m|                                         |\n"
    printf "\e[0;31m|    If 'env.sh' doesn't exist run the    |\n"
    printf "\e[0;31m|    setup script in SJSU-Dev2 Project    |\n"
    printf "\e[0;31m ========================================= \n"
    printf "\e[0m\n"
    exit 1
fi

# DIR=$( cd "$( dirname "${BASH_SOURCE[0]}" )/../" && pwd )
PROJECTS=(${SJBASE}/firmware/HelloWorld/ ${SJBASE}/firmware/examples/*/)
for PROJECT in ${PROJECTS[@]}
do
    echo "Creating link for: $PROJECT/env.sh"
    echo "Creating link for: $PROJECT/makefile"
    # Place env.sh link into project folder
    rm -f "$PROJECT/env.sh"
    ln -s -f "${SJBASE}/env.sh" "$PROJECT/env.sh"
    # # # # Place makefile link into project folder
    rm -f "$PROJECT/makefile"
    ln -s -f "${SJBASE}/makefile" "$PROJECT/makefile"
done