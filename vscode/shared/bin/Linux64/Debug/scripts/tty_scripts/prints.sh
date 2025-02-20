com_stop_tty="$(printf '%s%s%s\n' 'sudo systemctl stop getty@tty'$1'.service')"
com_start_tty="$(printf '%s%s%s\n' 'sudo systemctl start getty@tty'$1'.service')"
echo ${com_stop_tty} 
echo ${com_start_tty}
