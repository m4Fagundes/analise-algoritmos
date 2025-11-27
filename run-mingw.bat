@echo off
setlocal

:: diretório do script (remove trailing backslash para evitar problemas com aspas)
set "ROOT_DIR=%~dp0"
if "%ROOT_DIR:~-1%"=="\" set "ROOT_DIR=%ROOT_DIR:~0,-1%"

cd /d "%ROOT_DIR%" || (echo Erro: não foi possível mudar para %ROOT_DIR% & exit /b 1)

:: checar ferramentas
where cmake >nul 2>&1 || (echo cmake não encontrado. Abra MSYS2 MinGW shell ou adicione ao PATH & exit /b 1)
where mingw32-make >nul 2>&1 || (echo mingw32-make não encontrado. Verifique MinGW/MSYS2 & exit /b 1)

set "BUILD_DIR=%ROOT_DIR%\build"
if not exist "%BUILD_DIR%" mkdir "%BUILD_DIR%"

echo Configurando CMake (MinGW Makefiles)...
cmake -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release -S "%ROOT_DIR%" -B "%BUILD_DIR%" || (echo CMake falhou & exit /b 1)

set "JOBS=%NUMBER_OF_PROCESSORS%"
if "%JOBS%"=="" set "JOBS=2"

echo Compilando...
mingw32-make -C "%BUILD_DIR%" -j%JOBS% || (echo Compilação falhou & exit /b 1)

set "EXE=%BUILD_DIR%\analise_algoritmos.exe"
if not exist "%EXE%" (
  echo Executável não encontrado: %EXE%
  exit /b 1
)

echo Executando: "%EXE%"
"%EXE%"

endlocal
exit /b 0