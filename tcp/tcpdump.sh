#!/bin/bash

#echo "Give interface : loopback0/wlan0/eth0"
#read ifc
echo "Give protocol"
read protocol
#echo "Give client"
#read client
echo "Give server"
read server
echo "Give server port"
read serv_port
echo $protocol $server $serv_port
sudo tcpdump -nn -S -X -i lo $protocol and host $server and port $serv_port -l | tee dump.txt
