#!/bin/bash

i=2
num_clients=$1
last_terminal_tty=`expr $num_clients + 2`
graph_int=7

start_date=$(date | cut -b 1-19 | tr ' ' _ | tr : _)
echo $start_date
