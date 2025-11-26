// src/structure/MTree.cpp

#include "MTree.h"
#include <cmath>
#include <algorithm>
#include <iostream>

MTree::MTree(int capacity) 
    : root_(nullptr), capacity_(capacity), count_(0) {
    if (capacity_ < 2) {
        capacity_ = 2; // mínimo para permitir split
    }
}

void MTree::insert(const ImageData& image, int index) {
    // Armazena a imagem no dataStore
    if (static_cast<size_t>(index) >= dataStore_.size()) {
        dataStore_.resize(index + 1);
    }
    dataStore_[index] = image;
    
    MTreeEntry entry(image, index);
    
    if (!root_) {
        // Árvore vazia: cria nó raiz folha
        root_ = std::make_shared<MTreeNode>(true);
        root_->entries.push_back(entry);
        root_->coveringRadii.push_back(0.0);
        count_++;
        return;
    }
    
    // Insere recursivamente
    FeatureVector dummyParent;
    insertRecursive(root_, entry);
    count_++;
}

void MTree::insertRecursive(std::shared_ptr<MTreeNode>& node, const MTreeEntry& entry) {
    
    if (node->isLeaf) {
        // Nó folha: adiciona a entrada diretamente
        node->entries.push_back(entry);
        node->coveringRadii.push_back(0.0);
        
        // Verifica se precisa de split
        if (static_cast<int>(node->entries.size()) > capacity_) {
            if (node == root_) {
                splitRoot();
            }
        }
    } else {
        // Nó interno: escolhe a melhor subárvore
        int bestIdx = chooseBestSubtree(node, entry.features);
        
        if (bestIdx < 0 || bestIdx >= static_cast<int>(node->children.size())) {
            // Fallback: adiciona como nova entrada
            node->entries.push_back(entry);
            node->coveringRadii.push_back(0.0);
            auto newChild = std::make_shared<MTreeNode>(true);
            newChild->entries.push_back(entry);
            newChild->coveringRadii.push_back(0.0);
            node->children.push_back(newChild);
            return;
        }
        
        // Calcula distância ao routing object
        double distToRouting = calculateEuclideanDistance(
            entry.features, node->entries[bestIdx].features);
        
        // Insere recursivamente na subárvore escolhida
        insertRecursive(node->children[bestIdx], entry);
        
        // Atualiza o raio de cobertura se necessário
        if (distToRouting > node->coveringRadii[bestIdx]) {
            node->coveringRadii[bestIdx] = distToRouting;
        }
        
        // Verifica se o filho precisa de split
        if (static_cast<int>(node->children[bestIdx]->entries.size()) > capacity_) {
            splitChild(node, bestIdx);
        }
    }
}

void MTree::splitRoot() {
    if (!root_ || root_->entries.size() < 2) return;
    
    auto [p1, p2] = promote(root_->entries);
    
    std::vector<MTreeEntry> group1, group2;
    partition(root_->entries, p1, p2, group1, group2);
    
    auto newNode1 = std::make_shared<MTreeNode>(root_->isLeaf);
    auto newNode2 = std::make_shared<MTreeNode>(root_->isLeaf);
    
    newNode1->entries = std::move(group1);
    newNode2->entries = std::move(group2);
    
    newNode1->coveringRadii.resize(newNode1->entries.size(), 0.0);
    newNode2->coveringRadii.resize(newNode2->entries.size(), 0.0);
    
    // Cria nova raiz
    auto newRoot = std::make_shared<MTreeNode>(false);
    
    MTreeEntry rep1 = newNode1->entries[0];
    MTreeEntry rep2 = newNode2->entries[0];
    
    // Calcula raios de cobertura
    double radius1 = 0.0, radius2 = 0.0;
    for (const auto& e : newNode1->entries) {
        double d = calculateEuclideanDistance(e.features, rep1.features);
        radius1 = std::max(radius1, d);
    }
    for (const auto& e : newNode2->entries) {
        double d = calculateEuclideanDistance(e.features, rep2.features);
        radius2 = std::max(radius2, d);
    }
    
    newRoot->entries.push_back(rep1);
    newRoot->entries.push_back(rep2);
    newRoot->coveringRadii.push_back(radius1);
    newRoot->coveringRadii.push_back(radius2);
    newRoot->children.push_back(newNode1);
    newRoot->children.push_back(newNode2);
    
    root_ = newRoot;
}

