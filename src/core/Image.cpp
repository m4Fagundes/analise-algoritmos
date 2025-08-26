// src/core/Image.cpp

#include "Image.h"
#include "Vector.h"
#include <stdexcept>
#include <iostream>
#include <numeric>


#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

FeatureVector extractFeatures(const std::string& image_path, int bins_per_channel) {
    int width, height, channels;
    unsigned char *image_data = stbi_load(image_path.c_str(), &width, &height, &channels, 3);

    if (image_data == nullptr) {
        throw std::runtime_error("Falha ao carregar a imagem: " + image_path);
    }

    int histogram_size = bins_per_channel * bins_per_channel * bins_per_channel;
    FeatureVector histogram(histogram_size, 0.0f);

    int bin_size = 256 / bins_per_channel;

    for (int i = 0; i < width * height; ++i) {
        int r = image_data[i * 3 + 0];
        int g = image_data[i * 3 + 1];
        int b = image_data[i * 3 + 2];

        int r_bin = r / bin_size;
        int g_bin = g / bin_size;
        int b_bin = b / bin_size;

        int bin_index = r_bin * bins_per_channel * bins_per_channel + g_bin * bins_per_channel + b_bin;

        if (bin_index < histogram_size) {
            histogram[bin_index]++;
        }
    }

    stbi_image_free(image_data);


    float total_pixels = width * height;
    if (total_pixels > 0) {
        for (float& val : histogram) {
            val /= total_pixels;
        }
    }

    return histogram;
}