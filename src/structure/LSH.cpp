// src/structure/LSH.cpp
#include "LSH.h"
#include <cmath>
#include <limits>
#include <iostream>

LSH::LSH(int dimension, int num_tables, int num_bits) 
    : dimension_(dimension), num_tables_(num_tables), num_bits_(num_bits) {
    
    tables_.resize(num_tables);
    planes_.resize(num_tables);

    std::mt19937 gen(42); // Seed fixa para reprodutibilidade no relatório
    std::normal_distribution<float> d(0.0, 1.0);

    // Inicializa os hiperplanos com vetores normais aleatórios
    for (int i = 0; i < num_tables; ++i) {
        planes_[i].resize(num_bits);
        for (int j = 0; j < num_bits; ++j) {
            planes_[i][j].resize(dimension);
            for (int k = 0; k < dimension; ++k) {
                planes_[i][j][k] = d(gen);
            }
        }
    }
}

size_t LSH::computeHash(const FeatureVector& feature, int tableIdx) const {
    size_t hash = 0;
    // Verifica compatibilidade de dimensão
    size_t dim = std::min((size_t)dimension_, feature.size());

    for (int i = 0; i < num_bits_; ++i) {
        float dot_product = 0.0f;
        for (size_t d = 0; d < dim; ++d) {
            dot_product += feature[d] * planes_[tableIdx][i][d];
        }
        
        // Se produto escalar > 0, bit é 1. Senão, 0.
        if (dot_product >= 0) {
            hash |= (1ULL << i);
        }
    }
    return hash;
}

void LSH::addImage(const ImageData& img) {
    int idx = data_store_.size();
    data_store_.push_back(img);

    for (int i = 0; i < num_tables_; ++i) {
        size_t hash = computeHash(img.features, i);
        tables_[i][hash].push_back(idx);
    }
}

const ImageData& LSH::getImage(int index) const {
    return data_store_[index];
}

size_t LSH::size() const {
    return data_store_.size();
}

int LSH::findNearest(const FeatureVector& query, int k_ignored, int* comparisons_out) const {
    int comparisons = 0;
    double min_dist = std::numeric_limits<double>::max();
    int nearest_idx = -1;

    // Usamos um set para não comparar a mesma imagem duas vezes (se ela cair em múltiplos buckets)
    std::unordered_set<int> candidates_checked;

    for (int i = 0; i < num_tables_; ++i) {
        size_t hash = computeHash(query, i);
        
        // Verifica se existe bucket para esse hash
        auto it = tables_[i].find(hash);
        if (it != tables_[i].end()) {
            const std::vector<int>& bucket_candidates = it->second;
            
            for (int idx : bucket_candidates) {
                // Evita reprocessar o mesmo candidato
                if (candidates_checked.count(idx)) continue;
                candidates_checked.insert(idx);
                
                comparisons++;
                
                double dist = calculateEuclideanDistance(query, data_store_[idx].features);
                if (dist < min_dist) {
                    min_dist = dist;
                    nearest_idx = idx;
                }
            }
        }
    }

    if (comparisons_out) *comparisons_out = comparisons;
    return nearest_idx;
}