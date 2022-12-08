cd /home/pi/arucoSwarm
export LD_LIBRARY_PATH=/usr/local/lib
while ! [[ $(ip a | grep "inet 192") ]]; do sleep 1s; done

ipServerHost=192.168.43.163

cd /home/pi/arucoSwarm/build
/home/pi/arucoSwarm/build/followAruco /home/pi/arucoSwarm/mobilenet_ssd_v2_coco_quant_postprocess.tflite $ipServerHost 9090 2>&1 | tee out.txt