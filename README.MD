# Aruco Swarm

## rpi0-w2 setup:

after installing raspberian lite -64 bit on the rpi, run 
arucoInstall.sh to install the dependencies.

change the tello.conf to the right ssid with the `change_tello_conf.sh` script.

add the `bashrc_extention.sh` to `.bashrc` in the home folder.


## activate server:

open an hotspot from a phone

go to `/etc/wpa_supplicant/wpa_supplicant.conf` on the rpi0 and change the following fields:
```
network={
to new ssid     -> ssid="SSID"
to new password ->  psk="****"
}
```
OR run the `change_wpa_sup` script

connect to the hotspot thrugh another device, and check your ip.
you can do it bu runing `$ hostname -I | cut -b 1-15`
in bashrc_extension change ipServerHost to your ip

after all is done, run the following:

```
$ python3 server.py 0.0.0.0 9090 {number of drones}
```

after that connect the turn on the rpi0's and the drones