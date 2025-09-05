#ifndef QUADTREE_H
#define QUADTREE_H

#include <vector>
#include <memory>

// Vetor de características RGB
struct FeatureVector {
    double r, g, b;
};

// Caixa delimitadora (região do espaço RGB)
struct BoundingBox {
    double rMin, rMax;
    double gMin, gMax;
    double bMin, bMax;

    // Verifica se um ponto está dentro desta região
    bool contains(const FeatureVector& v) const {
        return (v.r >= rMin && v.r <= rMax &&
                v.g >= gMin && v.g <= gMax &&
                v.b >= bMin && v.b <= bMax);
    }

    // Calcula o ponto médio do cubo
    FeatureVector center() const {
        return {
            (rMin + rMax) / 2.0,
            (gMin + gMax) / 2.0,
            (bMin + bMax) / 2.0
        };
    }
};

// Nó da Quadtree (na prática Octree em 3D)
class QuadTree {
private:
    BoundingBox region;                  // região espacial deste nó
    std::vector<FeatureVector> points;   // pontos armazenados no nó
    std::unique_ptr<QuadTree> children[8]; // até 8 subdivisões
    int capacity;                        // capacidade máxima de pontos por nó
    int depth;                           // profundidade do nó
    int maxDepth;                        // profundidade máxima permitida

    // Cria os filhos (subdivide o espaço)
    void subdivide();

    // Determina qual filho deve receber o ponto
    int getChildIndex(const FeatureVector& v, const FeatureVector& mid) const;

public:
    QuadTree(const BoundingBox& region, int capacity=5, int depth=0, int maxDepth=10);

    // Insere um vetor RGB
    bool insert(const FeatureVector& v);

    // Retorna todos os pontos armazenados neste nó (e filhos)
    void collect(std::vector<FeatureVector>& out) const;
};

#endif