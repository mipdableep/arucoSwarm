# Aruco Swarm

## rpi0-w2 setup

after installing raspberian lite -64 bit on the rpi, run 
arucoInstall.sh to install the dependencies.

change the tello.conf to the right ssid with the `change_tello_conf.sh` script.

add the `bashrc_extention.sh` to `.bashrc` in the home folder.

## tello network setup

open an hotspot you can use for runing the server-client connection.

go to `/etc/wpa_supplicant/wpa_supplicant.conf` on the rpi0 and change the following fields,
OR run the `change_wpa_sup` script in the scripts folder, and follow the directions written on the screen.
```
network={
to new ssid     -> ssid="SSID"
to new password ->  psk="****"
}
```

connect to the hotspot thrugh the device that will run the server, and check your ip.
you can do it by runing `$ hostname -I | cut -d " " -f 1`
in bashrc_extension change the of each of the drones, change the `ipServerHost` 
to the ip of the device that will run the server on the hotspot network.

## activate server:

after all is done, run the following:

```
$ python3 server.py 0.0.0.0 9090 {number of drones}
```

after that connect the turn on the rpi0's and the drones