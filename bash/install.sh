#!/bin/bash
# bash -ic /home/deus/projects/zoxel-web/bash/run_latest.sh
source bash/set_vars.sh
desktop_filepath="$PWD/bash/$desktop_filename"
run_script_path="$PWD/bash/run_latest.sh"

echo "  > installing startup bash command"
echo " > new run path is [$run_script_path]"
echo " > new run path is [$run_script_path]"
echo "  > copying [$desktop_filepath] to [$desktop_install_path/$desktop_filename]"
echo "  > copying [$desktop_filepath] to [$HOME/.config/autostart/$desktop_filename]"

sed -i 's|^Exec=.*|Exec=gnome-terminal --full-screen -- '"$run_script_path"'|' "$desktop_filepath"
cp "$desktop_filepath" "$desktop_install_path/$desktop_filename"
cp "$desktop_filepath" "$HOME/.config/autostart/$desktop_filename"

# add  --full-screen to desktop file

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
