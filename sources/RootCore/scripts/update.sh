#!/bin/bash

set -e
set -u

opt=""
if test $# -gt 1
then
    echo invalid arguments for update: $* 1>&2
    exit 1
elif test $# -eq 1
then
    opt="$opt --list $1"
fi

rc --internal manage_all $opt update
