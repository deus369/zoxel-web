#!/bin/bash
ssh root@194.195.251.84
# after long
sleep 10
echo "enter commands:"
echo "nohup ./zoxel-web/run.sh &"
sleep 10
nohup ./zoxel-web/run.sh &
sleep 5
