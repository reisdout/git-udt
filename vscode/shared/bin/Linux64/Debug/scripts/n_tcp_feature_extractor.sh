#!/bin/bash


if [ "$1" == "-h" ]; then

    echo "Usage: $0 [-p|--path 'experiment path'] [-f|--flows 'number of flows']"
    exit 0
fi


if [ "$1" == "--help" ]; then

    echo echo "Usage: $0 [-p|--path 'experiment path'] [-f|--flows 'number of flows']"
    exit 0
fi



i=2
#num_clients=$1
last_terminal_tty=`expr $num_clients + 2`
graph_int=7
start_date=$(date | cut -b 1-19 | tr ' ' _ | tr : _)


VALID_ARGS=$(getopt -o p:,f: --long path:,flows: -- "$@")

if [[ $? -ne 0 ]]; then
    exit 1;
fi


if [[ $# -ne 4 ]]; then
   echo "You provided $# arguments, but we need 4. Use -h option."    
    exit 0;
fi


eval set -- "$VALID_ARGS"
while [ : ]; do
  case "$1" in
    -p | --path)
        #echo "Processing 'app' option"
        path_var=$2
        shift 2
        ;;

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
    echo "App call: ./communicator tcp_client ${door} ${path_var}"
    xterm -hold -e "cd /home/ns/UDT-workspace/git-udt/vscode/shared/bin/Linux64/Debug; ./communicator tcp_feature_saver_and_extractor ${door} ${path_var}" && /bin/bash &
    sleep 1

    i=`expr $i + 1` 

done


