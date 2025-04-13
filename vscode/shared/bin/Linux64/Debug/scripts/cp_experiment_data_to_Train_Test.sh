#!/bin/bash

#hold_var=" "
hold_var="-hold"

if [ "$1" == "-h" ]; then

    echo "Usage: $0 [-c|--cong_cont 'congestion control'] [-d|--data_type 'type of data'] [-r|--rate 'bottleneck rate'] [-f|--flows 'number of flows']"
    exit 0
fi


if [ "$1" == "--help" ]; then

    echo echo "Usage: $0 [-c|--cong_cont 'congestion control'] [-d|--data_type 'type of data'] [-r|--rate 'bottleneck rate'] [-f|--flows 'number of flows']"
    exit 0
fi





i=1
#num_clients=$1
last_terminal_tty=`expr $num_clients + 2`
graph_int=7
start_date=$(date | cut -b 1-19 | tr ' ' _ | tr : _)


VALID_ARGS=$(getopt -o c:,d:,r:,f: --long cong_cont:,data_type:,rate:,flows: -- "$@")

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
   -r | --rate) #é em Mbps...Não precisa da unida, ou seja, 100 para indicar 100Mbps
        #echo "Processing 'rate' option. Input argument is '$2'"
        rate_var=$2
        shift 2
        ;;

   -f | --flows)
        #echo "Processing 'flows' option. Input argument is '$2'"
        num_clients=$2
        flows_var=$2
        shift 2
        ;;

  

    --) shift; 
        break 
        ;;
  esac
done



#PRODUZINDO OS queue_ewma
echo "Constructing queue_ewma file" 
for dir in ~/Desktop/output/*; do 
   if [ -d "$dir" ]; then    
        echo $dir
        #${data_type_var}_${cong_cont_var}_${flows_var}Fluxos_${rate_var}Mbps_${start_date_tshark}
        pattern="${data_type_var}_${cong_cont_var}_${flows_var}Fluxos_${rate_var}Mbps"    
        echo "${pattern}"    
        case "$dir" in  *${data_type_var}_${cong_cont_var}_${flows_var}Fluxos_${rate_var}Mbps*)
            echo "dir in case:"        
            echo $dir 
            command_var="cp ${dir}/*.csv  ~/Desktop/output/Train_Test; cp ${dir}/iperf*.txt  ~/Desktop/output/Train_Test "
            echo "App call: ${command_var}"
            xterm $hold_var -e "echo \"Starting copy from ${dir}\"; ${command_var}; echo \"finished\" " && /bin/bash &
            sleep 5
             ;;
        esac
   fi
done


