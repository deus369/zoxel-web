#!/bin/bash

# builds latest zoxel_web and runs it
#   assumed running from project folder directory

# cd $HOME/zoxel/documents/zoxel_web

# checks local ip
source bash/check_local_ip.sh

#sudo cc zoxel_web.c -o zoxel_web && sudo ./zoxel_web
cc src/zoxel_web.c -o zoxel_web && ./zoxel_web