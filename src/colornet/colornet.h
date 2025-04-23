#ifndef COLORNET_H_INCLUDED
#define COLORNET_H_INCLUDED

#include <string>
#include <opencv2/core/core.hpp>
#include <ncnn/net.h>

// ImageColorizer class definition
class ImageColorizer {
public:
    // Constructor
    ImageColorizer(std::string  model_param, std::string  model_bin);

    // Perform colorization on a grayscale image
    cv::Mat colorize(const cv::Mat& bgr_image);

private:
    ncnn::Net net_;                 // The NCNN network
    std::string model_param_;      // Path to the .param file
    std::string model_bin_;        // Path to the .bin file
    const int input_width_ = 256;  // Expected input width
    const int input_height_ = 256; // Expected input height
};

#endif // COLORNET_H_INCLUDED
