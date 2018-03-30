#!/bin/bash

for j in `ls tests/test_mbbsd/*`
do
    if [ -x ${j} ]
        then
        echo "${j}"
        k=`valgrind --leak-check=yes ${j} 2>&1`
        d_lost_bytes=`echo "${k}"|grep "definitely lost"|awk '{print $4}'`
        d_lost_blocks=`echo "${k}"|grep "definitely lost"|awk '{print $7}'`
        id_lost_bytes=`echo "${k}"|grep "indirectly lost"|awk '{print $4}'`
        id_lost_blocks=`echo "${k}"|grep "indirectly lost"|awk '{print $7}'`
        p_lost_bytes=`echo "${k}"|grep "possibly lost"|awk '{print $4}'`
        p_lost_blocks=`echo "${k}"|grep "possibly lost"|awk '{print $7}'`
        t_lost_bytes=`expr "${d_lost_bytes}" + "${id_lost_bytes}" + "${p_lost_bytes}"`
        t_lost_blocks=`expr "${d_lost_blocks}" + "${id_lost_blocks}" + "${p_lost_blocks}"`

        echo "lost bytes: ${t_lost_bytes} lost blocks: ${t_lost_blocks}"

        if [ "${t_lost_bytes}" != "0" ]
            then
            echo "${k}"
            echo -e "\e[41mERROR!\e[m"
            exit 255
        fi
    fi
done

echo -e "\e[42mAll Pass!\e[m"
