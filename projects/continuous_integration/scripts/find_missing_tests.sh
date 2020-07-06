#!/bin/bash

# Import SJSU-Dev2 common shell script functions
# Get base path
COMMON_SCRIPT_DIRECTORY=$(dirname "$0")
SJBASE=$(cd "${COMMON_SCRIPT_DIRECTORY}/../../../" ; pwd -P)

. ${COMMON_SCRIPT_DIRECTORY}/common.sh

missing_test_totals=0

unity_file_list=$(find ${SJBASE}/library/ -name "unity_test.cpp")
combined_tests=$(cat ${unity_file_list})
listed_tests=$(echo "${combined_tests}" | grep -oP "#include \K\"(.*)\"" | sort)

echo "$listed_tests"

testable_files=$(find ${SJBASE}/library/ -name "*.hpp" -o -name "*.cpp")
filtered_files=$(echo "${testable_files}" | grep -v "_test.cpp")

echo "$filtered_files"

exit 0

for source_file in "$@"; do
  # We only want to check library files
  echo ${source_file} | grep "/library/" &>/dev/null
  is_a_library=$?

  # Test files should be ignored as we do not need unit tests for our unit tests
  echo ${source_file} | grep "/test/" &>/dev/null
  is_a_test=$?

  if [[ "$is_a_library" != "0" ]]; then
    continue
  fi

  if [[ "$is_a_test" == "0" ]]; then
    continue
  fi

  base_name=$(basename "${source_file}" | cut -f 1 -d '.')
  directory=$(dirname ${source_file})
  test_file="${directory}/test/${base_name}_test.cpp"
  if [[ ! -f "${test_file}" ]]; then
    echo -e "${RED}Missing test:${RESET} ${test_file}"
    missing_test_totals=$(($missing_test_totals + 1))
  fi
done

if [[ ${missing_test_totals} -eq "0" ]]; then
  echo -e "${BOLD_GREEN}Everything clear! No files missing tests!${RESET}"
  exit 0
else
  echo
  echo -ne "Total number of missing tests: "
  echo -e "${BOLD_RED}${missing_test_totals}${RESET}"
  exit ${missing_test_totals}
fi
