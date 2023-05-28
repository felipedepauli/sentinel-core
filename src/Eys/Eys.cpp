#include "Eys.hpp"

Eys::Eys() {
    std::cout << "[Eys::Info] Opening eyes!" << std::endl;
}
Eys::~Eys() {
    std::cout << "[Eys::Info] Closing eyes." << std:: endl;
}
int Eys::openEyes() {
    // A variável cap é uma referência para um objeto VideoCapture, que é uma classe do OpenCV para captura de vídeo
    // Ele tem como argumento uma string com o pipeline do GStreamer usado para abrir a câmera Raspberry Pi
    // std::string pipeline = "v4l2src ! video/x-raw,framerate=30/1,width=640,height=480 ! videoflip method=4 ! videoconvert ! appsink";
    std::string pipeline = "v4l2src ! video/x-raw,framerate=30/1,width=640,height=480 ! videoflip method=5 ! videoconvert ! appsink";
    // std::string pipeline = "v4l2src device=/dev/video0 ! video/x-raw,framerate=30/1,width=640,height=480 ! videoconvert ! appsink";
    cap.set(cv::CAP_PROP_BUFFERSIZE, 2); // O buffer interno armazenará apenas 3 frames


    // Abre a câmera usando o GStreamer
    cap.open(pipeline, cv::CAP_GSTREAMER);
    
    // Aqui tem a verificação se a câmera foi aberta com sucesso
    // Se não foi, imprime uma mensagem de erro e retorna -1, que encerrará o programa na hora
    if (!cap.isOpened()) {
        std::cerr << "[Eys::Error] Error opening eyes..." << std::endl;
        return -1;
    }
    return 0;
}

cv::Mat Eys::spark() {
    cv::Mat frame;
    cap >> frame;
    return frame;
}

