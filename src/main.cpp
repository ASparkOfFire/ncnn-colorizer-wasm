#include <iostream>
#include "colornet/colornet.h"
#include "main.h"
#include <emscripten/emscripten.h>
#include <opencv2/opencv.hpp>

ImageColorizer* colorizer;  // Use pointer to ImageColorizer

extern "C" EMSCRIPTEN_KEEPALIVE
void colorize(const uint8_t* input_data, size_t input_size, uint8_t* output_data, size_t output_size) {
    // Step 1: Copy input data to a buffer
    Buffer input_buf;
    input_buf.data = static_cast<uint8_t*>(malloc(input_size));
    input_buf.size = input_size;

    memcpy(input_buf.data, input_data, input_size);

    // Step 2: Convert to cv::Mat and free temp buffer
    const cv::Mat input_mat = buffer_to_mat(input_buf);
    free_buffer(input_buf);

    // Step 3: Colorize using your model
    const cv::Mat output_mat = colorizer->colorize(input_mat);

    // Step 4: Convert to buffer
    Buffer output_buf = mat_to_buffer(output_mat, ".jpg");

    // Step 5: Ensure output buffer is large enough
    if (output_buf.size > output_size) {
        // You can signal error with a global status or log
        fprintf(stderr, "Output buffer too small: %zu < %zu\n", output_size, output_buf.size);
        free_buffer(output_buf);
        return;
    }

    // Step 6: Copy result to output_data
    memcpy(output_data, output_buf.data, output_buf.size);
    free_buffer(output_buf);
}

// Add this to your C++ code (main.cpp)
extern "C" EMSCRIPTEN_KEEPALIVE
void colorize_rgba(const uint8_t* rgba_data, int width, int height,
                   uint8_t* output_rgb, size_t output_size) {
    // Create Mat directly from RGBA data
    cv::Mat input_rgba(height, width, CV_8UC4, (void*)rgba_data);

    // Convert RGBA to BGR (OpenCV's preferred format)
    cv::Mat input_bgr;
    cv::cvtColor(input_rgba, input_bgr, cv::COLOR_RGBA2BGR);

    // Colorize using existing colorizer
    cv::Mat output_bgr = colorizer->colorize(input_bgr);

    // Convert result from BGR to RGB
    cv::Mat output_rgb_mat;
    cv::cvtColor(output_bgr, output_rgb_mat, cv::COLOR_BGR2RGB);

    // Copy data to output buffer
    // Ensure output_size is large enough (width * height * 3)
    size_t required_size = width * height * 3;
    if (output_size < required_size) {
        fprintf(stderr, "Output buffer too small: %zu < %zu\n", output_size, required_size);
        return;
    }

    memcpy(output_rgb, output_rgb_mat.data, required_size);
}

int main(int argc, char **argv) {
    // Initialize colorizer with model paths
    colorizer = new ImageColorizer("/model/model.param", "/model/model.bin");
    std::cout << "Welcome to Colornet.\n";
}
