#!/bin/bash

short_help_var="-h"


if [ "$1" == "-h" ]; then

    echo "Usage: $0 [-c|--cong_cont 'congestion control'] [-d|--data_type 'type of data'] [-r|--rate 'bottleneck rate'] [-f|--flows 'number of flows']"
    exit 0
fi


if [ "$1" == "--help" ]; then

    echo "Usage: $0 [-c|--cong_cont 'congestion control'] [-d|--data_type 'type of data'] [-r|--rate 'bottleneck rate'] [-f|--flows 'number of flows']"
    exit 0
fi



i=2

graph_int=7

start_date=$(date | cut -b 1-19 | tr ' ' _ | tr : _)

VALID_ARGS=$(getopt -o c:d:r:f: --long cong_cont:,data_type:,rate:,flows: -- "$@")
if [[ $? -ne 0 ]]; then
    exit 1;
fi


if [[ $# -ne 8 ]]; then
   echo "You provided $# arguments, but we need 8. Use -h option."    
    exit 0;
fi


eval set -- "$VALID_ARGS"
while [ : ]; do
  case "$1" in
    -c | --cong_cont)
        #echo "Processing 'cong_cont' option. Input argument is '$2'"
        cong_cont_var=$2
        shift 2
        ;;
    -d | --data_type)
        #echo "Processing 'data_type' option. Input argument is '$2'"
        data_type_var=$2
        shift 2
        ;;
   -r | --rate)
        #echo "Processing 'rate' option. Input argument is '$2'"
        rate_var=$2
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



str_tcp_vegas="TcpVegas"

str_tcp_cubic="TcpCubic"

str_tcp_bbr="TcpBbr"

str_tcp_new_reno="TcpNewReno"

str_tcp_west_wood="TcpWestwoodPlus"




case $cong_cont_var in
    "TcpVegas"|"TcpCubic"|"TcpBbr"|"TcpNewReno"|"TcpWestwoodPlus")
        echo "Your congestion control is in!"
        ;;
    *)
        # error
        echo "Your congestion control is out!"
        exit 0
esac


if [ "$cong_cont_var" == "$str_tcp_vegas" ]; then
    echo "You choose vegas!"
    sudo sysctl -w net.ipv4.tcp_congestion_control=vegas

elif [ "$cong_cont_var" == "$str_tcp_cubic" ]; then
    echo "You choose Cubic"
    sudo sysctl -w net.ipv4.tcp_congestion_control=cubic

elif [ "$cong_cont_var" == "$str_tcp_bbr" ]; then
    echo "You choose Bbr"
    sudo sysctl -w net.ipv4.tcp_congestion_control=bbr

elif [ "$cong_cont_var" == "$str_tcp_new_reno" ]; then
    echo "You choose NewReno"
    sudo sysctl -w net.ipv4.tcp_congestion_control=reno

elif [ "$cong_cont_var" == "$str_tcp_west_wood" ]; then

    echo "You choose Westwood" 
    sudo sysctl -w net.ipv4.tcp_congestion_control=westwood


else
    echo "failed in set congestion control"
    exit 0

fi

sysctl net.ipv4.tcp_congestion_control





echo "Shell Parameters: ${cong_cont_var} ${data_type_var} ${rate_var} ${flows_var} ${date_var}"


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

    #O tcp client roda sem saver, pois as features serão posteriormente levantadas do dump, que é produzido pelo tshark
    #ao longo da simulção. Entretanto ele já vai criar o diretório da simulação. Por isso, os vários argv, como  no caso dos udt_clients
    #com a classe pure_udt, que reasliza todo o saver dentro dela.
    #/communicator tcp_client 9090 Vegas  Treino 100 1 Fri_Mar_7_04_12_25
    echo "App call: ./communicator tcp_client ${door} ${cong_cont_var} ${data_type_var} ${rate_var} ${flows_var} ${start_date}"
    xterm -hold -e "cd /home/ns/UDT-workspace/git-udt/vscode/shared/bin/Linux64/Debug; ./communicator tcp_client ${door} ${cong_cont_var} ${data_type_var} ${rate_var} ${flows_var} ${start_date}" && /bin/bash &
    sleep 0.5

    i=`expr $i + 1` 

done

