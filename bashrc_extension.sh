if [[ "$(awk '$5=="/" {print $1}' </proc/1/mountinfo)" != "$(awk '$5=="/" {print $1}' </proc/$$/mountinfo)" ]]
then
    echo -e "\nin chroot, breaking\n"
else

    while ! [[ $(ip a | grep "inet 192") ]]; do sleep 1s; done

    python3 /home/pi/arucoSwarm/sshClient.py

    # cd /home/pi/arucoSwarm/build
    # /home/pi/arucoSwarm/build/runAruco 2>&1 | tee out.txt
    
fi