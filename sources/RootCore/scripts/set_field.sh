#!/bin/bash

set -e
set -u

if test $# -ne 3
then
    echo invalid arguments for set_field: $* 1>&2
    exit 1
fi
file="$1"
tmpfile="$file-"
nameraw="$2"
namequote=`echo "$nameraw" | sed 's/\//\\\\\\//g'`
valueraw="$3"
valuequote=`echo "$valueraw" | sed 's/\//\\\\\\//g'`

if test \! -e $file
then
    echo file not found: $file 1>&2
    exit 1
fi
if test -e $tmpfile
then
    echo file is in the way $tmpfile 1>&2
    exit 1
fi

line="`grep -E "^[ \t]*$nameraw[ \t]*=" $file | tail -n 1`"
if test "$line" != ""
then
    oldvalue="`echo $line | sed "s/^[ \t]*$namequote[ \t]*=[ \t]*//"`"
    if test "$oldvalue" == "$valueraw"
    then
	exit 0
    fi
    cat $file | sed "s/^[ \t]*$namequote[ \t]*=.*/$namequote = $valuequote/" >$tmpfile
else
    echo "$nameraw = $valueraw" >$tmpfile
    cat $file >>$tmpfile
fi

mv -f $tmpfile $file
