if ! [[ "$(awk '$5=="/" {print $1}' </proc/1/mountinfo)" != "$(awk '$5=="/" {print $1}' </proc/$$/mountinfo)" ]]
then
    echo -e "\nin chroot, breaking\n"
else

    echo a
    while ! [[ $(ip a | grep "inet 10") ]]; do sleep 1s; done
    echo
    cd /home/pi/arucoSwarm
    sudo python3 /home/pi/arucoSwarm/sshClient.py

    # cd /home/pi/arucoSwarm/build
    # /home/pi/arucoSwarm/build/runAruco 2>&1 | tee out.txt
    
fi
