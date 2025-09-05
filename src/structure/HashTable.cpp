// src/structure/HashTable.cpp
#include "HashTable.h"
#include <functional>
#include <limits>
#include <filesystem>

HashTable::HashTable(size_t capacity)
    : buckets(std::max<size_t>(1, capacity)), count(0) {}

size_t HashTable::hashFunction(const std::string& key) const {
    return std::hash<std::string>{}(key) % buckets.size();
}

void HashTable::addImage(const ImageData& image) {
    std::string key = std::filesystem::path(image.path).filename().string();
    size_t idx = hashFunction(key);
    buckets[idx].push_back(image);
    ++count;
}

int HashTable::findNearest(const FeatureVector& query, int ignoreIndex) const {
    if (count == 0) return -1;

    int nearestIndex = -1;
    double minDistance = std::numeric_limits<double>::max();
    int globalIndex = 0;

    // percorre todos os buckets
    for (const auto& bucket : buckets) {
        for (const auto& img : bucket) {
            if (globalIndex == ignoreIndex) {
                ++globalIndex;
                continue;
            }

            double distance = calculateEuclideanDistance(query, img.features);
            if (nearestIndex == -1 || distance < minDistance) {
                minDistance = distance;
                nearestIndex = globalIndex;
            }
            ++globalIndex;
        }
    }

    return nearestIndex;
}

const ImageData& HashTable::getImage(int index) const {
    int globalIndex = 0;
    for (const auto& bucket : buckets) {
        for (const auto& img : bucket) {
            if (globalIndex == index) {
                return img;
            }
            ++globalIndex;
        }
    }
    throw std::out_of_range("Índice inválido em HashTable::getImage");
}
