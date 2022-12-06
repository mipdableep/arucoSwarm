export LD_LIBRARY_PATH=/usr/local/lib
while ! [[ $(ip a | grep "inet 192") ]]; do sleep 1s; done
cd /home/pi/arucoSwarm/build
/home/pi/arucoSwarm/build/followAruco /home/pi/arucoSwarm/mobilenet_ssd_v2_coco_quant_postprocess.tflite 192.168.138.125 9090 2>&1 | tee out.txt