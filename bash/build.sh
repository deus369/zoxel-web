#!/bin/bash
echo "  > building zoxel_web"

if [[ "$OSTYPE" == "msys" || "$OSTYPE" == "cygwin" ]]; then
    echo " + building on windows"
    gcc src/main.c -o zoxel_web -lws2_32
else
    echo " + building on linux"
    gcc src/main.c -o zoxel_web
fi

echo " + build complete"
