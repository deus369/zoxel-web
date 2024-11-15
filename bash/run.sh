#!/bin/bash
script_directory="$(dirname "$(realpath "$0")")"
project_directory="$script_directory/.."
cd $project_directory
build_file="$project_directory/zoxel_web"
if [[ "$OSTYPE" == "msys" || "$OSTYPE" == "cygwin" ]]; then
    build_file="$build_file.exe"
fi

while true; do
    git pull
    bash $script_directory/build.sh
    if [[ ! -f $build_file ]]; then
        echo "  > zoxel web not found"
        sleep 2
        continue
    fi
    # kill any previous
    echo " > running kill.sh"
    bash $script_directory/kill.sh
    # run zoxel-web service
    if [[ -f $build_file ]]; then
        echo "  > running zoxel web"
        $build_file -l
        # Check the exit status to determine if it crashed
        if [[ $? -ne 0 ]]; then
            echo "  > zoxel web crashed, restarting..."
        else
            echo "  > finished running zoxel web"
        fi
    else
        echo "  > could not run zoxel web"
        break  # Exit the loop if the build file still cannot be found
    fi
    # Optional: Add a delay to avoid a tight loop in case of frequent crashes
    sleep 2
done
