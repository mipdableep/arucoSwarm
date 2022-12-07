import sys



def main():

    new_ssid = input("input the Tello's serial number (after the - ):\n")

    format = """ctrl_interface=DIR=/var/run/wpa_supplicant GROUP=netdev
update_config=1

network={
        ssid=\"TELLO-\"
        psk=\"NONE\"
}"""
    # print (format.find("psk"))

    format_new = format[0:103] + new_ssid + format[103:]
    with open("tello.conf", "w+") as f:
        f.write(format_new)

main()

