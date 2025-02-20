#bin/bash

#bin/bash

#antes de se instalar este arquivo, deve-se instalar o mingetty
#sudo apt install mingetty

echo "Enter the value of n:" # n deve ser <= 63
read n

n=`expr $n + 1` # para o looping ir até n
i=5 #começando de 6, pois o procedimento ja foi realizado para as tty1 a tty4
graph_int=7
while [ $i -lt $n ]
do
  
  if [ $i -eq $graph_int ];  
  then
    i=`expr $i + 1`    
    continue
  fi
  echo ${i}
  i=`expr $i + 1` 
  #./change_conf_tty.sh ${i}    


done


#n_tty="tty$1"
#echo ${n_tty}

#mkdir -p tty$1

#echo "[Select]" >> ./tty$1/test$1.txt
#./change_conf_tty.sh ${n}

