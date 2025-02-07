#!/bin/bash

i=0
num_clients=60

while [ $i -lt $num_clients ];
do

    echo $i
    door=`expr $i + 9000`
    echo $door
    #command="./udt server ${door}"
    #command="ls"
    #b=`$command`
    #echo $b
    #./udt server ${door}
    xterm -hold -e ./udt client ${door} && /bin/bash &
    sleep 15

    i=`expr $i + 1` 

done


