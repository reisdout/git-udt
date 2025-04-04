#!/bin/bash

echo "test all iperf3 servers up: "

i=1
pkill -f iperf3

num_clients=$1

while [ $i -le $num_clients ];
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
   
#nohup iperf3 -s -p 5102 > 5102.txt 2>&1 & nohup iperf3 -s -p 5103 > 5103.txt 2>&1  & nohup iperf3 -s -p 5104 > 5104.txt 2>&1  & nohp iperf3 -s -p 5105 > 5105.txt 2>&1  & nohup iperf3 -s -p 5106 > 5106.txt 2>&1  & nohup iperf3 -s -p 5107 > 5107.txt 2>&1 &

    #if [[ $i -eq 1 ]]; then
       command="iperf3 -s -p ${door} -D"
    echo "${command}"
    $($command)
    #else

       #command="${command} ; iperf3 -s -p ${door} -D"
    #fi

    sleep 1

    i=`expr $i + 1` 

done

#command="${command} sleep 50; pkill -f iperf3 && /bin/bash &"


#command="${command} 

#$($command)
sleep 500
pkill -f iperf3
echo "servers finished!"


#xterm -hold -e "${command}"

