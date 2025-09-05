#pragma once

#include <string>
#include <vector>
#include "../core/Vector.h"
#include "List.h" // Para acessar a struct ImageData, apenas para isso

class HashTable {
public:
    explicit HashTable(size_t capacity = 101);

    void addImage(const ImageData& image);
    int findNearest(const FeatureVector& query, int ignoreIndex = -1) const;
    const ImageData& getImage(int index) const;

    size_t size() const { return count; }
    bool empty() const { return count == 0; }

private:
    std::vector<std::vector<ImageData>> buckets;
    size_t count;

    size_t hashFunction(const std::string& key) const;
};