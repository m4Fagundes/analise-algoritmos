// src/structure/MTree.h

#ifndef MTREE_H
#define MTREE_H

#include "../core/Image.h"
#include "../core/Vector.h"
#include "List.h"
#include <vector>
#include <memory>
#include <limits>
#include <algorithm>

/**
 * M-Tree: Árvore métrica para busca por similaridade em espaços métricos.
 * 
 * A M-Tree é uma estrutura de dados balanceada que organiza objetos com base
 * em uma função de distância métrica (ex: distância Euclidiana). Cada nó interno
 * contém um "routing object" com um raio de cobertura, permitindo poda eficiente
 * durante buscas por vizinho mais próximo.
 * 
 * Complexidade:
 * - Inserção: O(log n) em média
 * - Busca: O(log n) em média, O(n) no pior caso
 * 
 * Referência: Ciaccia, P., Patella, M., & Zezula, P. (1997). M-tree: An Efficient 
 * Access Method for Similarity Search in Metric Spaces.
 */

// Forward declaration
class MTreeNode;

// Entrada armazenada na M-Tree
struct MTreeEntry {
    ImageData image;
    FeatureVector features;
    int index;              // índice global da imagem
    double distanceToParent; // distância ao objeto pai (routing object)
    
    MTreeEntry() : index(-1), distanceToParent(0.0) {}
    MTreeEntry(const ImageData& img, int idx) 
        : image(img), features(img.features), index(idx), distanceToParent(0.0) {}
};

// Nó da M-Tree
class MTreeNode {
public:
    bool isLeaf;
    std::vector<MTreeEntry> entries;      // entradas (objetos) neste nó
    std::vector<double> coveringRadii;     // raios de cobertura (para nós internos)
    std::vector<std::shared_ptr<MTreeNode>> children; // filhos (para nós internos)
    
    MTreeNode(bool leaf = true) : isLeaf(leaf) {}
};

class MTree {
private:
    std::shared_ptr<MTreeNode> root_;
    std::vector<ImageData> dataStore_;  // armazena todas as imagens
    int capacity_;                       // capacidade máxima de cada nó
    size_t count_;                       // total de elementos
    
    // Funções auxiliares de inserção
    void insertRecursive(std::shared_ptr<MTreeNode>& node, const MTreeEntry& entry);
    
    // Split da raiz
    void splitRoot();
    
    // Split de um filho
    void splitChild(std::shared_ptr<MTreeNode>& parent, int childIdx);
    
    // Escolhe a melhor subárvore para inserção
    int chooseBestSubtree(const std::shared_ptr<MTreeNode>& node, 
                          const FeatureVector& features) const;
    
    // Busca recursiva do vizinho mais próximo
    void searchNearest(const std::shared_ptr<MTreeNode>& node,
                       const FeatureVector& query,
                       int ignoreIndex,
                       double& bestDist,
                       int& bestIndex,
                       int& comparisons) const;
    
    // Promoção de objetos para split (estratégia: mínima soma de raios)
    std::pair<int, int> promote(const std::vector<MTreeEntry>& entries) const;
    
    // Partição de entradas após promoção
    void partition(const std::vector<MTreeEntry>& entries,
                   int promote1, int promote2,
                   std::vector<MTreeEntry>& group1,
                   std::vector<MTreeEntry>& group2) const;

public:
    /**
     * Construtor da M-Tree
     * @param capacity Capacidade máxima de entradas por nó (recomendado: 4-50)
     */
    explicit MTree(int capacity = 10);
    
    /**
     * Insere uma imagem na M-Tree
     * @param image Dados da imagem a ser inserida
     * @param index Índice global da imagem
     */
    void insert(const ImageData& image, int index);
    
    /**
     * Busca o vizinho mais próximo
     * @param query Vetor de características da consulta
     * @param ignoreIndex Índice a ser ignorado na busca (-1 para nenhum)
     * @param comparisons Contador de comparações realizadas (saída)
     * @return Índice da imagem mais próxima, ou -1 se não encontrada
     */
    int findNearest(const FeatureVector& query, int ignoreIndex, int& comparisons) const;
    
    /**
     * Retorna a imagem pelo índice
     */
    const ImageData& getImage(int index) const;
    
    /**
     * Retorna o número de elementos na árvore
     */
    size_t size() const { return count_; }
    
    /**
     * Verifica se a árvore está vazia
     */
    bool empty() const { return count_ == 0; }
};

#endif // MTREE_H
