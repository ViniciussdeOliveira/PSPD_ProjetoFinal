#!/bin/bash
python3 - <<EOF
import socket
import struct
PORT = 8080
HOST = '127.0.0.1'
engine_type = 1  # 0=auto, 1=OpenMP, 2=MPI
size = 100
generations = 10
req = struct.pack('=BII', engine_type, size, generations)
s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.connect((HOST, PORT))
s.sendall(req)
data = s.recv(1024)
resp = struct.unpack('=Bf', data[:5])
print(f'Result: {resp[0]}, Time: {resp[1]:.4f}s')
s.close()
EOF 