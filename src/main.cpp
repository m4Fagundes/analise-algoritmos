// src/main.cpp
/*
    Sistema de Busca de Imagens com Diferentes Estruturas de Dados
    Autores:    Pedro Rodrigues Alves, Luca Lourenço Araújo Dal Bianco Gonzaga, Matheus Fagundes Araujo, Rafael Vilela Padilha Clark, 
                Victor Lopes Azevedo Araujo, Guilherme Henrique Vieira Nascimento e Leonardo de Oliveira Carvalho.
    Data: 2024-06-20
    Descrição: Implementa um sistema para carregar imagens, extrair características,
                  e realizar buscas utilizando busca sequencial, tabela hash e quadtree.
 */
#include <iostream>
#include <string>
#include <vector>
#include <stdexcept>
#include <filesystem>
#include <algorithm>
#include <random>

#include "core/Image.h"
#include "core/Vector.h"
#include "core/Timer.h"
#include "structure/List.h"
#include "structure/HashTable.h"
#include "structure/QuadTree.h"
using namespace std;

int randImageGlobal = 0;

bool isImageFile(const string& filename) {
    string extension = filesystem::path(filename).extension().string();
    transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
    return extension == ".jpg" || extension == ".jpeg" || extension == ".png";
}

ImageList processImagesFromFolder(const string& folder_path) {
    ImageList imageList;

    cout << "\nProcessando imagens da pasta: " << folder_path << endl;
    cout << "------------------------------------" << endl;

    try {
        Timer timer;
        for (const auto& entry : filesystem::directory_iterator(folder_path)) {
            if (entry.is_regular_file() && isImageFile(entry.path().string())) {
                string img_path = entry.path().string();

                cout << "Processando: " << entry.path().filename().string() << endl;

                timer.start();
                FeatureVector features = extractFeatures(img_path);
                const double extraction_time = timer.elapsed_milliseconds();

                cout << "  -> Vetor de caracteristicas extraido com " << features.size() << " dimensoes." << endl;
                cout << "  -> Tempo de extracao: " << extraction_time << " ms" << endl;

                ImageData img_data(img_path, features, extraction_time);
                imageList.addImage(img_data);
            }
        }
    } catch (const filesystem::filesystem_error& e) {
        throw runtime_error("Erro ao acessar a pasta: " + string(e.what()));
    }

    if (imageList.empty()) {
        throw runtime_error("Nenhuma imagem válida encontrada na pasta especificada.");
    }

    return imageList;
}

// Função para carregar a imagem de referência
ImageData loadReferenceImage(const string& reference_folder) {
    for (const auto& entry : filesystem::directory_iterator(reference_folder)) {
        if (entry.is_regular_file() && isImageFile(entry.path().string())) {
            string img_path = entry.path().string();
            cout << "\nCarregando imagem de referência: " << entry.path().filename().string() << endl;
            Timer timer;
            timer.start();
            FeatureVector features = extractFeatures(img_path);
            const double extraction_time = timer.elapsed_milliseconds();
            cout << "  -> Vetor de características extraído com " << features.size() << " dimensões." << endl;
            cout << "  -> Tempo de extração: " << extraction_time << " ms" << endl;
            return ImageData(img_path, features, extraction_time);
        }
    }
    throw runtime_error("Nenhuma imagem válida encontrada na pasta de referência.");
}

// Altere as funções de teste para receber a imagem de referência
void testSimilarity(const ImageList& imageList, const ImageData& refImage) {
    if (imageList.size() < 2) {
        cout << "Necessario pelo menos 2 imagens para testar similaridade." << endl;
        return;
    }
    cout << "\n=== Teste de Similaridade ===" << endl;
    // Compara referência com a mais próxima da lista
    int nearestIndex = imageList.findNearest(refImage.features, -1);
    if (nearestIndex >= 0) {
        const ImageData& nearest = imageList.getImage(nearestIndex);
        cout << "Comparando:" << endl;
        cout << "  Imagem referência: " << filesystem::path(refImage.path).filename().string() << endl;
        cout << "  Imagem mais próxima: " << filesystem::path(nearest.path).filename().string() << endl;
        Timer timer;
        timer.start();
        const double distance = calculateEuclideanDistance(refImage.features, nearest.features);
        cout << "Distancia euclidiana: " << distance << endl;
        const double calculationTime = timer.elapsed_milliseconds();
        cout << "Tempo de calculo: " << calculationTime << " ms" << endl;
    }
}

