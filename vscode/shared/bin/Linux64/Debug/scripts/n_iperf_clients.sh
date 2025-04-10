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


: '
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
'

rtt=41

: '

The * symbol is a shell glob (filename generation) character it will expand to a list of the non-hidden files in the current directory - in your case, that appears to include a Desktop item.
To prevent that quote or escape the * character to make it literal:

'

: '
wmem_max=`expr ${rate_var} \* ${rtt} \* 1000`

sys_ctl_wmem_max="sudo sysctl -w net.core.wmem_max=${wmem_max}" 

echo $sys_ctl_wmem_max

exec=`$sys_ctl_wmem_max`;

echo ${exec}

sleep 5

sudo systemctl restart NetworkManager.service

sleep 20
'


: '
experiment_path="/home/ns/Desktop/output/${data_type_var}_${cong_cont_var}_${flows_var}Fluxos_${rate_var}Mbps_${start_date}"

echo $experiment_path

exec="mkdir ${experiment_path}";
'

: '
A notação $() substitui o que é variavel e executa o resultado final
Por exemplo, se

data_type_var = Treino
cong_cont_var = TcpVegas
flows_var = 60
rate_var=500
start_date=Mon_Mar_31_02_58_45 

 a linha abaixo vai substituir o valor das variáveis presentes em  experiment_path e executar 
  
mkdir /home/home/ns/Desktop/output/Treino_TcpVegas_60Fluxos_500Mbps_Mon_Mar_31_02_58_45

Bizu. Essa é a lógica também nas linhas finais deste script

$($exec)

'



echo "${num_clients}"


while [ $i -le $num_clients ];
do
    
    door=`expr $i + 5101`
    cdoor=`expr $i + 4101`
    echo $door
    #command="./udt server ${door}"
    #command="ls"
    #b=`$command`
    #echo $b
    #./udt server ${door}
    #xterm -hold -e  ssh ns@10.0.1.3 ./udt server ${door} #&& /bin/bash &
    
    iperf_call="iperf3  -c 10.0.1.3 --cport ${cdoor} -p ${door} -t 30"
        
    #echo "App call: iperf3  -c 10.0.1.3 -t 30 -p ${door}"    
    echo "App call: ${iperf_call}"
    
    #xterm -hold -e  "iperf3  -c 10.0.1.3 --cport ${cdoor} -p ${door}" && /bin/bash &
    xterm -e  ${iperf_call} && /bin/bash &
    #${iperf_call}
    #xterm -hold -e  "iperf3  -c 10.0.1.3 -t 30 -p ${door}" && /bin/bash &
    #xterm -hold -e  "iperf3 -c 10.0.1.3 -N -i 1 -t 90 -b /${flows_rate}M -p ${door}" && /bin/bash &
    sleep 0.5

    i=`expr $i + 1` 

done


#iperf_call="iperf3  -c 10.0.1.3 --cport 4102 -p 5102 -t 90 -P ${num_clients}"

#echo "App call: ${iperf_call}"

#${iperf_call}
#Tem que esperar. Se não o script termina antes de chamar os clientes todos, o que faz com que
#os clientes finais (58-61) não subam.
sleep 20


echo "Reporting iperf command"

#Se colocar a notação $() nos comandos abaixo, o Xterm exibe uma mensagem dizendo que o 
#arquivo não existe, mas acaba atualizando o arquivo de qualquer forma. Estranho....

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

$(echo "iperf3 Last Call: ${iperf_call}" >> "${experiment_path}/iperf_report.txt")

