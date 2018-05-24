#!/bin/bash

if [ "{BASH_ARGC}" != "" -a "${BASH_ARGC}" -gt "2" ]
then
    echo "usage: docker_build.sh [[password]] [[tag]]"
    exit 255
fi

password='[TO_BE_REPLACED]'
tag=`git branch|grep '*'|awk '{print $2}'`

if [ "${BASH_ARGC}" == "1" ]
then
    password=${BASH_ARGV[0]}
elif [ "${BASH_ARGC}" == "2" ]
then
    password=${BASH_ARGV[1]}
    tag=${BASH_ARGV[0]}
fi

dockerfile="dockerfiles/Dockerfile.default"

sed -i "s/\[TO_BE_REPLACED\]/${password}/g" ${dockerfile}

docker build -t pttbbs:${tag} -f ${dockerfile} .

git check -- ${dockerfile}
