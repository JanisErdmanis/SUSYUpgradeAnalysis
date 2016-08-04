#!/bin/bash

set -e
set -u

if test $# -ne 0
then
    echo invalid arguments for get_release: $* 1>&2
    exit 1
fi

$ROOTCOREDIR/scripts/get_field.sh "$ROOTCOREBIN/rootcore_config" release
