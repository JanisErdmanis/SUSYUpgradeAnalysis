#!/bin/bash

set -e
set -u

# no argument check, since we have a variable argument list

dep_list=""
for pkg in $*
do
    for dep in `rc --internal package_info "$pkg" dep`
    do
	used=0
	for mydep in $dep_list
	do
	    if test "$mydep" == "$dep"
	    then
		used=1
	    fi
	done
	if test $used -eq 0
	then
	    dep_list="$dep_list $dep"
	fi
    done
done
echo $dep_list
