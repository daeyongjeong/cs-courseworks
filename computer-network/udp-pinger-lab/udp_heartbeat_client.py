# udp_heartbeat_client.py
import time
from socket import *

# Configuration
serverName = '127.0.0.1'
serverPort = 12000
clientSocket = socket(AF_INET, SOCK_DGRAM)

# Ping loop
sequence_number = 1
while True:
    time.sleep(1)  # Send a heartbeat every second
    timestamp = time.time()
    heartbeat_message = f"Heartbeat {sequence_number} {timestamp}"
    
    # Send the ping message
    clientSocket.sendto(heartbeat_message.encode(), (serverName, serverPort))
    sequence_number += 1
