// src/structure/LSH.h
#ifndef LSH_H
#define LSH_H

#include "../core/Image.h"
#include "../core/Vector.h"
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <random>
#include "List.h"

// Definição compatível com seu ImageData usado na main
// Caso ImageData já esteja em Image.h, pode remover esta struct se der conflito
#ifndef IMAGE_DATA_STRUCT
#define IMAGE_DATA_STRUCT
// Nota: Assumindo que ImageData tem este formato baseado no seu main.cpp
// Se já estiver definido em outro lugar, apenas certifique-se que LSH.h o enxergue.
#endif

class LSH {
private:
    // Armazena cópias dos dados para garantir que o índice retornado seja válido internamente
    std::vector<ImageData> data_store_;
    
    // Hiperplanos aleatórios: [num_tables][num_bits][dimension]
    std::vector<std::vector<std::vector<float>>> planes_;
    
    // Tabelas Hash: [table_index] -> (Hash -> Lista de Índices na data_store_)
    // Usamos string ou size_t como chave do hash
    std::vector<std::unordered_map<size_t, std::vector<int>>> tables_;

    int num_tables_; // L
    int num_bits_;   // K
    int dimension_;  // D

    // Gera um hash para um vetor usando a tabela específica
    size_t computeHash(const FeatureVector& feature, int tableIdx) const;

public:
    /**
     * @param dimension Dimensão do vetor de características (ex: 64)
     * @param num_tables Número de tabelas hash (L). Aumenta chance de encontrar (Recall). Recomendado: 5 a 10.
     * @param num_bits Número de bits do hash (K). Aumenta seletividade (Precisão). Recomendado: log2(N).
     */
    LSH(int dimension, int num_tables = 5, int num_bits = 10);

    void addImage(const ImageData& img);

    // Retorna o índice da imagem mais próxima dentro da estrutura LSH
    // Retorna -1 se nada for encontrado
    int findNearest(const FeatureVector& query, int k_ignored = -1, int* comparisons_out = nullptr) const;

    const ImageData& getImage(int index) const;
    
    size_t size() const;
};

#endif // LSH_H