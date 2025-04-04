#!/bin/bash



i=1


short_help_var="-h"


if [ "$1" == "-h" ] || [ "$1" == "--help" ]; then

    echo "Usage: $0 [-r|--rate 'bottleneck rate'] [-f|--flows 'number of flows'] "
    exit 0
fi




VALID_ARGS=$(getopt -o r:f: --long rate:,flows: -- "$@")

if [[ $? -ne 0 ]]; then
    exit 1;
fi


if [[ $# -ne 4 ]]; then
   echo "You provided $# arguments, but we need 4. See -h option."    
    exit 0;
fi


eval set -- "$VALID_ARGS"
while [ : ]; do
  case "$1" in
   -r | --rate) #é em Mbps...Não precisa da unida, ou seja, 100 para indicar 100Mbps
        #echo "Processing 'rate' option. Input argument is '$2'"
        rate_var=$2
        shift 2
        ;;

   -f | --flows)
        #echo "Processing 'flows' option. Input argument is '$2'"
        num_servers=$2        
        flows_var=$2
        shift 2
        ;;


    --) shift; 
        break 
        ;;
  esac
done



rtt=41

: '

The * symbol is a shell glob (filename generation) character it will expand to a list of the non-hidden files in the current directory - in your case, that appears to include a Desktop item.
To prevent that quote or escape the * character to make it literal:

'


rmem_max=`expr ${rate_var} \* ${rtt} \* 1000`

sys_ctl_rmem_max="sudo sysctl -w net.core.rmem_max=${rmem_max}" 

echo $sys_ctl_rmem_max

exec=`$sys_ctl_rmem_max`;

echo ${exec}

sleep 5

sudo systemctl restart NetworkManager.service

sleep 20




while [ $i -le $num_servers ];
do
    
    door=`expr $i + 5101`
    echo $door
    #command="./udt server ${door}"
    #command="ls"
    #b=`$command`
    #echo $b
    #./udt server ${door}
    #xterm -hold -e  ssh ns@10.0.1.3 ./udt server ${door} #&& /bin/bash &
    #'cd udt_workspace/git-udt/vscode/shared/bin/Linux64/Debug; ./udt server ${door}'
    
    echo "App call: iperf3 -s ${door}"
    xterm -hold -e  "iperf3 -s -p ${door}" && /bin/bash &
    sleep 1

    i=`expr $i + 1` 

done


