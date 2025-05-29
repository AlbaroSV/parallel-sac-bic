#ifndef PERFORMANCE_ANALYZER
#define PERFORMANCE_ANALYZER

#include <chrono>
#include <iostream>
#include <functional>
#include <fstream>
#ifdef __linux__
#include <sys/resource.h>
#elif _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#undef byte
#include <psapi.h>
#endif

// Estructura para almacenar los resultados de performance
// Modificación en la estructura PerformanceResult
struct PerformanceResult {
    long long time_ns;       // Tiempo en nanosegundos
    size_t memory_before_kb;
    size_t memory_after_kb;
    size_t additional_memory_kb;
    
    // Método que muestra todas las unidades de tiempo relevantes
    void print() const {
        std::cout << "=== Resultados de Performance ===" << std::endl;
        std::cout << "Tiempo de ejecución: " << time_ns << " ns";
        std::cout << " (" << time_ns/1000 << " µs";
        std::cout << " ~ " << time_ns/1000000 << " ms";
        std::cout << " ~ " << time_ns/1000000000 << " s)" << std::endl;
        std::cout << "Memoria antes: " << memory_before_kb << " KB" << std::endl;
        std::cout << "Memoria después: " << memory_after_kb << " KB" << std::endl;
        std::cout << "Memoria adicional: " << additional_memory_kb << " KB" << std::endl;
    }
};


void displayMemoryInfo() {
    MEMORYSTATUSEX memInfo;
    memInfo.dwLength = sizeof(MEMORYSTATUSEX);
    GlobalMemoryStatusEx(&memInfo);

    std::cout << "Memoria física total: " << memInfo.ullTotalPhys / (1024 * 1024) << " MB\n";
    std::cout << "Memoria física disponible: " << memInfo.ullAvailPhys / (1024 * 1024) << " MB\n";
    std::cout << "Memoria virtual total: " << memInfo.ullTotalVirtual / (1024 * 1024) << " MB\n";
    std::cout << "Memoria virtual disponible: " << memInfo.ullAvailVirtual / (1024 * 1024) << " MB\n";
}

// Función para obtener el uso de memoria
size_t getMemoryUsage() {
    #ifdef __linux__
        struct rusage usage;
        getrusage(RUSAGE_SELF, &usage);
        return usage.ru_maxrss;
    #elif _WIN32
        PROCESS_MEMORY_COUNTERS pmc;
        GetProcessMemoryInfo(GetCurrentProcess(), &pmc, sizeof(pmc));
        return pmc.WorkingSetSize / 1024;
    #endif
}

// Modificación en la función measurePerformance
template<typename Func>
PerformanceResult measurePerformance(Func func) {
    PerformanceResult result;
    
    auto start = std::chrono::high_resolution_clock::now();
    result.memory_before_kb = getMemoryUsage();
    
    func(); // Ejecutamos la función a medir
    
    result.memory_after_kb = getMemoryUsage();
    auto end = std::chrono::high_resolution_clock::now();
    
    // Calculamos en nanosegundos
    result.time_ns = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
    result.additional_memory_kb = result.memory_after_kb - result.memory_before_kb;
    
    return result;
}
#endif