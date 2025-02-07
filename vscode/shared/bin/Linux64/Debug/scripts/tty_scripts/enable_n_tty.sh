echo "Enter the value of n:"
read n
n=`expr $n + 1` # para o looping ir até n
i=2
graph_int=7
while [ $i -lt $n ]
do

   if [ $i -eq $graph_int ];  #pulando a interface gráfica
        then
            echo "Skiping GUI" 
            i=`expr $i + 1`    
            continue
   fi
  #echo "tty${i}";
  echo  "Criando a tty${i}..."
  comn="sudo systemctl start getty@tty${i}.service";
  echotty="\"TTY${i}\"\n" 
  #echo $echotty;
  #echottycc=`$echotty`;
  b=`$comn`;
  echo ${b};
  #sleep 1;
  $( (printf ${echotty}) >  /dev/tty${i})
  #$(ls > "/dev/tty${i}")
  #echo ${b};
  #$(chvt ${n})
  i=`expr $i + 1`
  sleep 1;
done

