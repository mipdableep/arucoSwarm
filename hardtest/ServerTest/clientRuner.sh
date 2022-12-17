IP_ADDRESS=$(hostname -I | cut -d " " -f 1)
python3 /home/fares/rbd/projects/aruco_swarm/arucoSwarm/hardtest/ServerTest/clientTest.py $IP_ADDRESS
