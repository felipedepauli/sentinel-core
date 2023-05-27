import cv2
import numpy as np
import struct
import socket
import time
import face_recognition

class Detector:
    def __init__(self):
        # Load sample images and extract face encodings
        self.modi_face_encoding = self.load_encoding("../Think/Memory/pics/Modi.jpg")
        self.trump_face_encoding = self.load_encoding("../Think/Memory/pics/Trump.jpg")
        self.felps_face_encoding = self.load_encoding("../Think/Memory/pics/Felps.jpg")
        self.boechat_face_encoding = self.load_encoding("../Think/Memory/pics/Boechat.jpg")

        # Create arrays of known face encodings and their names
        self.known_face_encodings = [self.modi_face_encoding, self.trump_face_encoding, self.felps_face_encoding, self.boechat_face_encoding]
        self.known_face_names = ["Narendra Modi", "Donald Trump", "Felps de Pauli", "Ricardo Boechat"]
    
    def load_encoding(self, image_path):
        print(f'[Detector] Loading encoding from {image_path}')
        image = face_recognition.load_image_file(image_path)
        encoding = face_recognition.face_encodings(image)[0]
        print(f'[Detector] Loaded encoding from {image_path}')
        return encoding

    def annotate_frame(self, frame):
        print('[Detector] Annotating frame')
        small_frame = cv2.resize(frame, (0, 0), fx=0.25, fy=0.25)

        all_face_locations = face_recognition.face_locations(small_frame, model='HOG')
        all_face_encodings = face_recognition.face_encodings(small_frame, all_face_locations)

        for current_face_location, current_face_encoding in zip(all_face_locations, all_face_encodings):
            top_pos, right_pos, bottom_pos, left_pos = current_face_location
            top_pos *= 4
            right_pos *= 4
            bottom_pos *= 4
            left_pos *= 4
        
            all_matches = face_recognition.compare_faces(self.known_face_encodings, current_face_encoding)

            name = "Unknown Face"
            
            if True in all_matches:
                first_match_index = all_matches.index(True)
                name = self.known_face_names[first_match_index]
            
            if name == "Unknown Face":
                continue

            cv2.rectangle(frame, (left_pos, top_pos), (right_pos, bottom_pos), (255, 0, 0), 2)
            cv2.putText(frame, name, (left_pos, bottom_pos), cv2.FONT_HERSHEY_SIMPLEX, 0.5, (255,255,255), 1)
        
        print('[Detector] Annotated frame')
        return frame


class Cortx:
    def __init__(self, host, port):
        print(f"[Cortx] Initializing Cortx with host: {host} and port: {port}...")
        self.host = host
        self.port = port
        self.detector = Detector()
        
    def receive_all(self, sock, bufsize, timeout=2.0):
        sock.settimeout(timeout)
        data = b''
        bytes_received = 0
        while len(data) < bufsize:
            try:
                packet = sock.recv(min(bufsize - len(data), 4096))
                if not packet:
                    print(f"[Cortx] Received empty packet after receiving {bytes_received} bytes.")
                    break
                data += packet
                bytes_received += len(packet)
                print(f"[Cortx] Received {len(packet)} bytes, total so far: {bytes_received}")
            except socket.timeout:
                print(f"[Cortx] Receive operation timed out after {timeout} seconds.")
                break
        return data


    def run(self):
        try:
            print(f"[Cortx] Opening socket and connecting to server at {self.host}:{self.port}...")
            s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            s.connect((self.host, self.port))

            print(f"[Cortx] Opening FIFO file for binary write...")
            fifo = open("/tmp/synapse", "wb")

            frame_number = 0
            while True:
                print(f"[Cortx:Info] Reading header...")
                header_data = self.receive_all(s, 8)  # Changed from s.recv(8)
                header = struct.unpack('<Q', header_data)[0]

                print(f"[Cortx:Info] Received header: {header}")

                if header != 0x123456789ABCDEF0:
                    print("[Cortx:Info] Invalid header received. Breaking the loop...")
                    break

                print(f"[Cortx:Info] Receiving buffer size...")
                bufferSize_data = self.receive_all(s, 8)  # Changed from s.recv(8)
                bufferSize = struct.unpack('<Q', bufferSize_data)[0]

                print(f"[Cortx:Info] Receiving image data...")
                buffer = self.receive_all(s, bufferSize)

                print(f"[Cortx:Info] Decoding image...")
                nparr = np.frombuffer(buffer, np.uint8)
                frame = cv2.imdecode(nparr, cv2.IMREAD_COLOR)
                cv2.imshow('Image', frame)
                cv2.waitKey(1)  

                if frame is not None and not frame.size == 0:
                    print(f"[Cortx] Detecting objects in the frame...")
                    annotated_frame = self.detector.annotate_frame(frame)

                    if annotated_frame is not None and not annotated_frame.size == 0:
                        print(f"[Cortx] Encoding annotated frame as a JPG...")
                        retval, buf = cv2.imencode(".jpg", annotated_frame)
                        
                        print(f"[Cortx] Writing the result to FIFO...")
                        fifo.write(buf.tobytes())
                        fifo.flush()

                frame_number += 1
                if frame_number % 100 == 0:
                    print(f"[Cortx] Processed {frame_number} frames...")
            
            print("[Cortx] Closing FIFO file...")
            fifo.close()
        except OverflowError:
            print("[Cortx] Error: Buffer size too large!")

        except Exception as e:
            print(f"[Cortx] Error occurred! Message: {str(e)}")

if __name__ == "__main__":
    cortx = Cortx('192.168.137.2', 30001)
    cortx.run()
