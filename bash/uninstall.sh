#!/bin/bash
script_directory="$(dirname "$(realpath "$0")")"
source "$script_directory/_core.sh"
echo "  > uninstalling startup bash command"
echo "  > removing [$desktop_install_path/$desktop_filename]"
rm $desktop_install_path/$desktop_filename
echo "  > removing [$HOME/.config/autostart/$desktop_filename]"
rm "$HOME/.config/autostart/$desktop_filename"


# if [ ! -f $install_location ]
# then
#     echo "  > [$install_location] file does not exist"
#     sleep 3
#     exit
# fi

# # Check if the command is present in .bash_profile
# if [[ $(grep "$run_command" $install_location) == $run_command ]]
# then
#     # Command is present, so remove it
#     converted_string=$(echo "$run_command" | sed -e 's/[$]/\\$/g' -e 's/[\/]/\\\//g')
#     sed -i "/$converted_string/d" $install_location
#     echo "  - command removed from [$install_location]"
#     echo "      - command converted_string was [$converted_string]"
# else
#     # Command is not present
#     echo "  > command not found in [$install_location]"
# fi
