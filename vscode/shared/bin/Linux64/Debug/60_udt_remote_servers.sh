#!/bin/bash

i=0
num_clients=1

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


