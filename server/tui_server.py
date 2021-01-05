#!/usr/bin/env python
import configparser
import socket
import sys

config = configparser.ConfigParser()
config.read('config.ini')

socketConfig = config['Socket']
hostIP = socketConfig['IP']
port = int(socketConfig['Port'])
bufferSize = int(socketConfig['BufferSize'])

try:
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
except OSError as msg:
    sys.exit(1)

try:
    s.setsockopt(socket.SOL_SOCKET, socket.SO_SNDBUF, bufferSize)
    s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    s.bind((hostIP, port))
except OSError as msg:
    s.close()
    sys.exit(1)

while True:
    try:
        s.listen(1)
        conn, clientIP = s.accept()
        #print('Connected by', clientIP)
    except OSError as msg:
        s.close()
        sys.exit(1)

    data = conn.recv(bufferSize).decode().rstrip('\0')
    code = data[0]
    print_str = data[1:]

    if code == 'P':
        print(print_str)
    elif code == 'I':
        conn.send(input(print_str).encode())
    elif code == 'S':
        conn.send("y".encode())

    conn.close()
