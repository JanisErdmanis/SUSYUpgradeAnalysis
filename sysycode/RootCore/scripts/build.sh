#!/bin/bash

set -e
set -u

manage_opt=""
update=0
if test $# -gt 1
then
    echo invalid argments for build: $* 1>&2
    exit 1
elif test $# -eq 1
then
    manage_opt="--list $1"
    update=1
fi

if test -z "${ROOTCOREDIR:+x}"
then
    dir=$0
    if test "`echo $dir | grep ^/`" = ""
    then
	dir=`pwd`/$dir
    fi
    while test "`echo $dir | grep '/\./'`" != ""
    do
	dir=`echo $dir | sed 's/\/\.\//\//'`
    done
    dir=`dirname $dir | xargs dirname`
    (cd $dir && ./configure) || exit $?
    source $dir/scripts/setup.sh || exit $?
fi

if test $update -ne 0
then
    rc --internal manage_all $manage_opt build || exit $?
fi

$ROOTCOREDIR/scripts/find_packages.sh || exit $?
if test $update -ne 0
then
    # if we updated from SVN the safest thing to do is to remove all
    # dependency files in order to avoid problems with disappearing
    # files.
    $ROOTCOREDIR/scripts/clean_dep.sh || exit $?
fi
rc compile || exit $?
$ROOTCOREDIR/scripts/make_par.sh || exit $?
