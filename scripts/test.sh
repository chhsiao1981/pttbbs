#!/bin/bash

if [ "${BASH_ARGC}" != "1" ]
    then
    echo "usage: test [test-name]"
    exit 255
fi

test_name=${BASH_ARGV[0]}

for p in tests/test_common/test_sys tests/test_mbbsd
do
    if [ -x "${p}/${test_name}" ]
        then
        echo -e "\e[33m${p}/${test_name}:\e[m"
        "${p}/${test_name}"
        echo ""
    fi
done
