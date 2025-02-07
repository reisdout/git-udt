#!/bin/bash

i=0
num_clients=60

while [ $i -lt $num_clients ];
do
    tty=`expr $i + 2`
    echo $i
    door=`expr $i + 9000`
    echo $door
    #command="./udt server ${door}"
    #command="ls"
    #b=`$command`
    #echo $b
    #./udt server ${door}
    echo  "sudo setsid -c /home/ns/UDT-workspace/git-udt/vscode/shared/bin/Linux64/Debug/udt client ${door} </dev/tty${tty} >/dev/tty${tty} 2>&1"
    sudo setsid -c /home/ns/UDT-workspace/git-udt/vscode/shared/bin/Linux64/Debug/udt client ${door} </dev/tty${tty} >/dev/tty${tty} 2>&1
    sudo chvt ${tty}
    #sudo /home/ns/UDT-workspace/git-udt/vscode/shared/bin/Linux64/Debug/udt client ${door}
    #sudo ../udt client ${door}
    sleep 2

    i=`expr $i + 1` 

done

#sleep 10

