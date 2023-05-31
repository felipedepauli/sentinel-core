import face_recognition
import cv2
import os
import glob

class Detector:
    def __init__(self):
        # Load sample images and extract face encodings
        print("[Auth::Info] Loading database...")
        self.known_face_encodings = []
        self.known_face_names = []
        self.known_face_ids = []

        # Get list of all .jpg files in the specified directory
        files = glob.glob("../Think/Memory/storage/faces_bd/*.jpg")
        
        for file in files:
            # Extract the id, name and encoding from each file
            face_id, face_name = os.path.basename(file).split('__')
            face_name = face_name.replace('_', ' ').replace('.jpg', '')
            face_encoding = self.load_encoding(file)
            
            if face_encoding is not None:
                self.known_face_encodings.append(face_encoding)
                self.known_face_names.append(face_name)
                self.known_face_ids.append(face_id)

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
            
            
        # Se houver um arquivo ../../Think/Memory/storage/faces_bd/synapse
        # enviar o id da pessoa identificada para esse arquivo (é um arquivo FIFO)
        fifo_path = "../Think/Memory/storage/faces_bd/synapse"
        if os.path.exists(fifo_path):
            print("Check person")
            if True in all_matches:
                first_match_index = all_matches.index(True)
                id = self.known_face_ids[first_match_index]
                with open(fifo_path, 'w') as fifo:
                    fifo.write(id)
        
        return frame, 10
