#!/bin/bash

i=2
num_clients=$1
last_terminal_tty=`expr $num_clients + 2`
graph_int=7
start_date=$(date | cut -b 1-19 | tr -s " " | tr ' ' _ | tr : _)

#./communicator $app_type $cong_control $port $simul_type $data_rate $num_flows $simul_start_time

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
    #ao longo da simulção. Por isso, nos argv só há a porta. Lembrando que no caso dos udt_clients os argv eram passados
    #para a classe pure_udt, que configurava um saver dentro dela.
    #Sendo assim será necessário criar um script que levante n tcp_feature_saver e, aí sim, passar os parâmetros, conforme n_udt_client.
    xterm -e "cd /home/ns/UDT-workspace/git-udt/vscode/shared/bin/Linux64/Debug; ./communicator tcp_feature_saver ${door} Treino 100 ${num_clients} ${start_date}" && /bin/bash &
    sleep 0.5

    i=`expr $i + 1` 

done



