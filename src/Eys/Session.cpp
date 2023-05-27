#include "Session.hpp"

#include <iostream>
#include <boost/endian/conversion.hpp>
#include <boost/bind.hpp>
#include <chrono>

constexpr uint64_t expected_header = 0x5247424559455345; // "EYESBGR"

Session::Session(boost::asio::ip::tcp::socket socket)
    : socket_(std::move(socket))
{
}

Session::~Session() {
    std::cout << "[Session::Info] Session finished." << std::endl;
}

void Session::start() {
    process();
}

void Session::send_header() {

}

int Session::openCamera(cv::VideoCapture &cap) {
    // A variável cap é uma referência para um objeto VideoCapture, que é uma classe do OpenCV para captura de vídeo
    // Ele tem como argumento uma string com o pipeline do GStreamer usado para abrir a câmera Raspberry Pi
    // std::string pipeline = "v4l2src ! video/x-raw,framerate=30/1,width=640,height=480 ! videoconvert ! appsink";
    std::string pipeline = "v4l2src ! video/x-raw,framerate=30/1,width=640,height=480 ! videoflip method=5 ! videoconvert ! appsink";
    // std::string pipeline = "v4l2src device=/dev/video0 ! video/x-raw,framerate=30/1,width=640,height=480 ! videoconvert ! appsink";



    // Abre a câmera usando o GStreamer
    // cap.open(pipeline, cv::CAP_GSTREAMER);
    cap.open(0);
    
    // Aqui tem a verificação se a câmera foi aberta com sucesso
    // Se não foi, imprime uma mensagem de erro e retorna -1, que encerrará o programa na hora
    if (!cap.isOpened()) {
        std::cerr << "Erro ao abrir a câmera." << std::endl;
        return -1;
    }
    return 0;
}

int Session::process() {
    cv::VideoCapture cap;
    if (openCamera(cap) != 0) {
        return -1;
    }

    // Defina a taxa de quadros desejada
    const double frameRate = 8.0; // 8 quadros por segundo

    // Calcule o intervalo de tempo entre quadros em milissegundos
    const auto frameInterval = std::chrono::milliseconds(static_cast<int>(1000.0 / frameRate));

    while (true) {
        // Marque o tempo de início
        auto startTime = std::chrono::steady_clock::now();

        cv::Mat frame;
        cap >> frame;

        if (frame.empty()) {
            std::cerr << "[Session::Info] Frame is empty!" << std::endl;
            break;
        }

        send_frame(frame);

        // Calcule quanto tempo demorou para capturar e enviar o quadro
        auto endTime = std::chrono::steady_clock::now();
        auto elapsedTime = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);

        // Se demorou menos tempo do que o intervalo de quadros desejado, durma pelo tempo restante
        // if (elapsedTime < frameInterval) {
        //     std::this_thread::sleep_for(frameInterval - elapsedTime);
        // }
    }

    return 0;
}
void Session::send_frame(const cv::Mat& frame) {
    // A função send_frame recebe uma referência para o socket e uma referência para a imagem a ser enviada

    // Sempre que uma conexão for estabelecida, temos que enviar um cabeçalho para o cliente
    // indicando que a comunicação vai começar. Isso é feito para que o cliente saiba que
    // a conexão foi estabelecida e que ele pode começar a receber dados.
    // O cabeçalho é um valor fixo conhecido por ambos os lados da conexão.
    // Para facilitar a implementação, vamos usar o valor fixo e sequencial de 64 bits abaixo:
    uint64_t header = 0x123456789ABCDEF0;
    boost::asio::write(socket_, boost::asio::buffer(&header, sizeof(header)));

    // Sempre que formos enviar dados para o cliente, precisamos fazer algumas coisas:
    // 1. Ter um buffer adequado para armazenar os dados que serão enviados
    // 2. Converter o valor para a ordem de bytes da rede
    // 3. Enviar o tamanho do buffer para o cliente antes de enviar os dados
    // 4. Enviar os dados
    // Vamos seguir estes passos

    // 1. Ter um buffer adequado para armazenar os dados que serão enviados
    // O vetor buffer é igual ao frame, só que convertido para jpg
    std::vector<uchar> buffer;
    cv::imencode(".jpg", frame, buffer);

    // 2. Converter o valor para a ordem de bytes da rede
    // Isso é necessário porque a ordem de bytes da rede é diferente da ordem de bytes do computador.
    // Não conversar o valor para a ordem de bytes da rede pode causar erros na comunicação.
    std::size_t bufferSize = buffer.size();
    uint64_t bufferSize_network = be64toh(bufferSize); // Modifique esta linha

    // 3. Enviar o tamanho do buffer para o cliente antes de enviar os dados
    // Isso é necessário porque o cliente precisa saber quantos bytes ele deve esperar receber
    // antes de tentar ler os dados. Ou seja, ele sabe quando a comunicação começa e quando termina.
    boost::asio::write(socket_, boost::asio::buffer(&bufferSize_network, sizeof(bufferSize_network))); // Modifique esta linha

    // 4. Enviar os dados da imagem
    boost::asio::write(socket_, boost::asio::buffer(buffer.data(), bufferSize));

    // // Wait for 1 seg
    // std::this_thread::sleep_for(std::chrono::milliseconds(5));
}
