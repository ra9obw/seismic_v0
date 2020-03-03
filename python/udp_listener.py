import sys
import traceback
# print(sys.path)
import socket
from time import sleep

def listen_udp(host_ip, host_port):
    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    sock.bind(('', host_port))

    while True:
        data = sock.recv(128)
        if not data:
            break
        print(data)

    sock.close()

if __name__ == '__main__':
    import msvcrt
    import csv

    listen_udp("192.168.1.15", 1500)