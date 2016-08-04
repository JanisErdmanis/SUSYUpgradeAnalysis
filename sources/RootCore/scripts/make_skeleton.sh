#!/bin/bash

set -e
set -u

if test $# -ne 1
then
    echo invalid arguments for make_skeleton: $* 1>&2
    exit 1
fi
DIR=$1

test "x$DIR" = "x" && DIR=`pwd`

mkdir -p "$DIR/cmt"
mkdir -p "$DIR/`basename $DIR`"
mkdir -p "$DIR/Root"

MAKEFILE=$DIR/cmt/Makefile.RootCore
if test \! -f "$MAKEFILE"
then
    echo >>"$MAKEFILE" '# this makefile also gets parsed by shell scripts'
    echo >>"$MAKEFILE" '# therefore it does not support full make syntax and features'
    echo >>"$MAKEFILE" '# edit with care'
    echo >>"$MAKEFILE"
    echo >>"$MAKEFILE" '# for full documentation check:'
    echo >>"$MAKEFILE" '# https://twiki.cern.ch/twiki/bin/viewauth/Atlas/RootCore#Package_Makefile'
    echo >>"$MAKEFILE"
    echo >>"$MAKEFILE"
    echo >>"$MAKEFILE" '# the name of the package:'
    echo >>"$MAKEFILE" "PACKAGE              = `basename $DIR`"
    echo >>"$MAKEFILE"
    echo >>"$MAKEFILE" '# the libraries to link with this one:'
    echo >>"$MAKEFILE" 'PACKAGE_PRELOAD      = '
    echo >>"$MAKEFILE"
    echo >>"$MAKEFILE" '# additional compilation flags to pass (not propagated to dependent packages):'
    echo >>"$MAKEFILE" 'PACKAGE_CXXFLAGS     = '
    echo >>"$MAKEFILE"
    echo >>"$MAKEFILE" '# additional compilation flags to pass (propagated to dependent packages):'
    echo >>"$MAKEFILE" 'PACKAGE_OBJFLAGS     = '
    echo >>"$MAKEFILE"
    echo >>"$MAKEFILE" '# additional linker flags to pass (for compiling the library):'
    echo >>"$MAKEFILE" 'PACKAGE_LDFLAGS      = '
    echo >>"$MAKEFILE"
    echo >>"$MAKEFILE" '# additional linker flags to pass (for compiling binaries):'
    echo >>"$MAKEFILE" 'PACKAGE_BINFLAGS     = '
    echo >>"$MAKEFILE"
    echo >>"$MAKEFILE" '# additional linker flags to pass (propagated to client libraries):'
    echo >>"$MAKEFILE" 'PACKAGE_LIBFLAGS     = '
    echo >>"$MAKEFILE"
    echo >>"$MAKEFILE" '# the list of packages we depend on:'
    echo >>"$MAKEFILE" 'PACKAGE_DEP          = '
    echo >>"$MAKEFILE"
    echo >>"$MAKEFILE" '# the list of packages we use if present, but that we can work without :'
    echo >>"$MAKEFILE" 'PACKAGE_TRYDEP       = '
    echo >>"$MAKEFILE"
    echo >>"$MAKEFILE" '# list pattern of scripts to link directly into binary path:'
    echo >>"$MAKEFILE" 'PACKAGE_SCRIPTS      = '
    echo >>"$MAKEFILE"
    echo >>"$MAKEFILE" '# whether to use pedantic compilation:'
    echo >>"$MAKEFILE" 'PACKAGE_PEDANTIC     = 1'
    echo >>"$MAKEFILE"
    echo >>"$MAKEFILE" '# whether to turn *off* optimization (set to dict to do it only for'
    echo >>"$MAKEFILE" '# dictionaries):'
    echo >>"$MAKEFILE" 'PACKAGE_NOOPT        = 0'
    echo >>"$MAKEFILE"
    echo >>"$MAKEFILE" '# whether to build no library (needs to be set if no source files are'
    echo >>"$MAKEFILE" '# present):'
    echo >>"$MAKEFILE" 'PACKAGE_NOCC         = 0'
    echo >>"$MAKEFILE"
    echo >>"$MAKEFILE" '# whether we build a reflex dictionary:'
    echo >>"$MAKEFILE" 'PACKAGE_REFLEX       = 0'
    echo >>"$MAKEFILE"
    echo >>"$MAKEFILE" '# the list of all unit tests that should be called in recursive testing,'
    echo >>"$MAKEFILE" '# i.e. in unit tests that call other unit tests'
    echo >>"$MAKEFILE" '# for that unit tests need to pass on all machines, and run very fast'
    echo >>"$MAKEFILE" 'PACKAGE_RECURSIVE_UT = '
    echo >>"$MAKEFILE" ''
    echo >>"$MAKEFILE" ''
    echo >>"$MAKEFILE" ''
    echo >>"$MAKEFILE" 'include $(ROOTCOREDIR)/Makefile-common'
fi

LINKDEF=$DIR/Root/LinkDef.h
if test \! -f "$LINKDEF"
then
    echo >>"$LINKDEF" '#ifdef __CINT__'
    echo >>"$LINKDEF" ''
    echo >>"$LINKDEF" '#pragma link off all globals;'
    echo >>"$LINKDEF" '#pragma link off all classes;'
    echo >>"$LINKDEF" '#pragma link off all functions;'
    echo >>"$LINKDEF" '#pragma link C++ nestedclass;'
    echo >>"$LINKDEF" ''
    echo >>"$LINKDEF" '#endif'
fi
