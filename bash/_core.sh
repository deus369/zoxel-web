#!/bin/bash
project_directory=$(pwd)
script_location=$project_directory/bash/run_latest.sh # $HOME/zoxel/documents/zoxel_web/run_latest.sh
# install_location=$HOME/.profile #.bash_profile
install_location=$HOME/.bash_login
initial_wait_time=0 # 1
is_full_screen="--full-screen "
# is_full_screen=""
desktop_filename="zoxel_web.desktop"
desktop_install_path="$HOME/.local/share/applications"
# run_command="bash -c 'sleep $initial_wait_time; gnome-terminal $is_full_screen-q -- $script_location' &"
echo "  > running shell from [$script_location]"
echo "  > install location is [$install_location]"
# echo "  > bash command is [$run_command]"


# run_command="x-terminal-emulator -f -e '$script_location'"
# run_command="gnome-terminal -- $script_location"
# run_command="x-terminal-emulator -H -e sleep 12; gnome-terminal -- $script_location"
