##rpi0-w2 setup:

after installing raspberian lite -64 bit on the rpi, run arucoInstall.sh to install the dependencies

change the tello.conf to the right ssid with the <sup>change_tello_conf.sh</sup> script

add the <sup>bashrc_extention.sh</sup> to <sup>.bashrc</sup> in the home folder


##activate server:

open an hotspot from a phone

go to <sup>/etc/wpa_supplicant/wpa_supplicant.conf</sup> on the rpi0 and change the following fields:

network={
to new ssid     -> ssid="SSID"
to new password ->  psk="****"
}

OR run the <sup>change_wpa_sup</sup> script

connect to the hotspot thrugh another device, and check your ip.
you can do it bu runing <sup>$hostname -I | cut -b 1-13</sup>
in bashrc_extension change ipServerHost to your ip

after all is done, run the following:

<sup>python3 server.py 0.0.0.0 {number of drones}</sup>

after that connect the 