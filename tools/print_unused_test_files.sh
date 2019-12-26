#!/bin/bash

# Get base path
SJBASE=$(dirname "$0")
SJBASE=$(
  cd "$SJBASE/.."
  pwd -P
)
# Import SJSU-Dev2 common shell script functions
. $SJBASE/tools/common.sh

missing_test_totals=0

for unused_test_file in "$@"; do
  echo -e "${RED}Missing from library test suite:${RESET} ${unused_test_file}"
  missing_test_totals=$(($missing_test_totals + 1))
done

if [[ ${missing_test_totals} -eq "0" ]]; then
  echo -e "${BOLD_GREEN}Everything clear! No files unused tests found!${RESET}"
  exit 0
else
  echo
  echo -ne "Total number of unused tests: "
  echo -e "${BOLD_RED}${missing_test_totals}${RESET}"
  exit ${missing_test_totals}
fi
