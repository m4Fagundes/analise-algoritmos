// src/core/Image.h

#ifndef IMAGE_H
#define IMAGE_H

#include "Vector.h" // Precisa saber o que é FeatureVector
#include <string>

/**
 * @brief Extrai um vetor de características (histograma de cores) de uma imagem.
 */
FeatureVector extractFeatures(const std::string& image_path, int bins_per_channel = 4);

#endif // IMAGE_H