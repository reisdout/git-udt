#bin/bash

#antes de se executar este script, deve-se instalar o mingetty:
#sudo apt install mingetty

echo "Enter the value of n:" # n deve ser <= 63
read n
n=`expr $n + 1` # para o looping ir até n
i=4 #começando de 4, pois o procedimento ja foi realizado para as tty1 a tty3
while [ $i -lt $n ]
do
  #echo "tty${i}";
  echo  "Automatizando o login para a tty${i}..."
  sudo mkdir -p /etc/systemd/system/getty@tty${i}.service.d  
  sleep 1;
  com_set_config_cat="$(printf '%s' 'cat > ')"
  com_set_config_conf_file="$(printf '%s%s%s\n' '/etc/systemd/system/getty@tty'${i}'.service.d/override.conf')"
  com_set_config_conf_file_eof="$(printf '%s\n' '<<- "_EOF_"')"
  
  com_set_config_conf_service="$(printf '%s\n' '[Service]')"
  
  com_set_config_conf_exec="$(printf '%s\n' 'ExecStart=')"

  com_set_config_conf_exec_attr="$(printf '%s\n' 'ExecStart=-/sbin/mingetty')"
  com_set_config_conf_exec_attr_par_one="$(printf '%s\n' '--autologin ns')"
  com_set_config_conf_exec_attr_par_two="$(printf '%s%s\n' '--noclear tty'${i})"
  
  com_last_eof="$(printf '%s\n' '_EOF_')"

  ${com_set_config_cat} ${com_set_config_conf_file} ${com_set_config_conf_file_eof}  ${com_set_config_conf_service} ${com_set_config_conf_exec} ${com_set_config_conf_exec_attr} ${com_set_config_conf_exec_attr_par_one} ${com_set_config_conf_exec_attr_par_two} ${com_last_eof} 
  
  com_stop_tty="$(printf '%s%s%s\n' 'sudo systemctl stop getty@tty'${i}'.service')"
  com_start_tty="$(printf '%s%s%s\n' 'sudo systemctl start getty@tty'${i}'.service')"
  ${com_stop_tty} 
  ${com_start_tty}

  i=`expr $i + 1`
  sleep 1;
done

