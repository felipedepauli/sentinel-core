import asyncio
import socket
import struct
import cv2
import websockets
import numpy as np
from Processing.TemporalLobe import Detector
from cv2 import imencode
import json
import base64
import sys

# Check the number of command-line arguments
if len(sys.argv) < 3:
    print("usage: python programa.py [ip] [port]")
    sys.exit(1)

# Get the command-line argument
ip = sys.argv[1]
port = int(sys.argv[2])

# Connect to the server
s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.connect((ip, port))

# Initialize the Detector object
detector = Detector()

# Define the main function
async def connect():
    while True:
        try:
            async with websockets.connect('ws://localhost:8080/python') as websocket:
                print("[Cortx::Info] Starting to detect people...")

                while True:
                    # Read the header
                    header = s.recv(8)
                    header_value = struct.unpack('!Q', header)[0]

                    # Check the header value
                    header_value_bytes = header_value.to_bytes(8, 'big')
                    header_value_bytes_reversed = header_value_bytes[::-1]
                    header_value_reversed = int.from_bytes(header_value_bytes_reversed, 'big')
                    if header_value_reversed != 0x5247424559455345:
                        print('Invalid header received')
                        continue

                    # Read the frame size
                    frame_size_data = s.recv(8)
                    frame_size = struct.unpack('!Q', frame_size_data)[0]

                    # Read the frame data
                    frame_data = b''
                    while len(frame_data) < frame_size:
                        chunk = s.recv(frame_size - len(frame_data))
                        if not chunk:
                            break
                        frame_data += chunk

                    # Check if the whole frame was read
                    if len(frame_data) != frame_size:
                        print('Incomplete frame received')
                        break

                    # Convert the frame data to an image
                    nparr = np.frombuffer(frame_data, np.uint8)
                    img_np = cv2.imdecode(nparr, cv2.IMREAD_COLOR)

                    # Annotate the frame
                    annotated_frame = detector.annotate_frame(img_np)
                    if annotated_frame.size > 0:
                        # Encode the annotated frame as JPG
                        is_success, buffer = imencode(".jpg", annotated_frame)
                        if is_success:
                            # Send the result to the Node.js server via WebSocket
                            data = {"id": id, "frame": base64.b64encode(buffer).decode()}
                            await websocket.send(json.dumps(data))
                        else:
                            print("Failed to encode the frame as .jpg")
                    else:
                        print("Empty frame")

        except websockets.exceptions.ConnectionClosedError:
            print("[Cortx::Info] Connection closed. Trying to reconnect...")
            await asyncio.sleep(1)

# Start the asyncio event loop
asyncio.get_event_loop().run_until_complete(connect())