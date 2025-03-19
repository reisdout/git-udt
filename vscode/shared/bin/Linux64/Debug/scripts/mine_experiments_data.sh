#!/bin/bash

i=0
num_clients=60


start_date=$(date | cut -b 1-19 | tr ' ' _ | tr : _)


#iniciar queue monitor

xterm -hold -e ssh ns@10.0.0.1  "cd /tmp/ramdisk/queue_monitor; ./queue_monitor.sh >> ./out/queue_along_time_${start_date}.txt; sleep 5; pkill -f queue_monitor" &


: '
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
    #xterm -hold -e  ssh ns@10.0.1.3 ./udt server ${door} #&& /bin/bash &
    #'cd udt_workspace/git-udt/vscode/shared/bin/Linux64/Debug; ./udt server ${door}'
    
    # trocar '  por " paea ser avaliada antes de executar o comando
    xterm -hold -e ssh ns@10.0.1.3  "cd udt_workspace/git-udt/vscode/shared/bin/Linux64/Debug; ./udt server ${door}" &
    sleep 3

    i=`expr $i + 1` 

done

'
