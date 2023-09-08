#!/bin/bash
# run our zoxel_web

# sudo cc -g -Wall main.c -o zoxel_web && sudo valgrind ./zoxel_web --log

# open http://192.168.0.107:8080
echo "Open http://192.168.0.107:8080"
sleep 6

cc -g -Wall src/main.c -o zoxel_web_dev && ./zoxel_web_dev  # --log

echo "Finished running server."

sleep 30