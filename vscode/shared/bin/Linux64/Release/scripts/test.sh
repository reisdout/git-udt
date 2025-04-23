#!/bin/bash


start_date=$(date | cut -b 1-19 | tr ' ' _ | tr : _)

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




rtt=41

wmem_max=`expr $rate_var * $rtt`

sys_ctl_wmem_max="sudo sysctl -w net.core.wmem_max=${wmem_max}" 

exec=`$sys_ctl_wmem_max`;

echo${exec}
