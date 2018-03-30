#!/bin/bash

for j in `ls tests/test_mbbsd/*`
do
    if [ -x ${j} ]
        then
        k=`echo "${j}"|sed 's/.*\///g'`
        echo "${k}"
    fi
done
