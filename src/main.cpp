// src/main.cpp

#include <iostream>
#include <string>
#include <vector>
#include <stdexcept>

#include "core/Image.h"
#include "core/Vector.h"
#include "core/Timer.h"

int main() {
    std::string image_path1 = "../images/cartoon-network.jpg";
    std::string image_path2 = "../images/cartoon-network.jpg";

    try {
        std::cout << "Iniciando teste do Modulo Core..." << std::endl;
        std::cout << "------------------------------------" << std::endl

        Timer timer;
        FeatureVector vec1, vec2;

        std::cout << "Processando a imagem 1: " << image_path1 << std::endl;
        timer.start();
        vec1 = extractFeatures(image_path1);
        double time1 = timer.elapsed_milliseconds();
        
        std::cout << "  -> Vetor de caracteristicas extraido com " << vec1.size() << " dimensoes." << std::endl;
        std::cout << "  -> Tempo de extracao: " << time1 << " ms" << std::endl;
        std::cout << std::endl;

        

        std::cout << "Processando a imagem 2: " << image_path2 << std::endl;
        timer.start();
        vec2 = extractFeatures(image_path2);
        double time2 = timer.elapsed_milliseconds();

        std::cout << "  -> Vetor de caracteristicas extraido com " << vec2.size() << " dimensoes." << std::endl;
        std::cout << "  -> Tempo de extracao: " << time2 << " ms" << std::endl;
        std::cout << std::endl;

        std::cout << "Calculando a similaridade (distancia Euclidiana) entre as imagens..." << std::endl;
        timer.start();
        double distance = calculateEuclideanDistance(vec1, vec2);
        double time_dist = timer.elapsed_milliseconds();

        std::cout << "  -> Distancia: " << distance << std::endl;
        std::cout << "  -> Tempo de calculo: " << time_dist << " ms" << std::endl;
        std::cout << "------------------------------------" << std::endl;
        std::cout << "Teste concluido com sucesso!" << std::endl;


    } catch (const std::exception& e) {
        std::cerr << "ERRO: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}