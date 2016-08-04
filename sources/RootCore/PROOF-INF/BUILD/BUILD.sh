#!/bin/bash

test "x$ROOTCOREDIR" != "x" && source $ROOTCOREDIR/scripts/unsetup.sh

if [ "$1" == "clean" ]
then
    source RootCoreBin/local_setup.sh
    exec rc clean
fi

source RootCore/scripts/grid_compile.sh
