// src/main.cpp
/*
    Sistema de Busca de Imagens com Diferentes Estruturas de Dados
    Autores: Pedro Rodrigues Alves,
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

#include "core/Image.h"
#include "core/Vector.h"
#include "core/Timer.h"
#include "structure/List.h"
#include "structure/HashTable.h"
#include "structure/QuadTree.h"
using namespace std;

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

void testSimilarity(const ImageList& imageList) {
    if (imageList.size() < 2) {
        cout << "Necessario pelo menos 2 imagens para testar similaridade." << endl;
        return;
    }

    cout << "\n=== Teste de Similaridade ===" << endl;

    // Usar as duas primeiras imagens
    const ImageData& image1 = imageList.getImage(0);
    const ImageData& image2 = imageList.getImage(1);

    cout << "Comparando:" << endl;
    cout << "  Imagem 1: " << filesystem::path(image1.path).filename().string() << endl;
    cout << "  Imagem 2: " << filesystem::path(image2.path).filename().string() << endl;

    Timer timer;
    timer.start();
    const double distance = calculateEuclideanDistance(image1.features, image2.features);
    cout << "Distancia euclidiana: " << distance << endl;
    const double calculationTime = timer.elapsed_milliseconds();
    cout << "Tempo de calculo: " << calculationTime << " ms" << endl;
}

void testSequentialSearch(const ImageList& imageList) {
    if (imageList.size() < 2) {
        cout << "Necessario pelo menos 2 imagens para testar busca." << endl;
        return;
    }

    cout << "\n=== Teste de Busca Sequencial ===" << endl;

    // Usar a última imagem como consulta
    const int queryIndex = imageList.size() - 1;
    const ImageData& queryImage = imageList.getImage(queryIndex);
    cout << "Imagem de consulta: " << filesystem::path(queryImage.path).filename().string() << endl;

    Timer timer;
    timer.start();
    const int nearestIndex = imageList.findNearest(queryImage.features, queryIndex); // Ignora a própria imagem
    const double searchTime = timer.elapsed_milliseconds();

    if (nearestIndex >= 0) {
        const ImageData& result = imageList.getImage(nearestIndex);
        cout << "  -> Imagem mais proxima: " << filesystem::path(result.path).filename().string() << endl;
        cout << "  -> Tempo de busca: " << searchTime << " ms" << endl;
        cout << "  -> Comparacoes realizadas: " << imageList.size()  << endl;
        cout << "  -> Distancia: " << calculateEuclideanDistance(queryImage.features, result.features) << endl;
    }
}

void testHashTableSearch(const HashTable& hashTable) {  
    if (hashTable.size() < 2) {
        cout << "Necessario pelo menos 2 imagens para testar busca com HashTable." << endl;
        return;
    }

    cout << "\n=== Teste de Busca com HashTable ===" << endl;

    // Usar a última imagem como consulta
    const int queryIndex = hashTable.size() - 1;
    const ImageData& queryImage = hashTable.getImage(queryIndex);

    cout << "Imagem de consulta: " 
         << filesystem::path(queryImage.path).filename().string() << endl;

    Timer timer;
    timer.start();
    const int nearestIndex = hashTable.findNearest(queryImage.features, queryIndex);
    const double searchTime = timer.elapsed_milliseconds();

    if (nearestIndex >= 0) {
        const ImageData& result = hashTable.getImage(nearestIndex);
        cout << "  -> Imagem mais proxima: " 
             << filesystem::path(result.path).filename().string() << endl;
        cout << "  -> Tempo de busca: " << searchTime << " ms" << endl;
        cout << "  -> Comparacoes realizadas: " << hashTable.size() << endl;
        cout << "  -> Distancia: " 
             << calculateEuclideanDistance(queryImage.features, result.features) << endl;
    }
}

void testQuadTreeSearch(const QuadTree& quadTree, const ImageList& imageList) {
    if (quadTree.size() < 2) {
        cout << "Necessário pelo menos 2 imagens para testar busca com QuadTree." << endl;
        return;
    }

    cout << "\n=== Teste de Busca com QuadTree ===" << endl;

    // Usar a última imagem como consulta
    const int queryIndex = imageList.size() - 1;
    const ImageData& queryImage = imageList.getImage(queryIndex);

    cout << "Imagem de consulta: "
         << filesystem::path(queryImage.path).filename().string() << endl;

    Timer timer;
    timer.start();

    // Para consulta, usamos uma posição no espaço RGB (ex.: R e G médios)
    FeatureVector pos = { queryImage.features[0], queryImage.features[1] };

    const int nearestIndex = quadTree.findNearest(queryImage.features, queryIndex);
    const double searchTime = timer.elapsed_milliseconds();

    if (nearestIndex >= 0) {
        const ImageData& result = quadTree.getImage(nearestIndex);
        cout << "  -> Imagem mais proxima: "
             << filesystem::path(result.path).filename().string() << endl;
        cout << "  -> Tempo de busca: " << searchTime << " ms" << endl;
        cout << "  -> Comparacoes realizadas: " << quadTree.size() << endl;
        cout << "  -> Distancia: "
             << calculateEuclideanDistance(queryImage.features, result.features) << endl;
    
        
    }
}

int main() {
    const string images_folder = "../images";

    try {
        BoundingBox globalRegion = {0, 255, 0, 255}; // Espaço RGB reduzido a R e G
        QuadTree quadTree(globalRegion, 4, 10);

        cout << "=== Sistema de Busca de Imagens ===" << endl;

        const ImageList imageList = processImagesFromFolder(images_folder);
        
        cout << "\nTotal de imagens carregadas: " << imageList.size() << endl;
        
        // Construção da HashTable
        HashTable hashTable(101); 
        for (size_t i = 0; i < imageList.size(); i++) {
            hashTable.addImage(imageList.getImage(i));
        }

        // Construção da QuadTree
        for (size_t i = 0; i < imageList.size(); i++) {
            FeatureVector pos = { imageList.getImage(i).features[0], imageList.getImage(i).features[1] };
            quadTree.insert(imageList.getImage(i), pos, i); // agora com índice global
        }


        cout << "\nTotal de imagens armazenadas na HashTable: " << hashTable.size() << endl;
        cout << "\nTotal de imagens armazenadas na QuadTree: " << quadTree.size() << endl;

        testSimilarity(imageList);
        testSequentialSearch(imageList);
        testHashTableSearch(hashTable);
        testQuadTreeSearch(quadTree, imageList);

    } catch (const exception& e) {
        cerr << "Erro: " << e.what() << endl;
        return 1;
    }

    return 0;
}
