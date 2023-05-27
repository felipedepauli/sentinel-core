#include "Session.hpp"

#include <iostream>
#include <boost/endian/conversion.hpp>
#include <boost/bind.hpp>
#include <chrono>

constexpr uint64_t expected_header = 0x5247424559455345; // "EYESBGR"

Session::Session(boost::asio::ip::tcp::socket socket)
    : socket_(std::move(socket)), evilEys_()
{
    std::cout << "[Session::Info] Session created." << std::endl;
}

Session::~Session() {
    std::cout << "[Session::Info] Session finished." << std::endl;
}

void Session::start() {
    process();
}

int Session::process() {
    // If you want to know all the CV conf (done on compilation time)
    // std::cout << "CV " << cv::getBuildInformation() << std::endl;
    if (evilEys_.openEyes() != 0) {
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
        frame = evilEys_.spark();

        if (frame.empty()) {
            std::cerr << "[Session::Info] Frame is empty!" << std::endl;
            break;
        }

        send_frame(frame);

        // Calcule quanto tempo demorou para capturar e enviar o quadro
        auto endTime = std::chrono::steady_clock::now();
        auto elapsedTime = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);

        // Se demorou menos tempo do que o intervalo de quadros desejado, durma pelo tempo restante
        if (elapsedTime < frameInterval) {
            std::this_thread::sleep_for(frameInterval - elapsedTime);
        }
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
}
