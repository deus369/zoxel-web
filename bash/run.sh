#!/bin/bash

build_file="zoxel_web"

if [[ "$OSTYPE" == "msys" || "$OSTYPE" == "cygwin" ]]; then
    build_file="$build_file.exe"
fi

if [[ ! -f $build_file ]]; then
    echo "  > zoxel web not found"
    bash bash/build.sh
fi

if [[ -f $build_file ]]; then
    echo "  > running zoxel web"
    ./$build_file # sudo
else
    echo "  > could not run zoxel web"
fi
echo "  > finished running zoxel web"
