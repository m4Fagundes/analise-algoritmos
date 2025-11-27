    // src/structure/QuadTree.h

    #pragma once

    #include <vector>
    #include <string>
    #include "../core/Vector.h"
    #include "List.h"

    struct BoundingBox {
        float x_min, x_max;
        float y_min, y_max;
    };

    struct QuadTreeEntry {
        ImageData image;
        int index; // índice global da imagem
    };

    class QuadTree {
    public:
        QuadTree(const BoundingBox& region, int capacity = 4, int maxDepth = 10);

        void insert(const ImageData& image, const FeatureVector& position, int index);
        int findNearest(const FeatureVector& query, int ignoreIndex = -1, int& comparisons = *(new int(0))) const;
        const ImageData& getImage(int index) const;

        size_t size() const { return count; }
        bool empty() const { return count == 0; }

    private:
        BoundingBox region;
        int capacity;
        int maxDepth;
        size_t count;

        std::vector<QuadTreeEntry> entries;
        bool divided;
        QuadTree* children[4];

        bool contains(const FeatureVector& pos) const;
        void subdivide();
    };
