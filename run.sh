#!/usr/bin/env zsh
set -euo pipefail

# Script para configurar, compilar e executar o projeto CMake (macOS / zsh)
# Coloque este arquivo na raiz do projeto (onde está o CMakeLists.txt) e execute:
#   ./run.sh [args...]

PROJECT_ROOT="$(cd "$(dirname "$0")" && pwd)"
cd "$PROJECT_ROOT"

echo "Projeto: $PROJECT_ROOT"

mkdir -p build

echo "Configurando com CMake (Release)..."
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release

echo "Compilando..."
CPU=$(sysctl -n hw.ncpu 2>/dev/null || echo 1)
cmake --build build --parallel "$CPU" --config Release || cmake --build build --parallel "$CPU"

EXEC_NAME="analise_algoritmos"
echo "Procurando executável '$EXEC_NAME' em build/..."
EXE_PATH="$(find build -type f -name "$EXEC_NAME" -perm -111 -print -quit 2>/dev/null || true)"

if [[ -z "$EXE_PATH" ]]; then
  EXE_PATH="$(find build -type f -name "$EXEC_NAME" -print -quit 2>/dev/null || true)"
fi

if [[ -z "$EXE_PATH" ]]; then
  echo "Erro: executável '$EXEC_NAME' não encontrado em 'build/'. Verifique a saída do build." >&2
  exit 2
fi

echo "Executando: $EXE_PATH"
"$EXE_PATH" "$@"
