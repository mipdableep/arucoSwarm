cd change_wpa_sup/
python3 maker.py 
sudo rm /etc/wpa_supplicant/wpa_supplicant.conf 
sudo mv wpa_supplicant.conf /etc/wpa_supplicant/

echo -e "\n/etc/wpa_supplicant/wpa_supplicant.conf:\n"
cat /etc/wpa_supplicant/wpa_supplicant.conf
echo -e '\n'