#include <iostream>
#include <fstream>
#include <bitset>
#include <vector>
#include <filesystem>
#include <string>
#include <iomanip>
#include "../../shared/global.cpp"
#include "../../services/performance_analyzer.cpp"
#include "../../services/file_system_manager.cpp"

#ifndef PERFORMANCE_TEST
#define PERFORMANCE_TEST 

using CipherByteProcessor = std::function<std::vector<char>(std::vector<char>)>; 
using CipherWordProcessor = std::function<std::vector<uint32_t>(std::vector<uint32_t>)>; 

// Formatea bytes a una unidad legible (KB, MB, GB)
std::string formatBytes(size_t bytes) {
    constexpr double KB = 1024.0;
    constexpr double MB = KB * 1024.0;
    constexpr double GB = MB * 1024.0;

    if (bytes >= GB) return std::to_string(bytes/GB) + " GB";
    if (bytes >= MB) return std::to_string(bytes/MB) + " MB";
    if (bytes >= KB) return std::to_string(bytes/KB) + " KB";
    return std::to_string(bytes) + " bytes";
}

// Calcula el throughput en MB/s
double calculateThroughput(size_t bytes, double time_ms) {
    if (time_ms <= 0) return 0.0;
    double time_sec = time_ms / 1000.0;
    double mb = bytes / (1024.0 * 1024.0);
    return mb / time_sec;
}

// Función común para mostrar los resultados
void displayPerformanceResults(const std::string& fileName, size_t fileSize, 
                             size_t processedSize, long read_ms, 
                             long processing_ms, long write_ms) {
    std::cout << "\n[Resultados para: " << fileName << "]\n";
    std::cout << "  - Tamaño del archivo: " << formatBytes(fileSize) 
              << " (" << fileSize << " bytes)\n";
    std::cout << "  - Tiempo de lectura: " << read_ms << " ms\n";
    std::cout << "  - Tiempo de procesamiento: " << processing_ms << " ms\n";
    std::cout << "  - Throughput procesamiento: " << std::fixed << std::setprecision(2) 
              << calculateThroughput(fileSize, processing_ms) << " MB/s\n";
    std::cout << "  - Tiempo de escritura: " << write_ms << " ms\n";
    std::cout << "  - Throughput total: " << std::fixed << std::setprecision(2) 
              << calculateThroughput(fileSize, read_ms + processing_ms + write_ms) << " MB/s\n";
}

// Ejecuta la prueba de rendimiento con procesador de words
void runPerformanceTestWithWordProcessor(CipherWordProcessor processor, const std::string& inputDir, const std::string& outputDir) {
    const auto filesPaths = getFilesInDirectory(inputDir);
    
    if (!fs::exists(outputDir)) {
        std::cout << "[INFO] Creando directorio de salida: " << outputDir << std::endl;
        fs::create_directories(outputDir);
    }

    std::cout << "\n=== INICIANDO PRUEBAS DE RENDIMIENTO (WORD PROCESSOR) ===\n";
    std::cout << "Archivos a procesar: " << filesPaths.size() << std::endl;
    
    for (size_t i = 0; i < filesPaths.size(); ++i) {
        const auto& filePath = filesPaths[i];
        try {
            std::cout << "\n[Procesando archivo " << (i+1) << "/" << filesPaths.size() << "]: " 
                      << fs::path(filePath).filename().string() << std::endl;
            
            auto total_start = std::chrono::high_resolution_clock::now();
            const auto fileName = fs::path(filePath).filename().string();            
            const auto outputPath = (fs::path(outputDir) / fileName).string();
            
            // Lectura del archivo
            std::cout << "  - Leyendo archivo..." << std::endl;
            auto read_start = std::chrono::high_resolution_clock::now();
            const auto fileData = readFile(filePath);
            auto read_end = std::chrono::high_resolution_clock::now();
            auto read_ms = std::chrono::duration_cast<std::chrono::milliseconds>(read_end-read_start).count();
            
            size_t fileSize = fileData.size();
            std::cout << "  - Lectura completada. Tamaño: " << formatBytes(fileSize) 
                      << ", tiempo: " << read_ms << " ms" << std::endl;
            
            // Convertir bytes a words (asumiendo 4 bytes por word)
            std::cout << "  - Convirtiendo datos a words..." << std::endl;
            std::vector<uint32_t> words;
            words.resize(fileData.size() / sizeof(uint32_t));
            memcpy(words.data(), fileData.data(), fileSize);
            
            // Procesamiento del archivo
            std::cout << "  - Procesando datos..." << std::endl;
            auto processing_start = std::chrono::high_resolution_clock::now();
            const auto processedWords = processor(words);
            auto processing_end = std::chrono::high_resolution_clock::now();
            auto processing_ms = std::chrono::duration_cast<std::chrono::milliseconds>(processing_end-processing_start).count();
            std::cout << "  - Procesamiento completado. Tiempo: " << processing_ms << " ms" << std::endl;
            
            // Convertir words de vuelta a bytes
            std::cout << "  - Convirtiendo datos a bytes..." << std::endl;
            std::vector<char> processedData(processedWords.size() * sizeof(uint32_t));
            memcpy(processedData.data(), processedWords.data(), processedData.size());
            size_t processedSize = processedData.size();
            
            // Escritura del archivo
            std::cout << "  - Escribiendo archivo de salida..." << std::endl;
            auto write_start = std::chrono::high_resolution_clock::now();
            writeFile(outputPath, processedData);
            auto write_end = std::chrono::high_resolution_clock::now();
            auto write_ms = std::chrono::duration_cast<std::chrono::milliseconds>(write_end-write_start).count();
            std::cout << "  - Escritura completada. Tiempo: " << write_ms << " ms" << std::endl;
            
            // Mostrar resultados
            displayPerformanceResults(fileName, fileSize, processedSize, read_ms, processing_ms, write_ms);
            
        } catch (const std::exception& e) {
            std::cerr << "[Error] procesando " << filePath << ": " << e.what() << std::endl;
        }
    }
    
    std::cout << "\n=== PRUEBAS COMPLETADAS (" << filesPaths.size() << " archivos procesados) ===\n";
}

