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
    echo  "sudo setsid -c /home/ns/UDT-workspace/git-udt/vscode/shared/bin/Linux64/Debug/udt client ${door} </dev/tty${i} >/dev/tty${i} 2>&1"
    sudo setsid -c /home/ns/UDT-workspace/git-udt/vscode/shared/bin/Linux64/Debug/udt client ${door} </dev/tty${i} >/dev/tty${i} 2>&1
    sudo chvt ${i}
    #sudo /home/ns/UDT-workspace/git-udt/vscode/shared/bin/Linux64/Debug/udt client ${door}
    #sudo ../udt client ${door}
    sleep 2

    i=`expr $i + 1` 

done

#sleep 10

