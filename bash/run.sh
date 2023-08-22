#!/bin/bash
if [[ ! -f zoxel_web ]]; then
    echo "  > zoxel web not found"
    bash bash/build.sh
fi
if [[ -f zoxel_web ]]; then
    echo "  > running zoxel web"
    ./zoxel_web # sudo
else
    echo "  > could not run zoxel web"
fi
echo "  > finished running zoxel web"