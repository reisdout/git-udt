#!/bin/bash

i=2
num_clients=60
last_terminal_tty=`expr $num_clients + 2`
graph_int=7

while [ $i -le $last_terminal_tty ];
do

    if [ $i -eq $graph_int ];  #pulando a interface gráfica
        then
            echo "Skiping GUI" 
            i=`expr $i + 1`    
            continue
    fi

    echo $i
    door=`expr $i + 9000`
    echo $door
    #command="./udt server ${door}"
    #command="ls"
    #b=`$command`
    #echo $b
    #./udt server ${door}
    xterm -hold -e "cd /home/ns/UDT-workspace/git-udt/vscode/shared/bin/Linux64/Debug; ./communicator udt_client ${door}" && /bin/bash &
    sleep 7

    i=`expr $i + 1` 

done


