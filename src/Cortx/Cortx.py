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

async def connect():
    while True:
        try:
            async with websockets.connect('ws://localhost:8080/python') as websocket:
                print("[Cortx::Info] Starting to detect people...")

                while True:
                    # Lê o cabeçalho
                    header = s.recv(8)  # O tamanho do cabeçalho é de 8 bytes (64 bits)
                    header_value = struct.unpack('!Q', header)[0]  # Desempacota como um uint64 big-endian

                    # Se o cabeçalho não for o esperado, ignora o restante do loop
                    header_value_bytes = header_value.to_bytes(8, 'big')
                    header_value_bytes_reversed = header_value_bytes[::-1]
                    header_value_reversed = int.from_bytes(header_value_bytes_reversed, 'big')
                    if header_value_reversed != 0x5247424559455345:
                        print('Cabeçalho inválido recebido')
                        continue

                    # Lê o tamanho do frame
                    frame_size_data = s.recv(8)  # O tamanho do frame é de 8 bytes (64 bits)
                    frame_size = struct.unpack('!Q', frame_size_data)[0]  # Desempacota como um uint64 big-endian

                    # Lê os dados do frame
                    frame_data = b''
                    while len(frame_data) < frame_size:
                        chunk = s.recv(frame_size - len(frame_data))
                        if not chunk:
                            break
                        frame_data += chunk

                    # Certifica-se de que o frame inteiro foi lido
                    if len(frame_data) != frame_size:
                        print('Frame incompleto recebido')
                        break

                    # Converte os dados do frame em uma imagem
                    nparr = np.frombuffer(frame_data, np.uint8)
                    img_np = cv2.imdecode(nparr, cv2.IMREAD_COLOR)

                    annotated_frame, id = detector.annotate_frame(img_np)
                    if annotated_frame.size > 0:  # Verifica se o frame não está vazio
                        # Codifica o frame anotado como JPG
                        is_success, buffer = imencode(".jpg", annotated_frame)
                        
                        # Verifica se a codificação foi bem-sucedida
                        if is_success:
                            # Envia o resultado para o servidor Node.js via WebSocket
                            data = {"id": id, "frame": base64.b64encode(buffer).decode()}
                            await websocket.send(json.dumps(data))
                            # print(buffer.tobytes()[:10].hex())
                        else:
                            print("Falha ao codificar o frame como .jpg")
                    else:
                        print("Frame vazio")
                
                    # # Exibe a imagem
                    # cv2.imshow('image', annotated_frame)
                    # cv2.waitKey(1)  # Aguarda por 1 ms para processar quaisquer eventos de janela

        except websockets.exceptions.ConnectionClosedError:
            print("[Cortx::Info] Connection closed. Trying to reconnect...")
            await asyncio.sleep(1)
    # Inicia o loop de eventos do asyncio
asyncio.get_event_loop().run_until_complete(connect())