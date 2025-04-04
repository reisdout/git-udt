#!/bin/bash

function get_cur_sec() {
	date +%s
}

function get_cur_nano_sec() {
	date +%s%N
}

#command="tc -s -d qdisc ls dev ens33";
#command="tc -s -d qdisc ls dev ens33 | awk '/backlog[[:space:]]/ { print $2 }'";
#echo "Command: $command"; #Output is: Command: ls

#b=`$command`;
#echo $b; #Output is: public_html REV test... (command worked successfully)

#variableA=$(tc -s -d qdisc ls dev enp0s20f3 | awk 'NR==6 &&  /backlog/ { print  $2 }')
#variableA=$(netstat -nua)
#read_time=$(date +%s%N)
#echo $variableA

# How long the script should run for. 
duration=$1 #600
CURSEC=`get_cur_sec`
#echo $duration
#echo $CURSEC
quitpoint=`expr $CURSEC + $duration`

while [ $CURSEC -lt $quitpoint ];
do
  printf `get_cur_nano_sec`
  printf ","  
  #variableA=$(tc -s -d qdisc ls dev enp0s20f3 | awk 'NR==6 || NR==3 &&  /backlog/ { print  $2 }')
  variableA=$(tc -s -d qdisc ls dev enp0s20f3 | awk 'NR==6 &&  /backlog/ { print  $2 }')
  printf $variableA
  printf "\n"
  sleep 0.000000005
  CURSEC=`get_cur_sec`  
done

echo "Finish queue capture!"


