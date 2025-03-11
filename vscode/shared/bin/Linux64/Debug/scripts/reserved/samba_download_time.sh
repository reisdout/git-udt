#!/bin/bash

    #command="./udt server ${door}"
    #command="ls"
    #b=`$command`
    #echo $b
    #./udt server ${door}
    #xterm -hold -e  ssh ns@10.0.1.3 ./udt server ${door} #&& /bin/bash &
    #'cd udt_workspace/git-udt/vscode/shared/bin/Linux64/Debug; ./udt server ${door}'
    
    # trocar '  por " paea ser avaliada antes de executar o comando
    echo "Starting download"
    start=`date +%s.%N`
    cp /run/user/1000/gvfs/smb-share:server=10.0.0.3,share=ns-share/$1 ~/$1
    #sleep 3
    end=`date +%s.%N`

    runtime=$(echo "$end - $start" |  bc -l)

    echo "$runtime"

    #i=`expr $i + 1` 




