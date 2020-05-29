#!/bin/sh
#  inst_local_dylib.sh  install dylib and replace path of dylib in program
#  usage: inst_local_dylib.sh program [dylib_path_keyword]
#    if omitted dylib_path_keyword, use '/usr/local'
#  (c)2018 Yoshikawa, Hiroyuki, FUJITSU LTD.
prog=$1
targ=$2
if [ x"$prog" == x ]; then
    echo "usage $0 prog [target]"
    exit 1
fi
if [ ! -f $prog ]; then
    echo "$0: $prog not found."
    exit 2
fi
progDir=`dirname $prog`

if [ x"$targ" == x ]; then
    targ=/usr/local
fi
targLibs=`otool -L $prog | grep $targ | awk '{print $1}'`
for l in $targLibs; do
    if [ ! -f $l ]; then
	echo "$0: $l not found."
	exit 3
    fi
    \cp -f $l $progDir
    lbase="@executable_path/`basename $l`"
    install_name_tool -change "$l" $lbase $prog
done
exit 0
