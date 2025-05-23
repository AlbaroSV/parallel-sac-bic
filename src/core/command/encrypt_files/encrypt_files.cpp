#include <iostream>
#include <fstream>
#include <bitset>
#include <vector>
#include <filesystem>
#include <string>
#include "../../shared/global.cpp"
#include "../../services/performance_analyzer.cpp"

#ifndef ENCRYPT_FILES 
#define ENCRYPT_FILES 

using CipherFunction = std::function<std::bitset<MAX_SIZE>(std::bitset<MAX_SIZE>, int, int)>;

// Función para procesar un archivo completo
void processFileByBlocks(const std::string& inputPath, 
                        const std::string& outputPath,
                        CipherFunction stream_cipher_ZUC) {
    std::ifstream inputFile(inputPath, std::ios::binary | std::ios::ate);
    if (!inputFile) {
        throw std::runtime_error("No se pudo abrir el archivo: " + inputPath);
    }

    size_t fileSize = inputFile.tellg();
    size_t totalBits = fileSize * 8;
    inputFile.seekg(0, std::ios::beg);

    std::ofstream outputFile(outputPath, std::ios::binary);
    if (!outputFile) {
        throw std::runtime_error("No se pudo crear el archivo: " + outputPath);
    }

    size_t processedBits = 0;
    const size_t blockByteSize = MAX_SIZE / 8;
    std::vector<char> buffer(blockByteSize);

    while (processedBits < totalBits) {
        size_t bitsToProcess = std::min(static_cast<size_t>(MAX_SIZE), totalBits - processedBits);
        size_t bytesToRead = (bitsToProcess + 7) / 8;

        inputFile.read(buffer.data(), bytesToRead);
        if (inputFile.gcount() != static_cast<std::streamsize>(bytesToRead)) {
            throw std::runtime_error("Error al leer el archivo de entrada");
        }

        std::bitset<MAX_SIZE> blockBits;
        for (size_t i = 0; i < bytesToRead; ++i) {
            for (int j = 0; j < 8; ++j) {
                if (i * 8 + j >= bitsToProcess) break;
                blockBits[i * 8 + j] = (buffer[i] >> (7 - j)) & 1;
            }
        }

        std::bitset<MAX_SIZE> encryptedBlock = stream_cipher_ZUC(blockBits, bitsToProcess, 0);

        size_t bitsRemaining = bitsToProcess;
        size_t bytesToWrite = bitsRemaining / 8;
        size_t remainingBits = bitsRemaining % 8;

        for (size_t i = 0; i < bytesToWrite; ++i) {
            char byte = 0;
            for (int j = 0; j < 8; ++j) {
                if (encryptedBlock[i * 8 + j]) {
                    byte |= (1 << (7 - j));
                }
            }
            outputFile.write(&byte, 1);
        }

        if (remainingBits > 0) {
            char byte = 0;
            for (size_t j = 0; j < remainingBits; ++j) {
                if (encryptedBlock[bytesToWrite * 8 + j]) {
                    byte |= (1 << (7 - j));
                }
            }
            outputFile.write(&byte, 1);
        }

        processedBits += bitsToProcess;
    }

    inputFile.close();
    outputFile.close();
}

void processFiles(const std::string& inputDir, 
                 const std::string& outputDir,
                 CipherFunction cipherFunction) {
    namespace fs = std::filesystem;

    if (!fs::exists(outputDir)) {
        fs::create_directory(outputDir);
    }

    for (const auto& entry : fs::directory_iterator(inputDir)) {
        if (entry.is_regular_file()) {
            try {
                std::string outputPath = outputDir + "/" + entry.path().filename().string();
                processFileByBlocks(entry.path().string(), outputPath, cipherFunction);
                std::cout << "Procesado: " << entry.path() << " -> " << outputPath << " " << entry.file_size() / 1024 << " kb" << std::endl;
                
            } catch (const std::exception& e) {
                std::cerr << "Error procesando " << entry.path() << ": " << e.what() << std::endl;
            }
        }
    }
}
#endif  