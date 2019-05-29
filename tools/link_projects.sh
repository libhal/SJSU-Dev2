#!/bin/bash

# Get base path
SJBASE=$(dirname "$0")
SJBASE=$(cd "$SJBASE/.." ; pwd -P)

PROJECTS=(\
  ${SJBASE}/projects/* \
  ${SJBASE}/demos/*/* )

for PROJECT in ${PROJECTS[@]}
do
    echo "Creating copy of project_makefile.mk for: $PROJECT/makefile"
    # Place env.sh link into project folder
    # Place makefile link into project folder
    rm -f "$PROJECT/makefile"
    cp "${SJBASE}/project_makefile.mk" "$PROJECT/makefile"
done