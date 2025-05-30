#include <iostream>
#include <fstream>
#include <bitset>
#include <vector>
#include <filesystem>
#include <string>
#include <chrono>

#ifndef FILE_SYSTEM_MANAGER
#define FILE_SYSTEM_MANAGER

namespace fs = std::filesystem;

// Lee un archivo y devuelve su contenido como vector de bytes
std::vector<char> readFile(const std::string& inputPath) {
    std::ifstream inputFile(inputPath, std::ios::binary | std::ios::ate);
    if (!inputFile) {
        throw std::runtime_error("No se pudo abrir el archivo: " + inputPath);
    }

    const size_t fileSize = inputFile.tellg();
    inputFile.seekg(0, std::ios::beg);

    std::vector<char> fileData(fileSize);
    if (!inputFile.read(fileData.data(), fileSize)) {
        throw std::runtime_error("Error al leer el archivo: " + inputPath);
    }

    return fileData;
}

// Escribe datos en un archivo
void writeFile(const std::string& outputPath, const std::vector<char>& data) {
    

    std::ofstream outputFile(outputPath, std::ios::binary);
    if (!outputFile) {
        throw std::runtime_error("No se pudo crear el archivo: " + outputPath);
    }
    if (!outputFile.write(data.data(), data.size())) {
        throw std::runtime_error("Error al escribir en el archivo: " + outputPath);
    }
}

// Obtiene la lista de archivos en un directorio
std::vector<std::string> getFilesInDirectory(const std::string& inputDir) {
    std::vector<std::string> files;
    
    if (!fs::exists(inputDir)) {
        throw std::runtime_error("El directorio no existe: " + inputDir);
    }

    for (const auto& entry : fs::directory_iterator(inputDir)) {
        try {
            if (entry.is_regular_file()) {
                files.push_back(entry.path().string());
                std::cout << "Procesando: " << entry.path().filename() << "\n";
            }
        } catch (const std::exception& e) {
            std::cerr << "Error procesando " << entry.path() << ": " << e.what() << std::endl;
        }
    }

    if (files.empty()) {
        std::cerr << "Advertencia: No se encontraron archivos en " << inputDir << std::endl;
    }

    return files;
}

#endif