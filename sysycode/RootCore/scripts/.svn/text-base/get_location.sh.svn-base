#!/bin/bash

set -e
set -u

# no argument check due to variable argument list

for pkg in "$@"
do
    rc --internal package_info "$pkg" srcdir
done
true
