#include <boost/asio.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgcodecs/imgcodecs.hpp>

using boost::asio::ip::tcp;

class Session : public std::enable_shared_from_this<Session> {
public:
    Session(boost::asio::ip::tcp::socket socket);
    ~Session();

    void start();

private:
    int openCamera(cv::VideoCapture &cap);
    void send_header(); 
    void send_frame(const cv::Mat& frame);
    int process();

    boost::asio::ip::tcp::socket socket_;
    uint64_t received_header;
};

