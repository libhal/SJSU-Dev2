#!/bin/bash

# Import SJSU-Dev2 common shell script functions
# Get base path
COMMON_SCRIPT_DIRECTORY=$(dirname "$0")
SJBASE=$(cd "${COMMON_SCRIPT_DIRECTORY}/../../../" ; pwd -P)

. ${COMMON_SCRIPT_DIRECTORY}/common.sh

if [ ! -x "$(command -v cspell)" ]; then
  echo "'cspell' command is missing. Use the following command to install it."
  echo "Requires nodejs and NPM to be installed:"
  echo
  echo "    sudo npm install -g cspell"
  echo
  exit 2
fi

all_sources=""

for source_file in "$@"
do
  echo ${source_file} | grep "/demos/" &> /dev/null
  is_a_demo=$?
  echo ${source_file} | grep "/projects/" &> /dev/null
  is_a_project=$?
  echo ${source_file} | grep "/test/" &> /dev/null
  is_a_test=$?

  if [[ $is_a_demo -eq "0" ]]; then
    continue
  fi
  if [[ $is_a_project -eq "0" ]]; then
    continue
  fi
  if [[ $is_a_test -eq "0" ]]; then
    continue
  fi

  relative_path=$(realpath --relative-to="$SJBASE/" "${source_file}")
  all_sources="$all_sources $relative_path"
done

cspell --color --root $SJBASE --config $SJBASE/.cspell.json $all_sources
SPELLING_RETURN_CODE=$?
echo "Return code = $SPELLING_RETURN_CODE"
exit $SPELLING_RETURN_CODE
