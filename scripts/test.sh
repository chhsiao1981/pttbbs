#!/bin/bash

if [ "${BASH_ARGC}" != "1" ]
    then
    echo "usage: test [test-name]"
    exit 255
fi

test_name=${BASH_ARGV[0]}

tests/test_mbbsd/${test_name}
