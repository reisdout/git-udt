
echo "Enter the value of n:"
read n
i=2
graph_int=7
while [ $i -le $n ];
do
  #echo "tty${i}";
  if [ $i -eq $graph_int ];  #pulando a interface gráfica
        then
            echo "Skiping GUI" 
            i=`expr $i + 1`    
            continue
  fi
    
  comn="chvt ${i}";
  echotty="\"TTY${i}\"\n" 
  #echo $echotty;
  #echottycc=`$echotty`;
  b=`$comn`;
  echo ${b};
  $( (printf ${echotty}) >  /dev/tty${i})
  #$(ls > "/dev/tty${i}")
  #echo ${b};
  #$(chvt ${n})
  i=`expr $i + 1`
  sleep 2;
done
