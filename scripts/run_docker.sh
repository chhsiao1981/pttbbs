#!/bin/bash

if [ "$#" -gt 1 ]
then
    echo "usage: run_docker.sh [[tag]]"
    exit 255
fi

tag=`git branch|grep '*'|awk '{print $2}'`

if [ "$#" -ge 1 ]
then
    tag=$1
fi

docker run --rm -itd -p 3000:3000 --name pttbbs pttbbs:${tag}
