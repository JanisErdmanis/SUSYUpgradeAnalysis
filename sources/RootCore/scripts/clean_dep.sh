#!/bin/bash

set -e
set -u

if test $# -ne 0
then
    echo invalid arguments for clean_dep: $* 1>&2
    exit 1
fi

for pkg in `rc --internal package_list --local-obj`
do
    echo cleaning dependencies in $pkg
    rm -rf `rc --internal package_info "$pkg" objdir`/*.d
done
true
