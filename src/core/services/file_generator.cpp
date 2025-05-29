#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <chrono>
#include <iomanip>

#ifndef FILE_GENERATOR
#define FILE_GENERATOR

// Función para crear un archivo del tamaño especificado
void createFile(const std::string& name, double size_gb) {
    // Convertir GB a bytes exactamente
    const size_t size = static_cast<size_t>(size_gb * 1024 * 1024 * 1024);
    const size_t buffer_size = 1024 * 1024; // 1MB buffer
    std::vector<char> buffer(buffer_size, 'A'); // Llenar el buffer con 'A's
    
    auto start = std::chrono::high_resolution_clock::now();
    
    std::cout << "Creando archivo " << name << " (" << size_gb << " GB)..." << std::endl;
    
    std::ofstream file(name, std::ios::binary);
    if (!file) {
        std::cerr << "Error al crear el archivo: " << name << std::endl;
        return;
    }
    
    size_t bytes_written = 0;
    while (bytes_written < size) {
        size_t write_now = std::min(buffer_size, size - bytes_written);
        file.write(buffer.data(), write_now);
        if (!file) {
            std::cerr << "Error al escribir en el archivo: " << name << std::endl;
            return;
        }
        bytes_written += write_now;
        
        // Mostrar progreso cada 100MB
        if (bytes_written % (100 * 1024 * 1024) == 0) {
            double progress = (static_cast<double>(bytes_written) / size) * 100;
            std::cout << "\r" << name << ": " << std::fixed << std::setprecision(1) 
                      << progress << "% completado (" 
                      << bytes_written / (1024 * 1024) << " MB de " 
                      << size / (1024 * 1024) << " MB)" << std::flush;
        }
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::seconds>(end - start);
    
    std::cout << "\nArchivo " << name << " creado exitosamente (" 
              << size_gb << " GB). Tiempo: " 
              << duration.count() << " segundos.\n" << std::endl;
}

#endif