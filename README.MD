


activate server:

open an hotspot from a phone

go to {/etc/wpa_supplicant/wpa_supplicant.conf} on the rpi0 and change the following fields:

network={
to new ssid     -> ssid="SSID"
to new password ->  psk="****"
}

OR run the change_wpa_sup script

connect to the hotspot thrugh another device, and check your ip.
you can do it bu runing ${hostname -I | cut -b 1-13}
in bashrc_extension change ipServerHost to your ip

after all is done, run the following:

python3 server.py 0.0.0.0 3
                          ^
                          |
            this is the number of drones 