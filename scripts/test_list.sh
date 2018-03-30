#!/bin/bash

for p in tests/test_common/test_sys tests/test_mbbsd
do
    echo -e "\e[32m${p}:\e[m"
    for j in `ls ${p}/*`
    do
        if [ -x ${j} ]
            then
            k=`echo "${j}"|sed 's/.*\///g'`
            echo "${k}"
        fi
    done
    echo ""
done
