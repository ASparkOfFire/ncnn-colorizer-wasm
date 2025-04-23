//
// Created by Kabir Kalsi on 4/23/25.
//

#ifndef MAIN_H
#define MAIN_H

#include <emscripten/emscripten.h>
#include <utils/utils.h>

#ifdef __cplusplus
extern "C" {
#endif

    EMSCRIPTEN_KEEPALIVE
    void colorize(const uint8_t* input_data, size_t input_size, uint8_t* output_data, size_t output_size);

#ifdef __cplusplus
}
#endif
EMSCRIPTEN_KEEPALIVE
   int main(int argc, char **argv);
#endif //MAIN_H