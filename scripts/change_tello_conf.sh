cd change_tello_conf/
python3 maker.py
sudo rm /etc/wpa_supplicant/tello.conf 
sudo mv tello.conf /etc/wpa_supplicant/

echo -e "\n/etc/wpa_supplicant/tello.conf:\n"
cat /etc/wpa_supplicant/tello.conf
echo -e '\n'