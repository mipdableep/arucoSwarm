import sys



def main():

    new_ssid = input("input the new ssid:\n")
    new_paswrd = input("\ninput the new password\nif there is no password, press enter:\n")

    if new_paswrd == "":
        new_paswrd = "NONE"

    format = """ctrl_interface=DIR=/var/run/wpa_supplicant GROUP=netdev
update_config=1

network={
        ssid=\"\"
        psk=\"\"
}"""
    # print (format.find("psk"))

    format_new = format[0:97] + new_ssid + format[97:112] + new_paswrd + format[112:]
    with open("wpa_supplicant.conf", "w+") as f:
        f.write(format_new)

main()

