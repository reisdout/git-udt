echo "Enter the value of n:"
read n
n=`expr $n + 1` # para o looping ir até n
i=2
while [ $i -lt $n ]
do
  #echo "tty${i}";
  echo  "Executando na tty${i}..."
  comn="chvt ${i}";
  echotty="\"TTY${i}\"\n" 
  #echo $echotty;
  #echottycc=`$echotty`;
  b=`$comn`;
  echo ${b};
  sleep 1;
  com_exec="ls -l"
  exec=`$com_exec`;
  $( (echo ${exec}) > /dev/tty${i});
  sleep 1;
  #$( (printf ${echotty}) >  /dev/tty${i})
  #$(ls > "/dev/tty${i}")
  #echo ${b};
  #$(chvt ${n})
  i=`expr $i + 1`
  sleep 1;
done

