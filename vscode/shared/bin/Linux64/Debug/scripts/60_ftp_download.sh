#!/bin/bash

i=2
num_clients=1
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
    xterm -hold -e "wget --user=ftpuser --password=10203087  -O /home/ns/video${door}.mp4 ftp://10.0.0.3/01.mp4 " && /bin/bash &    


    range=5   
    #sleep_time=$RANDOM
    #sleep_time=`expr $sleep_time % 5`
    #echo ">>>> ${sleep_time}"  
    #sleep_time=7
    sleep 5

    i=`expr $i + 1` 

done