void MTree::splitChild(std::shared_ptr<MTreeNode>& parent, int childIdx) {
    if (childIdx < 0 || childIdx >= static_cast<int>(parent->children.size())) return;
    
    auto child = parent->children[childIdx];
    if (child->entries.size() < 2) return;
    
    auto [p1, p2] = promote(child->entries);
    
    std::vector<MTreeEntry> group1, group2;
    partition(child->entries, p1, p2, group1, group2);
    
    auto newNode1 = std::make_shared<MTreeNode>(child->isLeaf);
    auto newNode2 = std::make_shared<MTreeNode>(child->isLeaf);
    
    newNode1->entries = std::move(group1);
    newNode2->entries = std::move(group2);
    
    newNode1->coveringRadii.resize(newNode1->entries.size(), 0.0);
    newNode2->coveringRadii.resize(newNode2->entries.size(), 0.0);
    
    MTreeEntry rep1 = newNode1->entries[0];
    MTreeEntry rep2 = newNode2->entries[0];
    
    // Calcula novos raios
    double radius1 = 0.0, radius2 = 0.0;
    for (const auto& e : newNode1->entries) {
        double d = calculateEuclideanDistance(e.features, rep1.features);
        radius1 = std::max(radius1, d);
    }
    for (const auto& e : newNode2->entries) {
        double d = calculateEuclideanDistance(e.features, rep2.features);
        radius2 = std::max(radius2, d);
    }
    
    // Atualiza o primeiro filho
    parent->entries[childIdx] = rep1;
    parent->coveringRadii[childIdx] = radius1;
    parent->children[childIdx] = newNode1;
    
    // Adiciona o segundo filho
    parent->entries.push_back(rep2);
    parent->coveringRadii.push_back(radius2);
    parent->children.push_back(newNode2);
    
    // Se o parent também estourou a capacidade e é a raiz
    if (static_cast<int>(parent->entries.size()) > capacity_ && parent == root_) {
        splitRoot();
    }
}

int MTree::chooseBestSubtree(const std::shared_ptr<MTreeNode>& node,
                              const FeatureVector& features) const {
    if (node->entries.empty()) return -1;
    
    int bestIdx = 0;
    double minDistance = std::numeric_limits<double>::max();
    double minEnlargement = std::numeric_limits<double>::max();
    bool foundInside = false;
    
    for (size_t i = 0; i < node->entries.size(); i++) {
        double dist = calculateEuclideanDistance(features, node->entries[i].features);
        double radius = (i < node->coveringRadii.size()) ? node->coveringRadii[i] : 0.0;
        
        if (dist <= radius) {
            // Objeto está dentro do raio de cobertura
            if (!foundInside || dist < minDistance) {
                minDistance = dist;
                bestIdx = static_cast<int>(i);
                foundInside = true;
            }
        } else if (!foundInside) {
            // Objeto está fora: calcula o aumento necessário do raio
            double enlargement = dist - radius;
            if (enlargement < minEnlargement) {
                minEnlargement = enlargement;
                bestIdx = static_cast<int>(i);
            }
        }
    }
    
    return bestIdx;
}

std::pair<int, int> MTree::promote(const std::vector<MTreeEntry>& entries) const {
    // Estratégia: escolhe os dois objetos mais distantes entre si
    int n = static_cast<int>(entries.size());
    int p1 = 0, p2 = (n > 1) ? 1 : 0;
    double maxDist = 0.0;
    
    for (int i = 0; i < n; i++) {
        for (int j = i + 1; j < n; j++) {
            double dist = calculateEuclideanDistance(entries[i].features, entries[j].features);
            if (dist > maxDist) {
                maxDist = dist;
                p1 = i;
                p2 = j;
            }
        }
    }
    
    return {p1, p2};
}

