#!/bin/bash

# build if latest updated website
cd ~/projects/zoxel-web
git pull && cc src/zoxel_web.c -o zoxel_web

if [[ -f zoxel_web ]]; then
    ./zoxel_web
fi

# Make sure
# builds latest zoxel_web and runs it
#   assumed running from project folder directory
# checks local ip
# source bash/check_local_ip.sh
# ssh-add ~/.ssh/zoxel