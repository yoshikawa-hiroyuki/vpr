#!/bin/sh

INSTALL='install'
MAKE='make'
DST=../vpr_app

platform=`uname`
if [ ${platform} == 'Linux' ]; then
    TARGET=vpr_qslim.so
elif [ ${platform} == 'Darwin' ]; then
    TARGET=vpr_qslim.dylib
else
    echo "not supported platform."
    exit 1
fi

if [ ! -x ${TARGET} ]; then
	${MAKE} ${TARGET}
fi

${INSTALL} -d ${DST}
${INSTALL} ${TARGET} ${DST}

