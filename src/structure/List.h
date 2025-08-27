#ifndef LIST_H
#define LIST_H

#include <vector>
#include <string>
#include "../core/Vector.h"

struct ImageData {
    std::string path;
    FeatureVector features;
    double extraction_time;

    ImageData() = default;
    ImageData(const std::string& p, const FeatureVector& f, double t)
        : path(p), features(f), extraction_time(t) {}
};

class ImageList {
    std::vector<ImageData> images;

public:
    void addImage(const ImageData& image);

    int findNearest(const FeatureVector& query, int ignoreIndex) const;

    const ImageData& getImage(int index) const;

    size_t size() const { return images.size(); }
    bool empty() const { return images.empty(); }
};

#endif // LIST_H
