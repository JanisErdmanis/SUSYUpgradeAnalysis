#!/bin/bash

set -e
#set -u

if test \! -z "${ROOTCORECONFIG:+x}"
then
    MYCONFIG="$ROOTCORECONFIG"
fi

if test \! -z "${ROOTCOREDIR:+x}"
then
    source $ROOTCOREDIR/scripts/unsetup.sh
fi

cmd_submit=grid_submit.sh
cmd_compile=grid_compile.sh
cmd_run=grid_run.sh
noclean=0
releasetest=0
while test $# -gt 0
do
    if test "$1" == "--nobuild"
    then
	cmd_submit=grid_submit_nobuild.sh
	cmd_compile=""
	cmd_run=grid_run_nobuild.sh
	shift
    elif test "$1" == "--nocompile"
    then
	cmd_submit=grid_submit_nobuild.sh
	cmd_compile=grid_compile_nobuild.sh
	cmd_run=grid_run_nobuild.sh
	shift
    elif test "$1" == "--noclean"
    then
	noclean=1
	shift
    elif test "$1" == "--grid-release-test"
    then
	releasetest=1
	shift
    else
	break
    fi
done

if test $# -lt 2 -o $# -gt 3
then
    echo invalid arguments for grid_test: $* 1>&2
    exit 1
fi
rcdir=$1
testdir=$2
if test $# -eq 3
then
    script_file="$3"
else
    script_file="rc test_ut --recursive"
fi

if test $releasetest -eq 1
then
    export ROOTCORE_GRID_RELEASE_PREFIX=/
fi

if test \! -e "$rcdir/local_setup.sh"
then
    echo rootcore not found at $rcdir 1>&2
    exit 1
fi

dir_submit="$testdir/grid_submit"
dir_compile="$testdir/grid_compile"
dir_run="$testdir/grid_run"
for dir in "$dir_submit" "$dir_compile" "$dir_run"
do
    if test -e $dir
    then
	echo please remove $dir 1>&2
	echo \ \ rm -rf $dir 1>&2
	exit 1
    fi
done

if test \! -z "${MYCONFIG:+x}"
then
    export ROOTCORECONFIG="$MYCONFIG"
fi

echo running test submission
if (source $rcdir/local_setup.sh && \
    "$ROOTCOREDIR/scripts/"$cmd_submit "$dir_submit")
then
    true
else
    echo submit test failed 2>&1
    exit 1
fi

if test \! -z "${ROOTCORECONFIG:+x}"
then
    unset ROOTCORECONFIG
fi

mv "$dir_submit" "$dir_compile"

echo running test compile
if test \! -z "${cmd_compile:+x}"
then
    if (source "$dir_compile/RootCore/scripts/"$cmd_compile "$dir_compile")
    then
	true
    else
	echo compile test failed 2>&1
	exit 1
    fi
fi

mv "$dir_compile" "$dir_run"

echo running test run
echo "$dir_run/RootCore/scripts/"$cmd_run
cat "$dir_run/RootCore/scripts/"$cmd_run
if (source "$dir_run/RootCore/scripts/"$cmd_run "$dir_run" && \
    rm -f $ROOTCOREBIN/load_packages_success && \
    $script_file && \
    root -l -b -q $ROOTCOREDIR/scripts/load_packages.C && \
    test -f $ROOTCOREBIN/load_packages_success && \
    cat $ROOTCOREBIN/bin/$ROOTCORECONFIG/rc >/dev/null 2>/dev/null)
then
    true
else
    echo run test failed 2>&1
    exit 1
fi

if test $noclean -eq 0
then
    rm -rf "$dir_run"
fi

echo it appears this RootCore installation is grid compatible
