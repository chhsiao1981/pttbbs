#!/bin/bash

if [ "${BASH_ARGC}" == "" ]
then
    MY_ARGC=0
else
    MY_ARGC=${BASH_ARGC}
fi

if [ "${MY_ARGC}" -gt "2" ]
then
    echo "usage: docker_build.sh [[password]] [[tag]]"
    exit 255
fi

if [ "${MY_ARGC}" -lt "2" ]
then
    tag=`git branch|grep '*'|awk '{print $2}'`
else
    tag=${BASH_ARGV[0]}
fi

if [ "${MY_ARGC}" == "1" ]
then
    password=${BASH_ARGV[0]}
elif [ "${MY_ARGC}" == "2" ]
then
    password=${BASH_ARGV[1]}
else
    password='[TO_BE_REPLACED]'
fi

sed "s/\[TO_BE_REPLACED\]/${password}/g" Dockerfile.tmpl > Dockerfile

docker build -t pttbbs:${tag} .
