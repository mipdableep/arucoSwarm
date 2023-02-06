import socket
import subprocess
import sys

# Get the client's IP address
name = 'camera_test'

serverIpAddress = '10.3.141.188'

# Create a TCP/IP socket
sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

# Connect the socket to the port where the server is listening
server_address = ( serverIpAddress, 10000)
print('connecting to {} port {}'.format(*server_address))
sock.connect(server_address)

try:
    # Send the client's IP address to the server
    sock.sendall(name.encode("utf-8"))

finally:
    print('closing socket')
    sock.close()
