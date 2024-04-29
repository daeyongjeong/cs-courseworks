# udp_heartbeat_server.py
import time
from socket import *

# Create a UDP socket 
# Notice the use of SOCK_DGRAM for UDP packets
serverSocket = socket(AF_INET, SOCK_DGRAM)
# Assign IP address and port number to socket
serverSocket.bind(('', 12000))

print("Heartbeat server is running...")

last_received = time.time()
timeout = 3

while True:
    serverSocket.settimeout(timeout)
    try:
        message, address = serverSocket.recvfrom(1024)
        current_time = time.time()
        print(f"Received {message.decode()} from {address}")
        last_received = current_time
    except timeout:
        print("Heartbeat timeout: No Heartbeat received. Client may be down.")
        break
    