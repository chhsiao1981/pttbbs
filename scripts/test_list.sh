#!/bin/bash

for p in tests/test_common/test_sys tests/test_mbbsd
do
    for j in `ls ${p}/*`
    do
        if [ -x ${j} ]
            then
            k=`echo "${j}"|sed 's/.*\///g'`
            echo "${k}"
        fi
    done
done
