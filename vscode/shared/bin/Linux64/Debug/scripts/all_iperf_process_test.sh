#!/bin/bash

short_help_var="-h"


start_date_tshark=$(date | cut -b 1-19 | tr ' ' _ | tr : _)


if [ "$1" == "-h" ]; then

    echo "Usage: $0 [-c|--cong_cont 'congestion control'] [-d|--data_type 'type of data'] [-r|--rate 'bottleneck rate'] [-f|--flows 'number of flows'] [-s|--saturation 'number of flows to sature band']"
    exit 0
fi


if [ "$1" == "--help" ]; then

    echo "Usage: $0 [-c|--cong_cont 'congestion control'] [-d|--data_type 'type of data'] [-r|--rate 'bottleneck rate'] [-f|--flows 'number of flows'] [-s|--saturation 'number of flows to sature band']"
    exit 0
fi





VALID_ARGS=$(getopt -o c:d:r:f:s: --long cong_cont:,data_type:,rate:,flows:,saturation: -- "$@")
if [[ $? -ne 0 ]]; then
    exit 1;
fi


if [[ $# -ne 10 ]]; then
   echo "You provided $# arguments, but we need 10. See -h option."    
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

   -s | --saturation)
        #echo "Processing 'flows' option. Input argument is '$2'"
        saturation=$2
        flows_rate=`expr $rate_var / $saturation`        
        shift 2
        ;;


    --) shift; 
        break 
        ;;
  esac
done

echo "Starting iperf3 servers in servers host: "

xterm -hold -e ssh ns@10.0.1.3  "cd ~/UDT-workspace/git-udt/vscode/shared/bin/Linux64/Debug/scripts; ./n_iperf_servers_from_clients_host.sh ${flows_var}" & 

sleep 120



experiment_path="/home/ns/Desktop/output/${data_type_var}_${cong_cont_var}_${flows_var}Fluxos_${rate_var}Mbps_${start_date_tshark}"

echo $experiment_path

exec="mkdir ${experiment_path}";

$($exec)


exec="chmod 777 ${experiment_path}";

$($exec)



echo "Starting iperf3 clients in clients host"

#${data_type_var}_${cong_cont_var}_${flows_var}Fluxos_${rate_var}Mbps_${start_date}

echo "./n_iperf_clients.sh -c ${cong_cont_var} -d ${data_type_var} -r ${rate_var} -p ${experiment_path} -f ${flows_var} -s ${saturation}"

xterm -hold -e  "./n_iperf_clients.sh -c ${cong_cont_var} -d ${data_type_var} -r ${rate_var} -p ${experiment_path} -f ${flows_var} -s ${saturation}"







echo "Starting iperf3 clients in clients host"

#${data_type_var}_${cong_cont_var}_${flows_var}Fluxos_${rate_var}Mbps_${start_date}

echo "./n_iperf_clients_test.sh -c ${cong_cont_var} -d ${data_type_var} -r ${rate_var} -p ${experiment_path} -f ${flows_var} -s ${saturation}"

xterm -hold -e  "./n_iperf_clients_test.sh -c ${cong_cont_var} -d ${data_type_var} -r ${rate_var} -p ${experiment_path} -f ${flows_var} -s ${saturation}" && /bin/bash &

sleep 10

$(echo "Data Type: ${data_type_var}" >> "${experiment_path}/iperf_report.txt")

$(echo "CC: ${cong_cont_var}" >> "${experiment_path}/iperf_report.txt")

$(echo "#Flows: ${flows_var}" >> "${experiment_path}/iperf_report.txt")

$(echo "Bottleneck Rate: ${rate_var}" >> "${experiment_path}/iperf_report.txt")

$(echo "Saturation: ${saturation}" >> "${experiment_path}/iperf_report.txt")

$(echo "WmemMax: xxxxMbps" >> "${experiment_path}/iperf_report.txt")




