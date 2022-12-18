IP_ADDRESS=$(hostname -I | cut -d " " -f 1)
python3 /home/pi/arucoSwarm/hardtest/ServerTest/clientTest.py $IP_ADDRESS
