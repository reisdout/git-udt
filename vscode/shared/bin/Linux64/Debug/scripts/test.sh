#!/bin/bash

cong_cont_var=$1
echo ${cong_cont_var}

if [ "$#" -ne 1 ]; then
    echo 'expecting one arguments' >&2
    exit 1
fi


str_tcp_vegas="TcpVegas"

str_tcp_cubic="TcpCubic"

str_tcp_bbr="TcpBbr"

str_tcp_new_reno="TcpNewReno"

str_tcp_west_wood="TcpWestwoodPlus"




case $cong_cont_var in
    "TcpVegas"|"TcpCubic"|"TcpBbr"|"TcpNewReno"|"TcpWestwoodPlus")
        echo "Your congestion control is in!"
        ;;
    *)
        # error
        echo "Your congestion control is out!"
        exit 0
esac


if [ "$cong_cont_var" == "$str_tcp_vegas" ]; then
    echo "You choose vegas!"
    sudo sysctl -w net.ipv4.tcp_congestion_control=vegas

elif [ "$cong_cont_var" == "$str_tcp_cubic" ]; then
    echo "You choose Cubic"
    sudo sysctl -w net.ipv4.tcp_congestion_control=cubic

elif [ "$cong_cont_var" == "$str_tcp_bbr" ]; then
    echo "You choose Bbr"
    sudo sysctl -w net.ipv4.tcp_congestion_control=bbr

elif [ "$cong_cont_var" == "$str_tcp_new_reno" ]; then
    echo "You choose NewReno"
    sudo sysctl -w net.ipv4.tcp_congestion_control=reno

elif [ "$cong_cont_var" == "$str_tcp_west_wood" ]; then

    echo "You choose Westwood" 
    sudo sysctl -w net.ipv4.tcp_congestion_control=westwood


else
    echo "failed in set congestion control"
    exit 0

fi

sysctl net.ipv4.tcp_congestion_control



