import face_recognition
import cv2
import os

class Detector:
    def __init__(self):
        # Load sample images and extract face encodings
        print("[Auth::Info] Loading database...")
        self.amorinha_face_encoding         = self.load_encoding("../Think/Memory/storage/pics/amorinha/nenem_16.jpeg")
        self.fabricio_face_encoding         = self.load_encoding("../Think/Memory/storage/pics/Fabricio.jpg")
        self.rafael_face_encoding           = self.load_encoding("../Think/Memory/storage/pics/Rafael.jpg")
        self.modi_face_encoding             = self.load_encoding("../Think/Memory/storage/pics/Modi.jpg")
        self.trump_face_encoding            = self.load_encoding("../Think/Memory/storage/pics/Trump.jpg")
        self.felps_face_encoding            = self.load_encoding("../Think/Memory/storage/pics/Felps.jpg")
        self.boechat_face_encoding          = self.load_encoding("../Think/Memory/storage/pics/Boechat.jpg")
        self.dePauli_face_encoding          = self.load_encoding("../Think/Memory/storage/pics/dePauli.jpg")
        self.heitorzimGamerBr_face_encoding = self.load_encoding("../Think/Memory/storage/pics/HeitorzimGamerBr.jpeg")

        self.known_face_encodings = [
            self.amorinha_face_encoding,           # 0
            self.fabricio_face_encoding,           # 1
            self.rafael_face_encoding,             # 2
            self.modi_face_encoding,               # 3
            self.trump_face_encoding,              # 4
            self.felps_face_encoding,              # 5
            self.boechat_face_encoding,            # 6
            self.dePauli_face_encoding,            # 7
            self.heitorzimGamerBr_face_encoding    # 8
        ]

        self.known_face_names = [
            "Helena Vieira",            # 0
            "Fabricio Pianovski",       # 1
            "Rafael Campanhola",        # 2
            "Narendra Modi",            # 3
            "Donald Trump",             # 4
            "Felipe de Pauli",          # 5
            "Ricardo Boechat",          # 6
            "Edson de Pauli",           # 7
            "Heitorzim da Massa BR"     # 8
        ]

        # Loaded sample images and extracted face encodings
        print("[Auth::Info] Database loaded")
        
    def load_encoding(self, image_path):
        image = face_recognition.load_image_file(image_path)
        encodings = face_recognition.face_encodings(image)
        if encodings:
            return encodings[0]
        else:
            print(f"No face found in {image_path}.")
            return None

    def annotate_frame(self, frame):
        # Resize frame of video to 1/4 size for faster face detection processing
        small_frame = cv2.resize(frame, (0, 0), fx=0.25, fy=0.25)

        # Detect all faces in the image
        all_face_locations = face_recognition.face_locations(small_frame, model='HOG')
        all_face_encodings = face_recognition.face_encodings(small_frame, all_face_locations)

        for current_face_location, current_face_encoding in zip(all_face_locations, all_face_encodings):
            # Splitting the tuple to get the four position values of current face
            top_pos, right_pos, bottom_pos, left_pos = current_face_location
            top_pos *= 4
            right_pos *= 4
            bottom_pos *= 4
            left_pos *= 4
        
            # Compare faces and get the matches list
            all_matches = face_recognition.compare_faces(self.known_face_encodings, current_face_encoding)

            # Initialize a name string as unknown face
            name = "Unknown Face"
            
            # User first match and get name from the respective index
            if True in all_matches:
                first_match_index = all_matches.index(True)
                name = self.known_face_names[first_match_index]
            
            if (name == "Unknown Face"):
                continue
        
            # Draw rectangle around the face detected
            cv2.rectangle(frame, (left_pos, top_pos), (right_pos, bottom_pos), (255, 0, 0), 2)

            # Write name below each face
            font = cv2.FONT_HERSHEY_DUPLEX
            if (name == "Unknown Face"):
                continue
            cv2.putText(frame, name, (left_pos, bottom_pos), cv2.FONT_HERSHEY_SIMPLEX, 0.5, (255,255,255), 1)
        
        return frame, 10