void testSequentialSearch(const ImageList& imageList, const ImageData& refImage) {
    if (imageList.size() < 2) {
        cout << "Necessario pelo menos 2 imagens para testar busca." << endl;
        return;
    }
    cout << "\n=== Teste de Busca Sequencial ===" << endl;
    cout << "Imagem de consulta: " << filesystem::path(refImage.path).filename().string() << endl;
    Timer timer;
    timer.start();
    const int nearestIndex = imageList.findNearest(refImage.features, -1); 
    const double searchTime = timer.elapsed_milliseconds();
    if (nearestIndex >= 0) {
        const ImageData& result = imageList.getImage(nearestIndex);
        cout << "  -> Imagem mais proxima: " << filesystem::path(result.path).filename().string() << endl;
        cout << "  -> Tempo de busca: " << searchTime << " ms" << endl;
        cout << "  -> Comparacoes realizadas: " << imageList.size()  << endl;
        cout << "  -> Distancia: " << calculateEuclideanDistance(refImage.features, result.features) << endl;
    }
}

void testHashTableSearch(const HashTable& hashTable, const ImageData& refImage) {  
    if (hashTable.size() < 2) {
        cout << "Necessario pelo menos 2 imagens para testar busca com HashTable." << endl;
        return;
    }
    cout << "\n=== Teste de Busca com HashTable ===" << endl;
    cout << "Imagem de consulta: " << filesystem::path(refImage.path).filename().string() << endl;
    Timer timer;
    timer.start();
    const int nearestIndex = hashTable.findNearest(refImage.features, -1);
    const double searchTime = timer.elapsed_milliseconds();
    if (nearestIndex >= 0) {
        const ImageData& result = hashTable.getImage(nearestIndex);
        cout << "  -> Imagem mais proxima: " << filesystem::path(result.path).filename().string() << endl;
        cout << "  -> Tempo de busca: " << searchTime << " ms" << endl;
        cout << "  -> Comparacoes realizadas: " << hashTable.size() << endl;
        cout << "  -> Distancia: " << calculateEuclideanDistance(refImage.features, result.features) << endl;
    }
}

void testQuadTreeSearch(const QuadTree& quadTree, const ImageData& refImage) {
    if (quadTree.size() < 2) {
        cout << "Necessário pelo menos 2 imagens para testar busca com QuadTree." << endl;
        return;
    }
    cout << "\n=== Teste de Busca com QuadTree ===" << endl;
    cout << "Imagem de consulta: " << filesystem::path(refImage.path).filename().string() << endl;
    Timer timer;
    timer.start();
    int comparisons = 0;
    const int nearestIndex = quadTree.findNearest(refImage.features, -1, comparisons);
    const double searchTime = timer.elapsed_milliseconds();
    if (nearestIndex >= 0) {
        const ImageData& result = quadTree.getImage(nearestIndex);
        cout << "  -> Imagem mais proxima: " << filesystem::path(result.path).filename().string() << endl;
        cout << "  -> Tempo de busca: " << searchTime << " ms" << endl;
        cout << "  -> Comparacoes realizadas: " << comparisons << endl;
        cout << "  -> Distancia: " << calculateEuclideanDistance(refImage.features, result.features) << endl;
    }
}

int main() {
    const string images_folder = "../images";
    const string reference_folder = "../imageReference";

    try {
        random_device rd;
        mt19937 gen(rd());
        uniform_int_distribution<> distrib(1, 10000);
        BoundingBox globalRegion = {0, 255, 0, 255};
        QuadTree quadTree(globalRegion, 4, 10);

        cout << "=== Sistema de Busca de Imagens ===" << endl;

        const ImageList imageList = processImagesFromFolder(images_folder);
        cout << "\nTotal de imagens carregadas: " << imageList.size() << endl;

        // Carrega imagem de referência
        ImageData referenceImage = loadReferenceImage(reference_folder);

        // Construção da HashTable
        HashTable hashTable(101); 
        for (size_t i = 0; i < imageList.size(); i++) {
            hashTable.addImage(imageList.getImage(i));
        }

        // Construção da QuadTree
        for (size_t i = 0; i < imageList.size(); i++) {
            FeatureVector pos = { imageList.getImage(i).features[0], imageList.getImage(i).features[1] };
            quadTree.insert(imageList.getImage(i), pos, i);
        }

        cout << "\nTotal de imagens armazenadas na HashTable: " << hashTable.size() << endl;
        cout << "\nTotal de imagens armazenadas na QuadTree: " << quadTree.size() << endl;

        testSimilarity(imageList, referenceImage);
        testSequentialSearch(imageList, referenceImage);
        testHashTableSearch(hashTable, referenceImage);
        testQuadTreeSearch(quadTree, referenceImage);

    } catch (const exception& e) {
        cerr << "Erro: " << e.what() << endl;
        return 1;
    }

    return 0;
}
