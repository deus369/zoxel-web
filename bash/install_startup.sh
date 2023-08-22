#!/bin/bash

source bash/set_vars.sh
echo "  > installing startup bash command"

# # Check if the $install_location file exists
# if [ ! -f $install_location ]
# then
#     # $install_location file does not exist, so create it
#     touch $install_location
#     echo "  + created file [$install_location] "
# else
#     echo "  > file already exists [$install_location]"
# fi

# # Check if the command has already been added to $install_location
# if [[ $(grep "$run_command" $install_location) == $run_command ]]
# then
#     # Command already added
#     echo "  > command found in [$install_location]"
# else
#     # Command not added, so add it
#     echo $run_command >> $install_location
#     echo "  + command added to [$install_location]"
# fi

echo "  > copying [bash/$desktop_filename] to [$desktop_install_path/$desktop_filename]"

cp "bash/$desktop_filename" "$desktop_install_path/$desktop_filename"

echo "  > copying [bash/$desktop_filename] to [$HOME/.config/autostart/$desktop_filename]"
cp "bash/$desktop_filename" "$HOME/.config/autostart/$desktop_filename"