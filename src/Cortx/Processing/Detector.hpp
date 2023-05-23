/**
 * @file   Detector.hpp
 * @brief  The Detector header file.
 * @details This file contains the Detector class with all necessary methods for object detection.
 */

#ifndef DETECTOR_HPP
#define DETECTOR_HPP

#include <string>
#include <opencv2/opencv.hpp>

/**
 * @class  Detector
 * @brief  This class is responsible for object detection in the input frames.
 */
class Detector {
public:
    /**
     * @brief Constructs a new Detector object
     * @param classes_file Path to the file containing the names of the classes.
     * @param model_configuration Path to the file containing the model configuration.
     * @param model_weights Path to the file containing the model weights.
     */
    Detector(const std::string& classes_file, const std::string& model_configuration, const std::string& model_weights);

    /**
     * @brief Annotates the detected objects in the frame.
     * @details This function runs the detection model on the frame and adds bounding boxes 
     * and class labels to the objects detected in the frame.
     * @param frame The input frame.
     * @return The annotated frame.
     */
    cv::Mat annotate_frame(const cv::Mat& frame);

private:
    /**
     * @brief Labels for each possible class the model can predict.
     */
    std::vector<std::string> labels;
    
    /**
     * @brief The Deep Neural Network (DNN) that will be used for detection.
     */
    cv::dnn::Net network;
    
    /**
     * @brief The minimum probability to eliminate weak detections.
     */
    double probability_minimum;

    /**
     * @brief Threshold for the non maximum suppression algorithm.
     */
    double threshold;
    
    /**
     * @brief Colors for the bounding boxes for each class.
     */
    std::vector<cv::Scalar> colors;

    /**
     * @brief A blob from image to pass through the network for the detection.
     */
    cv::Mat blob;
};

#endif // DETECTOR_HPP