void MTree::partition(const std::vector<MTreeEntry>& entries,
                      int promote1, int promote2,
                      std::vector<MTreeEntry>& group1,
                      std::vector<MTreeEntry>& group2) const {
    
    if (entries.empty()) return;
    if (promote1 < 0 || promote1 >= static_cast<int>(entries.size())) promote1 = 0;
    if (promote2 < 0 || promote2 >= static_cast<int>(entries.size())) promote2 = (entries.size() > 1) ? 1 : 0;
    
    const FeatureVector& center1 = entries[promote1].features;
    const FeatureVector& center2 = entries[promote2].features;
    
    for (size_t i = 0; i < entries.size(); i++) {
        double dist1 = calculateEuclideanDistance(entries[i].features, center1);
        double dist2 = calculateEuclideanDistance(entries[i].features, center2);
        
        if (dist1 <= dist2) {
            MTreeEntry e = entries[i];
            e.distanceToParent = dist1;
            group1.push_back(e);
        } else {
            MTreeEntry e = entries[i];
            e.distanceToParent = dist2;
            group2.push_back(e);
        }
    }
    
    // Garante que ambos os grupos tenham pelo menos um elemento
    if (group1.empty() && !group2.empty()) {
        group1.push_back(group2.back());
        group2.pop_back();
    } else if (group2.empty() && !group1.empty()) {
        group2.push_back(group1.back());
        group1.pop_back();
    }
}

int MTree::findNearest(const FeatureVector& query, int ignoreIndex, int& comparisons) const {
    comparisons = 0;
    
    if (!root_ || root_->entries.empty()) {
        return -1;
    }
    
    double bestDist = std::numeric_limits<double>::max();
    int bestIndex = -1;
    
    searchNearest(root_, query, ignoreIndex, bestDist, bestIndex, comparisons);
    
    return bestIndex;
}

void MTree::searchNearest(const std::shared_ptr<MTreeNode>& node,
                          const FeatureVector& query,
                          int ignoreIndex,
                          double& bestDist,
                          int& bestIndex,
                          int& comparisons) const {
    
    if (!node) return;
    
    if (node->isLeaf) {
        // Nó folha: compara com todas as entradas
        for (const auto& entry : node->entries) {
            if (entry.index == ignoreIndex) continue;
            
            comparisons++;
            double dist = calculateEuclideanDistance(query, entry.features);
            
            if (dist < bestDist) {
                bestDist = dist;
                bestIndex = entry.index;
            }
        }
    } else {
        // Nó interno: usa poda baseada no raio de cobertura
        std::vector<std::pair<double, int>> candidates;
        
        for (size_t i = 0; i < node->entries.size(); i++) {
            double distToRouting = calculateEuclideanDistance(query, node->entries[i].features);
            double radius = (i < node->coveringRadii.size()) ? node->coveringRadii[i] : 0.0;
            
            // Condição de poda
            double minPossibleDist = std::max(0.0, distToRouting - radius);
            
            if (minPossibleDist < bestDist) {
                candidates.push_back({distToRouting, static_cast<int>(i)});
            }
        }
        
        // Ordena candidatos por distância
        std::sort(candidates.begin(), candidates.end());
        
        // Visita subárvores em ordem de proximidade
        for (const auto& [dist, idx] : candidates) {
            if (idx >= static_cast<int>(node->children.size())) continue;
            
            double radius = (idx < static_cast<int>(node->coveringRadii.size())) 
                           ? node->coveringRadii[idx] : 0.0;
            double minPossibleDist = std::max(0.0, dist - radius);
            
            if (minPossibleDist < bestDist) {
                searchNearest(node->children[idx], query, ignoreIndex, 
                             bestDist, bestIndex, comparisons);
            }
        }
    }
}

const ImageData& MTree::getImage(int index) const {
    return dataStore_[index];
}
