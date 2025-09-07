// src/core/Vector.h

#ifndef VECTOR_H
#define VECTOR_H

#include <vector>

using FeatureVector = std::vector<float>;

/**
 * @brief Calcula a distância Euclidiana entre dois vetores de características.
 */
double calculateEuclideanDistance(const FeatureVector& v1, const FeatureVector& v2);

#endif // VECTOR_H