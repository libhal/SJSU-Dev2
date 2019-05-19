#!/bin/bash

# Get base path
SJBASE=$(dirname "$0")
SJBASE=$(cd "$SJBASE/.." ; pwd -P)

PROJECTS=(\
  ${SJBASE}/firmware/HelloWorld/ \
  ${SJBASE}/firmware/Hyperload/ \
  ${SJBASE}/firmware/examples/*/*/ )

for PROJECT in ${PROJECTS[@]}
do
    echo "Creating link for: $PROJECT/makefile"
    # Place env.sh link into project folder
    # Place makefile link into project folder
    rm -f "$PROJECT/makefile"
    cp "${SJBASE}/project_makefile.mk" "$PROJECT/makefile"
done