#!/bin/bash

hold_var=" "
#hold_var="-hold"

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


total_extraction_calls=1
n_to_wait=20

prefix_dir="/home/ns/Desktop/output/"
prefix_dir_size=${#prefix_dir}


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
            dir_size=${#dir}
            liquid_dir=${dir:$prefix_dir_size:$dir_size}
            echo "liquid dir: ${liquid_dir}"
            command_var="./communicator router_ewma ${liquid_dir}"
            echo "App call: ${command_var}"
            xterm $hold_var -e "cd /home/ns/UDT-workspace/git-udt/vscode/shared/bin/Linux64/Release;  ${command_var}" && /bin/bash &
            sleep 3
             ;;
        esac
   fi
done


sleep 5


echo "Generating metric files"

for dir in ~/Desktop/output/*; do 
   if [ -d "$dir" ]; then    
    echo $dir
    #${data_type_var}_${cong_cont_var}_${flows_var}Fluxos_${rate_var}Mbps_${start_date_tshark}
    pattern="${data_type_var}_${cong_cont_var}_${flows_var}Fluxos_${rate_var}Mbps"    
    echo "${pattern}"    
    case "$dir" in  *${data_type_var}_${cong_cont_var}_${flows_var}Fluxos_${rate_var}Mbps*)
        echo "dir in case:"        
        echo $dir
        dir_size=${#dir}
        liquid_dir=${dir:$prefix_dir_size:$dir_size}
        echo "liquid dir: ${liquid_dir}"
        i=1
        
            while [ $i -le $num_clients ];
            do

                #echo "$exprtotal_extraction_calls"
                door=`expr $i + 5101` 
                echo $door
                #command="./udt server ${door}"
                #command="ls"
                #b=`$command`
                #echo $b
                #./udt server ${door}
                command_var="./communicator tcp_feature_saver_and_extractor ${door} ${liquid_dir}"
                echo "App call: ${command_var}"
                xterm $hold_var -e "cd /home/ns/UDT-workspace/git-udt/vscode/shared/bin/Linux64/Release;  ${command_var}" && /bin/bash &
                #xterm -e "cd /home/ns/UDT-workspace/git-udt/vscode/shared/bin/Linux64/Debug;  ${command_var}" && /bin/bash &

                #sleep 3

                i=`expr $i + 1` 
                exprtotal_extraction_calls=`expr $exprtotal_extraction_calls + 1`
                mod=$(($exprtotal_extraction_calls % $n_to_wait))                
                if [ $mod -eq 0 ]; then
                    echo "Need wait."
                    sleep 10
                elif [ $exprtotal_extraction_calls -le 5 ]; then
                    sleep 1
                else
                    sleep 3
                fi
                
            done
            

            ;;
        esac


   fi
done

