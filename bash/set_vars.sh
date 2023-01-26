#!/bin/bash

cd .. # go to project folder
project_directory=$(pwd)
script_location=$project_directory/bash/run_latest.sh # $HOME/zoxel/documents/zoxel_web/run_latest.sh
install_location=$HOME/.profile #.bash_profile
# run_command="x-terminal-emulator -f -e '$script_location'"
# run_command="gnome-terminal -- $script_location"
# run_command="x-terminal-emulator -H -e sleep 12; gnome-terminal -- $script_location"
run_command="bash -c 'sleep 6; gnome-terminal --full-screen -q -- $script_location' &"

echo "Location of script is: $script_location"