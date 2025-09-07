// src/core/Vector.cpp

#include "Vector.h"
#include <cmath>
#include <stdexcept>

double calculateEuclideanDistance(const FeatureVector& v1, const FeatureVector& v2) {
    if (v1.size() != v2.size()) {
        throw std::invalid_argument("Vetores devem ter o mesmo tamanho para calcular a distancia.");
    }

    double sum_of_squares = 0.0;
    for (size_t i = 0; i < v1.size(); ++i) {
        sum_of_squares += (v1[i] - v2[i]) * (v1[i] - v2[i]);
    }

    return std::sqrt(sum_of_squares);
}