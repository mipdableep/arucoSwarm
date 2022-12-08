def main():

    new_ssid = input("input the new ssid:\n")
    new_paswrd = input("\ninput the new password\nif there is no password, press enter:\n")

    if new_paswrd == "":
        new_paswrd = "NONE"

    format1 = """ctrl_interface=DIR=/var/run/wpa_supplicant GROUP=netdev
update_config=1

network={
        ssid=\"\"
        psk=\"\"
}"""

    format2 = """ctrl_interface=DIR=/var/run/wpa_supplicant GROUP=netdev
update_config=1

network={
        ssid=\"\"
        key_mgmt=
}"""

    if new_paswrd != "NONE":
        format_new = format1[0:97] + new_ssid + format1[97:112] + new_paswrd + format1[112:]
    else:
        format_new = format2[0:97] + new_ssid + format2[97:116] + new_paswrd + format2[116:]
    
    with open("wpa_supplicant.conf", "w+") as f:
        f.write(format_new)

    # print(format_new)

if __name__ == "__main__":
    main()