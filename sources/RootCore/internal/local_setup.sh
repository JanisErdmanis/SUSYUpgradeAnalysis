my_shell=$0
rootcore_setup="${BASH_SOURCE:-$0}"
rootcore_internal=`readlink $rootcore_setup | xargs dirname`
if test "x`echo $rootcore_internal | grep -v -E -e '^/'`" != "x"
then
    rootcore_internal="`dirname $rootcore_setup`/$rootcore_internal"
fi
eval `$rootcore_internal/env_setup --shell $my_shell $rootcore_setup`
