/**
 * @file   Detector.cpp
 * @brief  Contains the implementation of the methods for the Detector class.
 */

#include "Detector.hpp"
#include <fstream>
#include <sstream>
#include <random>

/**
 * @brief Constructor for the Detector class
 * @param labels_ - Path to the file with the labels for the classes.
 * @param yolo_cfg - Path to the Yolo configuration file.
 * @param yolo_weights - Path to the Yolo weights file.
 * 
 * The constructor initializes the Detector with the provided labels, Yolo configuration, and Yolo weights.
 * It also sets the minimum probability and threshold values, generates random colors for the labels,
 * and loads the Yolo neural network with the given configuration and weights.
 */
Detector::Detector(const std::string& labels_, const std::string& yolo_cfg, const std::string& yolo_weights)
    : probability_minimum(0.5), threshold(0.3) {
    // Step 01. Load Yolo neural network
    network = cv::dnn::readNetFromDarknet(yolo_cfg, yolo_weights);

    // Step 02. Load class labels from file
    std::ifstream ifs(labels_.c_str());
    std::string line;
    while (std::getline(ifs, line)) {
        labels.push_back(line);
    }

    // Step 03. Generate random colors for the labels
    std::random_device rd;
    std::mt19937 rng(rd());
    std::uniform_int_distribution<int> uni(0, 255);
    for (int i = 0; i < labels.size(); ++i) {
        colors.push_back(cv::Scalar(uni(rng), uni(rng), uni(rng)));
    }
}

/**
 * @brief Annotate a frame with detected objects
 * @param frame - Frame to be annotated
 * @return Annotated frame
 * 
 * This function takes an input frame, converts it into a blob to feed into the neural network, 
 * performs a forward-pass through the network, gets the output and for each detected object, 
 * draws bounding boxes and labels on the frame, and then returns the annotated frame.
 */
cv::Mat Detector::annotate_frame(const cv::Mat& frame) {
    // Step 01. Convert frame to blob
    // cv::dnn::blobFromImage(frame, blob, 1 / 255.0, cv::Size(416, 416), true, false);
    cv::dnn::blobFromImage(frame, blob, 1 / 255.0, cv::Size(416, 416), cv::Scalar(0, 0, 0), true, false);
    // I used the cv::dnn::blobFromImage function to convert the input image to a 4-dimensional blob. This blob is then passed as input to the network. This technique can lead to performance improvements because it prepares the image data in a format that's more optimal for the network to process.

    // Step 02. Set blob as input to the network
    network.setInput(blob);

    // Step 03. Perform a forward-pass and get output
    std::vector<cv::Mat> output_from_network;
    network.forward(output_from_network, network.getUnconnectedOutLayersNames());

    // Step 04. Extract bounding boxes and labels
    std::vector<int> class_ids;
    std::vector<float> confidences;
    std::vector<cv::Rect> boxes;

    // Iterate over each output (layer) after the forward-pass
    for (auto& output : output_from_network) {

        // Iterate over each detection of the output
        auto* data = (float*)output.data;
        for (int j = 0; j < output.rows; ++j, data += output.cols) {

            // Step 05. Get class scores and class id
            cv::Mat scores = output.row(j).colRange(5, output.cols);
            cv::Point classIdPoint;

            double confidence;

            // Step 06. Get maximum score and its index
            cv::minMaxLoc(scores, 0, &confidence, 0, &classIdPoint);
            if (confidence > probability_minimum) {

                // Step 07. Extract bounding box parameters
                int centerX = (int)(data[0] * frame.cols);
                int centerY = (int)(data[1] * frame.rows);
                int width = (int)(data[2] * frame.cols);
                int height = (int)(data[3] * frame.rows);
                int left = centerX - width / 2;
                int top = centerY - height / 2;

                // Step 08. Save class id, confidence and box location
                class_ids.push_back(classIdPoint.x);
                confidences.push_back((float)confidence);
                boxes.push_back(cv::Rect(left, top, width, height));
            }
        }
    }

    // Step 09. Apply non-maxima suppression
    std::vector<int> indices;
    cv::dnn::NMSBoxes(boxes, confidences, (float)probability_minimum, (float)threshold, indices);

    // Step 10. Draw bounding boxes and labels on the frame
    for (int idx : indices) {
        cv::Rect box = boxes[idx];
        int class_id = class_ids[idx];

        cv::rectangle(frame, box, colors[class_id], 4);
        std::string label = labels[class_id] + ": " + std::to_string(confidences[idx]);
        int baseLine;
        cv::Size labelSize = cv::getTextSize(label, cv::FONT_HERSHEY_SIMPLEX, 0.5, 1, &baseLine);

        cv::rectangle(frame, cv::Rect(cv::Point(box.x, box.y - labelSize.height),
                                      cv::Size(labelSize.width, labelSize.height + baseLine)),
                      colors[class_id], cv::FILLED);
        cv::putText(frame, label, box.tl(), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar());
    }

    // Return the annotated frame
    return frame;
}

