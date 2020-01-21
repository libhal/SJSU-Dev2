#!/bin/bash

# ==============================================================================
# This shell script contains the common set of functions used across shell
# scripts in SJSU-Dev2
# ==============================================================================

# Color text
BLACK="\e[0;30m"
RED="\e[0;31m"
GREEN="\e[0;32m"
YELLOW="\e[0;33m"
BLUE="\e[0;34m"
PURPLE="\e[0;35m"
CYAN="\e[0;36m"
WHITE="\e[0;37m"
# Bold Color text
BOLD_BLACK="\e[1;30m"
BOLD_RED="\e[1;31m"
BOLD_GREEN="\e[1;32m"
BOLD_YELLOW="\e[1;33m"
BOLD_BLUE="\e[1;34m"
BOLD_PURPLE="\e[1;35m"
BOLD_CYAN="\e[1;36m"
BOLD_WHITE="\e[1;37m"
# Reset
RESET="\e[0m"

function print_divider
{
  printf "$BOLD_YELLOW"
  printf "_____________________________________________________________________"
  printf "\n\n"
  printf "$BOLD_WHITE $1\n"
  printf "$BOLD_YELLOW"
  printf "_____________________________________________________________________"
  printf "\n"
  printf "$RESET\n"
}

function print_status
{
  if [ $1 -ne 0 ]
  then
    printf "$BOLD_RED"
    printf "✘"
  else
    printf "$BOLD_GREEN"
    printf "✔"
  fi
  printf "$RED"
}
