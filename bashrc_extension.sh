if [[ "$(awk '$5=="/" {print $1}' </proc/1/mountinfo)" != "$(awk '$5=="/" {print $1}' </proc/$$/mountinfo)" ]]
then
    echo -e "\nin chroot, breaking\n"
else

    while ! [[ $(ip a | grep "inet 192") ]]; do sleep 1s; done

    ipServerHost=192.168.43.163

    cd /home/pi/arucoSwarm/build
    /home/pi/arucoSwarm/build/followAruco $ipServerHost 9090 2>&1 | tee out.txt
    
fi
