#!/bin/bash
i=2
num_clients=$1
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
    #xterm -hold -e  ssh ns@10.0.1.3 ./udt server ${door} #&& /bin/bash &
    #'cd udt_workspace/git-udt/vscode/shared/bin/Linux64/Debug; ./udt server ${door}'
    
    # trocar '  por " paea ser avaliada antes de executar o comando
    xterm -hold -e  "cd /home/ns/UDT-workspace/git-udt/vscode/shared/bin/Linux64/Debug; ./communicator udt_server ${door}" && /bin/bash &
    sleep 3

    i=`expr $i + 1` 

done


