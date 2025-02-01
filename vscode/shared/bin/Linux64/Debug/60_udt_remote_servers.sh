#!/bin/bash

i=0
num_clients=2

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
    xterm -hold -e ssh ns@10.0.1.3 &&./udt server ${door} & /bin/bash &
    sleep 6

    i=`expr $i + 1` 

done


