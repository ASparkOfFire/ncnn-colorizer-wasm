#include "net.h"
#include "colornet.h"

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/calib3d/calib3d.hpp>
#include <colornet/Sig17Slice.h>

#include <utility>
#include <vector>

ImageColorizer::ImageColorizer(std::string model_param, std::string model_bin)
    : model_param_(std::move(model_param)), model_bin_(std::move(model_bin)) {
    net_.opt.use_vulkan_compute = true;
    net_.register_custom_layer("Sig17Slice", Sig17Slice_layer_creator);
}

cv::Mat ImageColorizer::colorize(const cv::Mat &bgr_image) {
    if (net_.load_param(model_param_.c_str())) {
        std::cerr << "Failed to load model parameters" << std::endl;
        return {};
    }
    if (net_.load_model(model_bin_.c_str())) {
        std::cerr << "Failed to load model" << std::endl;
        return {};
    }

    cv::Mat lab, L, input_img;
    cv::Mat base_img = bgr_image.clone();

    base_img.convertTo(base_img, CV_32F, 1.0 / 255);
    cv::cvtColor(base_img, lab, cv::COLOR_BGR2Lab);
    cv::extractChannel(lab, L, 0);

    cv::resize(L, input_img, cv::Size(input_width_, input_height_));

    ncnn::Mat in_LAB_L(input_img.cols, input_img.rows, 1, (void *) input_img.data);
    in_LAB_L = in_LAB_L.clone();

    ncnn::Extractor ex = net_.create_extractor();
    ex.input("input", in_LAB_L);

    ncnn::Mat out;
    ex.extract("out_ab", out);

    cv::Mat colored_lab(out.h, out.w, CV_32FC2);
    memcpy((uchar *) colored_lab.data, out.data, out.w * out.h * 2 * sizeof(float));

    cv::Mat a(out.h, out.w, CV_32F, (float *) out.data);
    cv::Mat b(out.h, out.w, CV_32F, (float *) out.data + out.w * out.h);

    cv::resize(a, a, base_img.size());
    cv::resize(b, b, base_img.size());

    cv::Mat color, chn[] = {L, a, b};
    cv::merge(chn, 3, lab);
    cv::cvtColor(lab, color, cv::COLOR_Lab2BGR);
    color.convertTo(color, CV_8UC3, 255);

    return color;
}
