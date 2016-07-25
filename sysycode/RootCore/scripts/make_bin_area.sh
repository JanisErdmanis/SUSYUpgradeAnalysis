#!/bin/bash

set -e
set -u

if test -z "${ROOTCOREDIR:+x}"
then
    set +e
    set +u
    source "`dirname $0 | xargs dirname`/local_setup.sh"
    set -e
    set -u
fi
exec rc make_bin_area "$@"
