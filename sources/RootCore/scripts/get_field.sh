#!/bin/bash

set -e
set -u

if test $# -ne 2
then
    echo invalid arguments for get_field: $* 1>&2
    exit 1
fi
file="$1"
nameraw="$2"
namequote=`echo "$nameraw" | sed 's/\//\\\\\\//g'`

if test \! -e $file
then
    echo file not found: $file 1>&2
    exit 1
fi
grep -E "^[ \t]*$nameraw[ \t]*=[ \t]*" "$file" | sed "s/^[ \t]*$namequote[ \t]*=[ \t]*//" | tail -n 1
true
