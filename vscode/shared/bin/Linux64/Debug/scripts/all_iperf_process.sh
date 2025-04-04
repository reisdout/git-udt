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





rtt=43

wmem_max=`expr ${rate_var} \* ${rtt} \* 1000`

sys_ctl_wmem_max="sudo sysctl -w net.core.wmem_max=${wmem_max}" 

echo $sys_ctl_wmem_max

exec=`$sys_ctl_wmem_max`;

echo ${exec}

sleep 5

sudo systemctl restart NetworkManager.service

sleep 20





echo "Starting Tshark ACK capture in client host:"

sleep 2

#tshark -i ens33 -B 300 -f "tcp[tcpflags] == tcp-ack and src 10.0.1.3"  -w ./tcp_ACK_pcap_09_52.pcapng


xterm -hold -e  "cd /tmp/ramdisk/tshark; tshark -i ens33 -a duration:750 -B 300 -f \"tcp[tcpflags] == tcp-ack and src 10.0.1.3\"  -w ./tcp_ACK_pcap_${start_date_tshark}.pcapng" && /bin/bash &

echo "Starting Tshark SYN capture in host:"

#tshark -i ens33 -a packets:30 -f "(tcp[tcpflags] == tcp-syn or tcp[tcpflags]== tcp-syn|tcp-ack) and (src 10.0.0.3 or  src 10.0.1.3)"  -w ./tcp_SYN_pcap_09_52.pcapng


xterm -hold -e  "cd /tmp/ramdisk/tshark; tshark -i ens33 -a packets:30 -f \"(tcp[tcpflags] == tcp-syn or tcp[tcpflags]== tcp-syn|tcp-ack) and (src 10.0.0.3 or  src 10.0.1.3)\"  -w ./tcp_SYN_pcap_${start_date_tshark}.pcapng" && /bin/bash &


sleep 30

echo "Starting Tshark packet capture in router:"

xterm -hold -e ssh ns@10.0.0.1  "cd /tmp/ramdisk/tshark;  tshark -i enp0s20f3 -a duration:750 -f \"(tcp[13] & 8 !=0) and src 10.0.0.3\" -s 100 -w ./tcp_pcap_${start_date_tshark}.pcapng" && /bin/bash &


sleep 10

echo "Starting iperf3 servers in servers host: "

xterm -hold -e ssh ns@10.0.1.3  "cd ~/UDT-workspace/git-udt/vscode/shared/bin/Linux64/Debug/scripts; ./n_iperf_servers_from_clients_host.sh ${flows_var}" & 

sleep 100


echo "Starting queue capture in router:"

xterm -hold -e ssh ns@10.0.0.1  "echo \"Starting queue record\"; cd /tmp/ramdisk/queue;  ./queue_monitor.sh 200 >> queue_along_time_${start_date_tshark}.txt" && /bin/bash &

sleep 2


experiment_path="/home/ns/Desktop/output/${data_type_var}_${cong_cont_var}_${flows_var}Fluxos_${rate_var}Mbps_${start_date_tshark}"

echo $experiment_path

exec="mkdir ${experiment_path}";

$($exec)


exec="chmod 777 ${experiment_path}";

$($exec)



echo "Starting iperf3 clients in clients host"

#${data_type_var}_${cong_cont_var}_${flows_var}Fluxos_${rate_var}Mbps_${start_date}

xterm -hold -e  "./n_iperf_clients.sh -c ${cong_cont_var} -d ${data_type_var} -r ${rate_var} -f ${flows_var} -s ${saturation}" && /bin/bash &

sleep 700

echo "Check ack if ACK capture get off"

echo "Generanting ACK dump in clients host"

xterm -hold -e  "cd /tmp/ramdisk/tshark; tcpdump -S -n -tt --micro -r tcp_ACK_pcap_${start_date_tshark}.pcapng > tcp_dump_ACK_${start_date_tshark}.txt" && /bin/bash &

echo "Generating SYN dump"

xterm -hold -e  "cd /tmp/ramdisk/tshark; tcpdump -S -n -tt --micro -r tcp_SYN_pcap_${start_date_tshark}.pcapng > tcp_dump_SYN_${start_date_tshark}.txt" && /bin/bash &

echo "Generating packets dump in router"

xterm -hold -e ssh ns@10.0.0.1  "cd /tmp/ramdisk/tshark; tcpdump -S -n -tt --nano -r tcp_pcap_${start_date_tshark}.pcapng > tcp_dump_${start_date_tshark}.txt" && /bin/bash &

sleep 30

echo "Copying dumps and queue mesures to experiemnt directories"


cmd="mkdir ${experiment_path}/clients_data"

$($cmd)


exec="chmod 777 ${experiment_path}/clients_data";

$($exec)



cmd="mkdir ${experiment_path}/router_data"

$($cmd)


exec="chmod 777 ${experiment_path}/router_data";

$($exec)


cmd="scp ns@10.0.0.1:/tmp/ramdisk/tshark/tcp_dump_${start_date_tshark}.txt  ${experiment_path}/router_data/"

$($cmd)

sleep 10

cmd="mv  ${experiment_path}/router_data/tcp_dump_${start_date_tshark}.txt   ${experiment_path}/router_data/tcp_dump.txt"

$($cmd)


cmd="scp ns@10.0.0.1:/tmp/ramdisk/queue/queue_along_time_${start_date_tshark}.txt ${experiment_path}/router_data/"

$($cmd)

sleep 10

cmd="mv  ${experiment_path}/router_data/queue_along_time_${start_date_tshark}.txt  ${experiment_path}/router_data/queue_along_time.txt"

$($cmd)



cmd="cp /tmp/ramdisk/tshark/tcp_dump_ACK_${start_date_tshark}.txt ${experiment_path}/clients_data/"

$($cmd)


cmd="mv ${experiment_path}/clients_data/tcp_dump_ACK_${start_date_tshark}.txt ${experiment_path}/clients_data/clients_dump_ACK.txt"

$($cmd)



cmd="cp /tmp/ramdisk/tshark/tcp_dump_SYN_${start_date_tshark}.txt ${experiment_path}/clients_data/"

$($cmd)


cmd="mv ${experiment_path}/clients_data/tcp_dump_SYN_${start_date_tshark}.txt ${experiment_path}/clients_data/clients_dump_SYN.txt"

$($cmd)



$(echo "Data Type: ${data_type_var}" >> "${experiment_path}/iperf_report.txt")

$(echo "CC: ${cong_cont_var}" >> "${experiment_path}/iperf_report.txt")

$(echo "#Flows: ${flows_var}" >> "${experiment_path}/iperf_report.txt")

$(echo "Bottleneck Rate: ${rate_var}" >> "${experiment_path}/iperf_report.txt")

$(echo "Saturation: ${saturation}" >> "${experiment_path}/iperf_report.txt")

$(echo "WmemMax: ${wmem_max}" >> "${experiment_path}/iperf_report.txt")



