#!/bin/bash


        num_clients=60
        i=1
        n_to_wait=7        
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
                #command_var="./communicator tcp_feature_saver_and_extractor ${door} ${liquid_dir}"
                #echo "App call: ${command_var}"
                #xterm $hold_var -e "cd /home/ns/UDT-workspace/git-udt/vscode/shared/bin/Linux64/Debug;  ${command_var}" && /bin/bash &
                #xterm -e "cd /home/ns/UDT-workspace/git-udt/vscode/shared/bin/Linux64/Debug;  ${command_var}" && /bin/bash &

                #sleep 3

                i=`expr $i + 1` 
                exprtotal_extraction_calls=`expr $exprtotal_extraction_calls + 1`
                mod=$(($exprtotal_extraction_calls % $n_to_wait))
                echo "${mod}"
                if [ $mod -eq 0 ]; then
                    echo "Need wait."
                    sleep 5
                else
                    sleep 3
                fi
                
            done

