#! /bin/bash

cd ..
echo "Inside Directory [$PWD]"
ssh-add $HOME/.ssh/zoxel
echo "-> Getting modified or updated git files."
modified_and_new_files="$(git ls-files --modified --others --exclude-standard)"
if [ -z "$modified_and_new_files" ]; then
	echo ""
	echo "========================="
	echo "-> No files have been updated."
	echo ""
	echo ""
	echo "========================="
	echo ""
	sleep 6
	exit
else
	# echo "Files have been modified or added."
	echo ""
	echo "========================="
	echo ""
	echo "-> Modified:"
	git ls-files --modified
	echo ""
	echo "-> New:"
	git ls-files --others --exclude-standard
	echo ""
	echo "========================="
	echo ""
	# echo $modified_and_new_files
	sleep 1
fi

echo "-> Enter your commit message"
read commitmsg # get commit message off user
echo "	- Commit message is [$commitmsg]"
git add -A	# add all files to staged list
git commit -m "$commitmsg"	# create commit
echo "	- Created commit."
sleep 1
# finally push the git change
git push -u origin main # no longer master branch but main
echo Finished Git Push
sleep 16
