#!/bin/bash

set -e
set -u

if test $# -ne 1
then
    echo invalid arguments for $0: $* 1>&2
    exit 1
fi
pkg="$1"

if test `rc --internal package_info "$pkg" known` -eq 0
then
    echo unknown package $pkg 1>&2
    exit 1
fi
name="$pkg"
echo cleaning "$name"

rm -rf $ROOTCOREBIN/obj/$ROOTCORECONFIG/$name $ROOTCOREOBJ/$ROOTCORECONFIG/$name
true
