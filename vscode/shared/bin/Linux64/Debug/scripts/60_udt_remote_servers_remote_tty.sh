#!/bin/bash

i=0
num_clients=60

while [ $i -lt $num_clients ];
do

    echo $i
    tty=`expr $i + 2`
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
    #xterm -hold -e ssh ns@10.0.1.3  "cd udt_workspace/git-udt/vscode/shared/bin/Linux64/Debug; ./udt server ${door}" &
    sudo setsid -c /home/ns/UDT-workspace/git-udt/vscode/shared/bin/Linux64/Debug/udt server ${door} </dev/tty${tty} >/dev/tty${tty} 2>&1
    sudo chvt ${tty}
    sleep 2

    i=`expr $i + 1` 

done


