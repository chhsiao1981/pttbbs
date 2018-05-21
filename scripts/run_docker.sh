#!/bin/bash

if [ "${BASH_ARGC}" == "" ]
then
    MY_ARGC=0
else
    MY_ARGC=${BASH_ARGC}
fi

if [ "${MY_ARGC}" -gt "1" ]
then
   echo "usage: run_docker.sh [[tag]]"
   exit 255
fi

if [ "${MY_ARGC}" == "0" ]
then
    tag=`git branch|grep '*'|awk '{print $2}'`
else
    tag=${BASH_ARGV[0]}
fi

docker run --rm -itd -p 3000:3000 --name pttbbs pttbbs:${tag}
