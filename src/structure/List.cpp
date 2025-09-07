// src/structure/list.cpp

#include "List.h"
#include "../core/Vector.h"
#include <limits>

void ImageList::addImage(const ImageData& image) {
    images.push_back(image);
}

int ImageList::findNearest(const FeatureVector& query, const int ignoreIndex) const {
    if (images.empty()) return -1;

    int nearestIndex = -1;
    double minDistance = std::numeric_limits<double>::max();

    for (int i = 0; i < images.size(); ++i) {
        if (i == ignoreIndex) continue; // Pula a imagem de consulta

        double distance = calculateEuclideanDistance(query, images[i].features);
        if (nearestIndex == -1 || distance < minDistance) {
            minDistance = distance;
            nearestIndex = i;
        }
    }

    return nearestIndex;
}

const ImageData& ImageList::getImage(const int index) const {
    return images[index];
}