// Ejecuta la prueba de rendimiento con procesador de bytes
void runPerformanceTestWithByteProcessor(CipherByteProcessor processor, const std::string& inputDir, const std::string& outputDir) {
    const auto filesPaths = getFilesInDirectory(inputDir);
    
    if (!fs::exists(outputDir)) {
        std::cout << "[INFO] Creando directorio de salida: " << outputDir << std::endl;
        fs::create_directories(outputDir);
    }

    std::cout << "\n=== INICIANDO PRUEBAS DE RENDIMIENTO (BYTE PROCESSOR) ===\n";
    std::cout << "Archivos a procesar: " << filesPaths.size() << std::endl;
    
    for (size_t i = 0; i < filesPaths.size(); ++i) {
        const auto& filePath = filesPaths[i];
        try {
            std::cout << "\n[Procesando archivo " << (i+1) << "/" << filesPaths.size() << "]: " 
                      << fs::path(filePath).filename().string() << std::endl;
            
            auto total_start = std::chrono::high_resolution_clock::now();
            const auto fileName = fs::path(filePath).filename().string();            
            const auto outputPath = (fs::path(outputDir) / fileName).string();
            
            // Lectura del archivo
            std::cout << "  - Leyendo archivo..." << std::endl;
            auto read_start = std::chrono::high_resolution_clock::now();
            const auto fileData = readFile(filePath);
            auto read_end = std::chrono::high_resolution_clock::now();
            auto read_ms = std::chrono::duration_cast<std::chrono::milliseconds>(read_end-read_start).count();
            
            size_t fileSize = fileData.size();
            std::cout << "  - Lectura completada. Tamaño: " << formatBytes(fileSize) 
                      << ", tiempo: " << read_ms << " ms" << std::endl;
            
            // Procesamiento del archivo
            std::cout << "  - Procesando datos..." << std::endl;
            auto processing_start = std::chrono::high_resolution_clock::now();
            const auto processedData = processor(fileData);
            auto processing_end = std::chrono::high_resolution_clock::now();
            auto processing_ms = std::chrono::duration_cast<std::chrono::milliseconds>(processing_end-processing_start).count();
            std::cout << "  - Procesamiento completado. Tiempo: " << processing_ms << " ms" << std::endl;
            
            size_t processedSize = processedData.size();
            
            // Escritura del archivo
            std::cout << "  - Escribiendo archivo de salida..." << std::endl;
            auto write_start = std::chrono::high_resolution_clock::now();
            writeFile(outputPath, processedData);
            auto write_end = std::chrono::high_resolution_clock::now();
            auto write_ms = std::chrono::duration_cast<std::chrono::milliseconds>(write_end-write_start).count();
            std::cout << "  - Escritura completada. Tiempo: " << write_ms << " ms" << std::endl;
            
            // Mostrar resultados
            displayPerformanceResults(fileName, fileSize, processedSize, read_ms, processing_ms, write_ms);
            
        } catch (const std::exception& e) {
            std::cerr << "[Error] procesando " << filePath << ": " << e.what() << std::endl;
        }
    }
    
    std::cout << "\n=== PRUEBAS COMPLETADAS (" << filesPaths.size() << " archivos procesados) ===\n";
}

#endif