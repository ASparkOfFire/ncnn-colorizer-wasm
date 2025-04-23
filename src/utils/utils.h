//
// Created by Kabir Kalsi on 4/23/25.
//

#ifndef UTILS_H
#define UTILS_H
#include <emscripten/emscripten.h>
#include <opencv2/core/mat.hpp>

typedef struct {
    uint8_t* data;
    size_t size;
} Buffer;

Buffer mat_to_buffer(const cv::Mat& input, const std::string& format);
cv::Mat buffer_to_mat(Buffer buffer);

EMSCRIPTEN_KEEPALIVE
void free_buffer(Buffer buffer);

#endif //UTILS_H
