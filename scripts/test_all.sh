#!/bin/bash

for p in tests/test_common/test_sys tests/test_mbbsd
do
    for j in `ls ${p}/*`
    do
        if [ -x ${j} ]
            then
            echo "${j}:"
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
done

exit 0