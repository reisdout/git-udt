#!/bin/bash

#hold_var=" "
hold_var="-hold"

if [ "$1" == "-h" ]; then

    echo "Usage: $0 [-c|--cong_cont 'congestion control'] [-d|--data_type 'type of data'] [-r|--rate 'bottleneck rate'] [-f|--flows 'number of flows'] [-l|--location 'destination of the files']"
    exit 0
fi


if [ "$1" == "--help" ]; then

    echo "Usage: $0 [-c|--cong_cont 'congestion control'] [-d|--data_type 'type of data'] [-r|--rate 'bottleneck rate'] [-f|--flows 'number of flows'] [-l|--location 'destination of the files']"
    exit 0
fi





i=1
#num_clients=$1
last_terminal_tty=`expr $num_clients + 2`
graph_int=7
start_date=$(date | cut -b 1-19 | tr ' ' _ | tr : _)


VALID_ARGS=$(getopt -o c:,d:,r:,f:,l: --long cong_cont:,data_type:,rate:,flows:,location: -- "$@")

if [[ $? -ne 0 ]]; then
    exit 1;
fi


if [[ $# -ne 10 ]]; then
   echo "You provided $# arguments, but we need 10. Use -h option."    
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

   -l | --location)        
        file_destination=$2
        shift 2
        ;;


  
    --) shift; 
        break 
        ;;
  esac
done



sudo mkdir /home/ns/Desktop/output/${file_destination}
sleep 1
sudo chmod 777 /home/ns/Desktop/output/${file_destination}
sleep 1

for dir in ~/Desktop/output/*; do 
   if [ -d "$dir" ]; then    
        echo $dir
        #${data_type_var}_${cong_cont_var}_${flows_var}Fluxos_${rate_var}Mbps_${start_date_tshark}
        pattern="${data_type_var}_${cong_cont_var}_${flows_var}Fluxos_${rate_var}Mbps"    
        echo "${pattern}"    
        case "$dir" in  *${data_type_var}_${cong_cont_var}_${flows_var}Fluxos_${rate_var}Mbps*)
            echo "dir in case:"        
            echo $dir 
            command_var="cp -v ${dir}/*.csv  ~/Desktop/output/${file_destination}; cp -v ${dir}/iperf*.txt  ~/Desktop/output/${file_destination}"
            echo "App call: ${command_var}"
            xterm $hold_var -e "echo \"Starting copy from ${dir}\"; ${command_var}; echo \"finished\" " && /bin/bash &
            sleep 3
             ;;
        esac
   fi
done

num_flows_cc=$(echo $cong_cont_var | cut -c 4-)

$(echo "${flows_var} ${num_flows_cc}-${rate_var}Mbps" > "${HOME}/Desktop/output/${file_destination}/numFlows.txt")
sleep 1



