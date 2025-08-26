# Versão mínima do CMake necessária para este projeto
cmake_minimum_required(VERSION 3.10)

# Define o nome do projeto, versão e a linguagem
project(
    AnaliseAlgoritmos
    VERSION 1.0
    LANGUAGES CXX
)

# Define o padrão da linguagem C++ a ser utilizado (C++17 é uma boa escolha)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Nome do executável que será gerado
set(EXECUTABLE_NAME analise_algoritmos)

# Adiciona o executável e lista todos os seus arquivos-fonte (.cpp)
# ATENÇÃO: Nomes atualizados de acordo com a sua imagem
add_executable(${EXECUTABLE_NAME}
    src/main.cpp
    src/core/Image.cpp
    src/core/Vector.cpp
    # src/structure/List.cpp
    # src/structure/KDTree.cpp
    # src/structure/HashTable.cpp
)

# Adiciona os diretórios que contêm arquivos de cabeçalho (.h)
# para que o compilador possa encontrá-los com #include "..."
# ATENÇÃO: Pasta "estruturas" atualizada para "structure"
target_include_directories(${EXECUTABLE_NAME} PUBLIC
    ${PROJECT_SOURCE_DIR}/src
    ${PROJECT_SOURCE_DIR}/extern
)

# Mensagem para o usuário após a configuração do CMake
message(STATUS "Configuração concluída! Para compilar, execute: cmake --build .")