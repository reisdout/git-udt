#!/bin/bash

i=2
num_clients=60
last_terminal_tty=`expr $num_clients + 2`
graph_int=7

RANDOM=$$

while [ $i -le $last_terminal_tty ];
do

    if [ $i -eq $graph_int ];  #pulando a interface gráfica
        then
            echo "Skiping GUI" 
            i=`expr $i + 1`    
            continue
    fi

    echo $i
    door=`expr $i + 5000`
    echo $door
    #command="./udt server ${door}"
    #command="ls"
    #b=`$command`
    #echo $b
    #./udt server ${door}
    xterm -hold -e "iperf -c 10.0.1.3 -p ${door}" && /bin/bash &    


    range=5   
    sleep_time=$RANDOM
    sleep_time=`expr $sleep_time % 5`
    echo ">>>> ${sleep_time}"  
    #sleep_time=7
    sleep ${sleep_time}

    i=`expr $i + 1` 

done


