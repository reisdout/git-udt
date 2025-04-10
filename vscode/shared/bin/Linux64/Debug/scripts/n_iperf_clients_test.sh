#!/bin/bash

short_help_var="-h"


if [ "$1" == "-h" ]; then

    echo "Usage: $0 [-c|--cong_cont 'congestion control'] [-d|--data_type 'type of data'] [-r|--rate 'bottleneck rate'] [-p|--path 'experiment path'] [-f|--flows 'number of flows'] [-s|--saturation 'number of flows to sature band']"
    exit 0
fi


if [ "$1" == "--help" ]; then

    echo "Usage: $0 [-c|--cong_cont 'congestion control'] [-d|--data_type 'type of data'] [-r|--rate 'bottleneck rate'] [-p|--path 'experiment path'] [-f|--flows 'number of flows'] [-s|--saturation 'number of flows to sature band']"
    exit 0
fi



i=1

start_date=$(date | cut -b 1-19 | tr ' ' _ | tr : _)

VALID_ARGS=$(getopt -o c:d:r:p:f:s: --long cong_cont:,data_type:,rate:,path:,flows:,saturation: -- "$@")

if [[ $? -ne 0 ]]; then
    exit 1;
fi


if [[ $# -ne 12 ]]; then
   echo "You provided $# arguments, but we need 12. See -h option."    
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

   -p | --path) #onde estarão os arquivos do experimento
        #echo "Processing 'rate' option. Input argument is '$2'"
        experiment_path=$2
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



#Tem que esperar. Se não o script termina antes de chamar os clientes todos, o que faz com que
#os clientes finais (58-61) não subam.

echo "Reporting iperf command"

iperf_call="iperf3  -c 10.0.1.3 --cport -p "

#Se colocar a notação $() nos comandos abaixo, o Xterm exibe uma mensagem dizendo que o 
#arquivo não existe, mas acaba atualizando o arquivo de qualquer forma.

cd  ${experiment_path}


sleep 1

touch "${experiment_path}/iperf_report.txt"

sleep 2

chmod 777 "${experiment_path}/iperf_report.txt" 

: '
$(echo "Data Type: ${data_type_var}" >> "${experiment_path}/iperf_report.txt")

$(echo "CC: ${cong_cont_var}" >> "${experiment_path}/iperf_report.txt")

$(echo "#Flows: ${flows_var}" >> "${experiment_path}/iperf_report.txt")

$(echo "Bottleneck Rate: ${rate_var}" >> "${experiment_path}/iperf_report.txt")

$(echo "Saturation: ${saturation}" >> "${experiment_path}/iperf_report.txt")

$(echo "WmemMax: ${wmem_max}" >> "${experiment_path}/iperf_report.txt")
'

$(echo "iperf3_call: ${iperf_call}" > "${experiment_path}/iperf_report.txt")

