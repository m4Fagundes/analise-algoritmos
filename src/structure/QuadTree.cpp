// src/structure/QuadTree.cpp

#include "QuadTree.h"
#include <limits>
#include <stdexcept>

QuadTree::QuadTree(const BoundingBox& region, int capacity, int maxDepth)
    : region(region), capacity(capacity), maxDepth(maxDepth),
      count(0), divided(false) {
    for (int i = 0; i < 4; i++) children[i] = nullptr;
}

bool QuadTree::contains(const FeatureVector& pos) const {
    return pos[0] >= region.x_min && pos[0] <= region.x_max &&
           pos[1] >= region.y_min && pos[1] <= region.y_max;
}

void QuadTree::subdivide() {
    float midX = (region.x_min + region.x_max) / 2.0f;
    float midY = (region.y_min + region.y_max) / 2.0f;

    BoundingBox nw = {region.x_min, midX, midY, region.y_max};
    BoundingBox ne = {midX, region.x_max, midY, region.y_max};
    BoundingBox sw = {region.x_min, midX, region.y_min, midY};
    BoundingBox se = {midX, region.x_max, region.y_min, midY};

    children[0] = new QuadTree(nw, capacity, maxDepth - 1);
    children[1] = new QuadTree(ne, capacity, maxDepth - 1);
    children[2] = new QuadTree(sw, capacity, maxDepth - 1);
    children[3] = new QuadTree(se, capacity, maxDepth - 1);

    divided = true;
}

void QuadTree::insert(const ImageData& image, const FeatureVector& position, int index) {
    if (!contains(position)) return;

    if (entries.size() < capacity || maxDepth == 0) {
        entries.push_back({image, index});
        count++;
        return;
    }

    if (!divided) subdivide();

    for (int i = 0; i < 4; i++) {
        children[i]->insert(image, position, index);
    }
}

int QuadTree::findNearest(const FeatureVector& query, int ignoreIndex, int& comparisons) const {
    double minDistance = std::numeric_limits<double>::max();
    int nearestIndex = -1;

    // verifica entradas locais
    for (const auto& entry : entries) {
        if (entry.index == ignoreIndex) continue;

        double distance = calculateEuclideanDistance(query, entry.image.features);

        if (distance < minDistance) {
            minDistance = distance;
            nearestIndex = entry.index;
        }
    }

    // verifica recursivamente nos filhos
    if (divided) {
        for (int i = 0; i < 4; i++) {
            int candidate = children[i]->findNearest(query, ignoreIndex, comparisons);
            if (candidate != -1) {
                double distance = calculateEuclideanDistance(query, children[i]->getImage(candidate).features);
                comparisons++;
                if (distance < minDistance) {
                    minDistance = distance;
                    nearestIndex = candidate;
                }
            }
        }
    }

    return nearestIndex;
}


const ImageData& QuadTree::getImage(int index) const  {
    for (const auto& entry : entries) {
        if (entry.index == index) {
            return entry.image;
        }
    }

    if (divided) {
        for (int i = 0; i < 4; i++) {
            try {
                return children[i]->getImage(index);
            } catch (...) {
                // continua buscando nos outros filhos
            }
        }
    }

    throw std::out_of_range("Índice inválido em QuadTree::getImage");
}
