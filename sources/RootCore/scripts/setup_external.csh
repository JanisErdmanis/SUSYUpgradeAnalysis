echo X
echo $_
echo X
set rootcore_setup = ` echo $_ | awk ' { print $2 } ' `
echo rootcore_setup = $rootcore_setup
set rootcore_dir = "`dirname $rootcore_setup | xargs dirname`"
echo rootcore_dir = $rootcore_dir
eval `$rootcore_dir/internal/env_setup $rootcore_setup`
unset rootcore_setup
unset rootcore_dir
