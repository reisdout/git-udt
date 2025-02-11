#bin/bash


#Este script altera a configuração da tty, cujo numero é recebido como parametro
#Poe exemplo, se quisermos alterar a tty11, basta chamarmos no console o .st:

# autologin_just_one_tty.sh 11.

n_tty="tty$1"
echo ${n_tty}
#cat > /etc/systemd/system/getty@tty$1.service.d/override.conf <<- "_EOF_"
# [Service]
# ExecStart=
# ExecStart=-/sbin/mingetty --autologin ns --noclear ${n_tty}
# _EOF_

com_stop_tty="$(printf '%s%s%s\n' 'sudo systemctl stop getty@tty'$1'.service')"
${com_stop_tty}

sudo mkdir -p /etc/systemd/system/getty@tty$1.service.d
echo "[Service]" > /etc/systemd/system/getty@tty$1.service.d/override.conf
echo "ExecStart=" >> /etc/systemd/system/getty@tty$1.service.d/override.conf
echo "ExecStart=-/sbin/mingetty --autologin ns --noclear tty$1" >> /etc/systemd/system/getty@tty$1.service.d/override.conf
echo "_EOF_" >> /etc/systemd/system/getty@tty$1.service.d/override.conf

com_start_tty="$(printf '%s%s%s\n' 'sudo systemctl start getty@tty'$1'.service')"
${com_start_tty}
