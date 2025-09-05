#include "QuadTree.h"

QuadTree::QuadTree(const BoundingBox& region, int capacity, int depth, int maxDepth)
    : region(region), capacity(capacity), depth(depth), maxDepth(maxDepth) {}

// Divide o nó atual em 8 sub-regiões
void QuadTree::subdivide() {
    FeatureVector mid = region.center();

    // 8 combinações (menor/maior em R, G, B)
    children[0] = std::make_unique<QuadTree>(
        BoundingBox{region.rMin, mid.r, region.gMin, mid.g, region.bMin, mid.b},
        capacity, depth+1, maxDepth);

    children[1] = std::make_unique<QuadTree>(
        BoundingBox{mid.r, region.rMax, region.gMin, mid.g, region.bMin, mid.b},
        capacity, depth+1, maxDepth);

    children[2] = std::make_unique<QuadTree>(
        BoundingBox{region.rMin, mid.r, mid.g, region.gMax, region.bMin, mid.b},
        capacity, depth+1, maxDepth);

    children[3] = std::make_unique<QuadTree>(
        BoundingBox{mid.r, region.rMax, mid.g, region.gMax, region.bMin, mid.b},
        capacity, depth+1, maxDepth);

    children[4] = std::make_unique<QuadTree>(
        BoundingBox{region.rMin, mid.r, region.gMin, mid.g, mid.b, region.bMax},
        capacity, depth+1, maxDepth);

    children[5] = std::make_unique<QuadTree>(
        BoundingBox{mid.r, region.rMax, region.gMin, mid.g, mid.b, region.bMax},
        capacity, depth+1, maxDepth);

    children[6] = std::make_unique<QuadTree>(
        BoundingBox{region.rMin, mid.r, mid.g, region.gMax, mid.b, region.bMax},
        capacity, depth+1, maxDepth);

    children[7] = std::make_unique<QuadTree>(
        BoundingBox{mid.r, region.rMax, mid.g, region.gMax, mid.b, region.bMax},
        capacity, depth+1, maxDepth);
}

// Retorna índice do filho correto para inserir o ponto
int QuadTree::getChildIndex(const FeatureVector& v, const FeatureVector& mid) const {
    int index = 0;
    if (v.r >= mid.r) index |= 1; // lado direito
    if (v.g >= mid.g) index |= 2; // lado superior
    if (v.b >= mid.b) index |= 4; // lado "frente"
    return index;
}

// Inserção de ponto RGB
bool QuadTree::insert(const FeatureVector& v) {
    if (!region.contains(v)) return false;

    // Se ainda há espaço neste nó
    if (points.size() < capacity || depth == maxDepth) {
        points.push_back(v);
        return true;
    }

    // Se ainda não subdividiu, cria filhos
    if (!children[0]) {
        subdivide();
    }

    // Escolhe filho apropriado
    FeatureVector mid = region.center();
    int index = getChildIndex(v, mid);
    return children[index]->insert(v);
}

// Coleta todos os pontos (para debug ou busca linear)
void QuadTree::collect(std::vector<FeatureVector>& out) const {
    out.insert(out.end(), points.begin(), points.end());
    for (int i = 0; i < 8; i++) {
        if (children[i]) {
            children[i]->collect(out);
        }
    }
}
