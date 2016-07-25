#!/bin/bash

set -e

url_list="$1"
file="$2"
md5sum="$3"

test "$url_list" == "" -o "$file" == "" && echo insufficient arguments for download.sh && exit 1

mkdir -p `dirname "$file"`
for url in $url_list
do
    if test \! -f "$file"
    then
	if test "${url:0:1}" == "/"
	then
	    echo copying $url
	    if cp "$url" "$file"
	    then
		true
	    else
		echo failed to copy
		rm -f "$file"
	    fi
	elif test "`which curl`" != ""
	then
 	    echo downloading $url
 	    if curl $url -o "$file"
 	    then
 		true
 	    else
 		echo failed to download $url
 		rm -f "$file"
 	    fi
	elif test "`which wget`" != ""
	then
 	    echo downloading $url
 	    if (cd `dirname "$file"` && wget $url)
 	    then
 		true
 	    else
 		echo failed to download $url
		rm -f "$file"
 	    fi
	else
 	    echo no tool for downloading file found
	fi
	if test "$md5sum" != "" -a "`which md5sum`" != ""
	then
	    localmd5=`md5sum "$file" | awk ' { print $1 } '`
	    if test "$md5sum" == "$localmd5"
	    then
		echo passed md5 checksum test
	    else
		echo md5 checksum failed, removing file
		rm -f "$file"
	    fi
	fi
    fi
done

if test \! -f "$file"
then
    echo failed to download file.  please download or copy from:
    echo "  $url_list"
    exit 1
fi
