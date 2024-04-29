# udp_pinger_stats.py
import time
from socket import *

# Configuration
serverName = '127.0.0.1'
serverPort = 12000
clientSocket = socket(AF_INET, SOCK_DGRAM)
clientSocket.settimeout(1)  # Set timeout to 1 second

rtts = []  # List to store RTTs for calculating stats
lost_packets = 0  # Counter for lost packets

# Ping loop
for sequence_number in range(1, 11):
    send_time = time.time()
    message = f'Ping {sequence_number} {send_time}'
    try:
        # Send the ping message
        clientSocket.sendto(message.encode(), (serverName, serverPort))

        # Receive the response
        modifiedMessage, serverAddress = clientSocket.recvfrom(1024)
        recv_time = time.time()

        # Calculate and print RTT
        rtt = recv_time - send_time
        rtts.append(rtt)
        print(f"Received from server: {modifiedMessage.decode()}")
        print(f"RTT: {rtt:.6f} seconds")
    except timeout:
        # Handle timeout (packet loss)
        lost_packets += 1
        print("Request timed out")

# Calculate and print statistics
if rtts:
    print(f"Minimum RTT: {min(rtts):.6f} seconds")
    print(f"Maximum RTT: {max(rtts):.6f} seconds")
    print(f"Average RTT: {sum(rtts) / len(rtts):.6f} seconds")
print(f"Packet loss rate: {(lost_packets / 10) * 100:.2f}%")

# Close the socket
clientSocket.close()
