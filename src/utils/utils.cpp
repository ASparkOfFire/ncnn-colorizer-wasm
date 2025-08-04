//
// Created by Kabir Kalsi on 4/23/25.
//

#include "utils.h"
#include <opencv2/imgcodecs.hpp>
#include <vector>
#include <iostream>

// Convert cv::Mat to a buffer (JPG or PNG)
Buffer mat_to_buffer(const cv::Mat& input, const std::string& format = ".jpg") {
    Buffer result;

    if (input.empty()) {
        std::cerr << "Input image is empty!" << std::endl;
        return {};
    }

    // Encode the image to the specified format (JPG or PNG)
    std::vector<uchar> buffer;
    bool success = cv::imencode(format, input, buffer);
    if (!success) {
        std::cerr << "Failed to encode the image." << std::endl;
        return {};
    } else {
        std::cout << "Image encoded successfully, buffer size: " << buffer.size() << std::endl;
    }

    result.size = buffer.size();
    result.data = static_cast<uint8_t*>(malloc(result.size));
    memcpy(result.data, buffer.data(), result.size);

    return result;
}


cv::Mat buffer_to_mat(const Buffer buffer) {
    // Debugging: Log buffer size
    std::cout << "Buffer size: " << buffer.size << " bytes." << std::endl;

    // Debugging: Check if the buffer is empty or invalid
    if (buffer.size == 0 || buffer.data == nullptr) {
        std::cerr << "Invalid buffer! Buffer size is 0 or data is null." << std::endl;
        return {};  // Return an empty Mat if the buffer is invalid
    }

    const std::vector<uchar> data(buffer.data, buffer.data + buffer.size);

    // Debugging: Log a few bytes of the buffer to see if it looks like valid image data
    std::cout << "First few bytes of the buffer: ";
    for (size_t i = 0; i < std::min(data.size(), size_t(20)); ++i) {
        std::cout << std::hex << int(data[i]) << " ";
    }
    std::cout << std::dec << std::endl; // Reset to decimal format for future logs

    // Debugging: Log the size of data being passed to imdecode
    std::cout << "Decoding buffer data, size: " << data.size() << " bytes." << std::endl;

    cv::Mat decodedImage = cv::imdecode(data, cv::IMREAD_UNCHANGED);

    // Debugging: Check if decoding was successful
    if (decodedImage.empty()) {
        std::cerr << "Failed to decode the image from the buffer." << std::endl;
    } else {
        std::cout << "Image decoded successfully. Dimensions: " << decodedImage.rows
                  << "x" << decodedImage.cols << ", Type: " << decodedImage.type() << std::endl;
    }

    return decodedImage;
}

EMSCRIPTEN_KEEPALIVE
void free_buffer(Buffer buffer) {
    if (buffer.data) {
        free(buffer.data);
        buffer.data = nullptr;
    }
}
