// src/main.cpp

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

                cout << "  -> Vetor de características extraído com " << features.size() << " dimensões." << endl;
                cout << "  -> Tempo de extração: " << extraction_time << " ms" << endl;

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
        cout << "Necessário pelo menos 2 imagens para testar similaridade." << endl;
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
    const double calculationTime = timer.elapsed_milliseconds();

    cout << "Distância euclidiana: " << distance << endl;
    cout << "Tempo de cálculo: " << calculationTime << " ms" << endl;
}

void testSequentialSearch(const ImageList& imageList) {
    if (imageList.size() < 2) {
        cout << "Necessário pelo menos 2 imagens para testar busca." << endl;
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
        cout << "  -> Imagem mais próxima: " << filesystem::path(result.path).filename().string() << endl;
        cout << "  -> Distância: " << calculateEuclideanDistance(queryImage.features, result.features) << endl;
        cout << "  -> Tempo de busca: " << searchTime << " ms" << endl;
        cout << "  -> Comparações realizadas: " << imageList.size()  << endl;
    }
}

void testHashTableSearch(const HashTable& hashTable) {
    if (hashTable.size() < 2) {
        cout << "Necessário pelo menos 2 imagens para testar busca com HashTable." << endl;
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
        cout << "  -> Imagem mais próxima: " 
             << filesystem::path(result.path).filename().string() << endl;
        cout << "  -> Distância: " 
             << calculateEuclideanDistance(queryImage.features, result.features) << endl;
        cout << "  -> Tempo de busca: " << searchTime << " ms" << endl;
        cout << "  -> Comparações realizadas: " << hashTable.size() << endl;
    }
}

int main() {
    const string images_folder = "../images";

    try {
        cout << "=== Sistema de Busca de Imagens ===" << endl;

        const ImageList imageList = processImagesFromFolder(images_folder);
        
        cout << "\nTotal de imagens carregadas: " << imageList.size() << endl;
        
        HashTable hashTable(101); 
        for (size_t i = 0; i < imageList.size(); i++) {
            hashTable.addImage(imageList.getImage(i));
        }

        cout << "\nTotal de imagens armazenadas na HashTable: " << hashTable.size() << endl;

        testSimilarity(imageList);
        testSequentialSearch(imageList);
        testHashTableSearch(hashTable);

    } catch (const exception& e) {
        cerr << "Erro: " << e.what() << endl;
        return 1;
    }

    return 0;
}
