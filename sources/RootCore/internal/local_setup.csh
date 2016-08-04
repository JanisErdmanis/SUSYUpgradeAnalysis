#echo $_

set rootcore_setup = ` echo $_ | awk ' { print $2 } ' `
#echo rootcore_setup = $rootcore_setup

if ( "x$1" != "x" ) then
    set rootcore_setup = "$1"
endif
#echo rootcore_setup = $rootcore_setup

set rootcore_internal = `readlink $rootcore_setup | xargs dirname `
#echo rootcore_internal = $rootcore_internal

if ( "x`echo $rootcore_internal | grep -v -E -e '^/'`" != "x" ) then
    set rootcore_internal = "`dirname $rootcore_setup`/$rootcore_internal"
endif
#echo rootcore_internal = $rootcore_internal

eval `$rootcore_internal/env_setup $rootcore_setup`
