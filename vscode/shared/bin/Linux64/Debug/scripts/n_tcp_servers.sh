#!/bin/bash

if [ "$1" == "-h" ]; then

    echo "Usage: $0 [-f|--flows 'number of flows']"
    exit 0
fi


if [ "$1" == "--help" ]; then

    echo echo "Usage: $0 [-f|--flows 'number of flows']"
    exit 0
fi


i=2
num_clients=$1
last_terminal_tty=`expr $num_clients + 2`
graph_int=7


VALID_ARGS=$(getopt -o f: --long flows: -- "$@")

if [[ $? -ne 0 ]]; then #para saber se a ultima execucao saiu normalmente
    exit 1;
fi


if [[ $# -ne 2 ]]; then
   echo "You provided $# arguments, but we need 2. Use -h option."    
    exit 0;
fi


eval set -- "$VALID_ARGS"
while [ : ]; do
  case "$1" in

   -f | --flows)
        #echo "Processing 'flows' option. Input argument is '$2'"
        num_clients=$2
        last_terminal_tty=`expr $num_clients + 2` 
        flows_var=$2
        shift 2
        ;;

  

    --) shift; 
        break 
        ;;
  esac
done



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
    
    echo "App call: ./communicator /communicator tcp_server ${door}"
    xterm -hold -e  "cd /home/ns/UDT-workspace/git-udt/vscode/shared/bin/Linux64/Debug; ./communicator tcp_server ${door}" && /bin/bash &
    sleep 1

    i=`expr $i + 1` 

done


