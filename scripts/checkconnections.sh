echo -e "\nyour wpa config:\n"
cat /etc/wpa_supplicant/wpa_supplicant.conf | grep -e "ssid" -e "psk"
echo -e "\nyour tello config:\n"
cat /etc/wpa_supplicant/tello.conf | grep -e "ssid" -e "psk"
echo -e "\ncurrent server ip:\n"
cat /home/pi/arucoSwarm/bashrc_extension.sh | grep ipServerHost=
echo -e "\n"
