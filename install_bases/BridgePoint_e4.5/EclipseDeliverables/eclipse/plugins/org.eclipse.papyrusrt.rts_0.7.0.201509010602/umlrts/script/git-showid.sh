#!/bin/sh

mods=`git status --porcelain | wc -c`
if [ $mods -ne 0 ]; then
	extra=+
fi

git show -s --pretty=format:"%h$extra"

