#/bin/bash
sudo tc qdisc del dev enp0s20f3 root
sleep 5
sudo tc qdisc add dev enp0s20f3 root handle 1: netem delay 40ms
sleep 5
sudo tc qdisc add dev enp0s20f3 parent 1: handle 2: tbf rate 100mbit burst 120000 limit 450000
sleep 5
sudo tc -s qdisc show dev enp0s20f3
sleep 5
#sudo nmcli connection show
sudo nmcli connection modify 0056e987-0275-32bc-9ef4-e89c9c1cf9f6 link.tx-queue-length 300
sleep 5
sudo nmcli connection up 0056e987-0275-32bc-9ef4-e89c9c1cf9f6
sleep 5
sudo ip -s link show enp0s20f3
sleep 5
sudo sysctl -w net.core.netdev_max_backlog=300
sleep 5
sudo sysctl net.core.netdev_max_backlog
sleep 5
sudo ip route add 10.0.0.0/24 via 10.0.0.1
sleep 5
sudo ip route add 10.0.1.0/24 via 10.0.1.1
sleep 5

sleep 5
sudo mkdir /tmp/ramdisk

sleep 2
sudo chmod 777 /tmp/ramdisk/

sleep 2
sudo mount -t tmpfs -o size=2G myramdisk /tmp/ramdisk

sleep 2

mount | tail -n 1


