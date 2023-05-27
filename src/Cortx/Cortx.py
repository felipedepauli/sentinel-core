import socket
import struct
import cv2
import numpy as np
import sys

# Criar um socket TCP/IP
sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

# Conectar o socket ao endereço do servidor
server_address = ('localhost', 30001)  # Substitua pelo IP e porta do seu servidor
sock.connect(server_address)

try:
    # Receber o cabeçalho
    data = sock.recv(8)
    header = struct.unpack('Q', data)[0]
    print('received header: ', hex(header))
    
    # Verificar o cabeçalho
    if header != 0x123456789ABCDEF0:
        print('Invalid header')
        sys.exit(1)
    
    # Receber o tamanho do buffer
    data = sock.recv(8)
    size = struct.unpack('Q', data)[0]
    print('received size: ', size)

    # Receber os dados da imagem
    data = b''
    while len(data) < size:
        packet = sock.recv(size - len(data))
        if not packet:
            break
        data += packet

    # Decodificar a imagem
    nparr = np.frombuffer(data, np.uint8)
    img = cv2.imdecode(nparr, cv2.IMREAD_COLOR)

    # Mostrar a imagem
    cv2.imshow('Received Image', img)
    cv2.waitKey(1)

finally:
    # Fechar o socket
    sock.close()
