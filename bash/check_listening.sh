#!/bin/bash
echo "  > testing zoxel_web"
netstat -nato | findstr :8080
echo "  > if nothing above me, not running :)"
