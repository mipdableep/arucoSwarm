cd /home/pi/arucoSwarm/

ls /dev/ | grep -v -E "sd" > /home/pi/arucoSwarm/scripts/compare.txt
break
diff -q /home/pi/arucoSwarm/scripts/devls.txt /home/pi/arucoSwarm/scripts/compare.txt
if [[ $? == "0" ]]
then
    echo -e "\nin chroot, breaking\n"
    cd /home/pi
else

    export LD_LIBRARY_PATH=/usr/local/lib
    while ! [[ $(ip a | grep "inet 192") ]]; do sleep 1s; done

    ipServerHost=192.168.43.163

    cd /home/pi/arucoSwarm/build
    /home/pi/arucoSwarm/build/followAruco /home/pi/arucoSwarm/mobilenet_ssd_v2_coco_quant_postprocess.tflite $ipServerHost 9090 2>&1 | tee out.txt
    
fi
