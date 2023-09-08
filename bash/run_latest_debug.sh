#!/bin/bash
# run our zoxel_web

# sudo
# sudo cc -g -Wall main.c -o zoxel_web && sudo valgrind ./zoxel_web --log
cc -g -Wall src/main.c -o zoxel_web_dev && valgrind ./zoxel_web_dev --log

echo "Finished running server."

sleep 30