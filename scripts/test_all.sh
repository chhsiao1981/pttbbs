#!/bin/bash

for j in `ls tests/test_mbbsd/*`
do
    if [ -x ${j} ]
        then
        echo "${j}"
        "${j}"
        ret="$?"
        if [ "${ret}" != "0" ]
            then
            echo "ERROR: ${ret}"
            exit 255
        fi
        echo ""
    fi
done

exit 0