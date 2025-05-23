
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


// Función para imprimir el uso de memoria
size_t printMemoryUsage() {
    #ifdef __linux__
        struct rusage usage;
        getrusage(RUSAGE_SELF, &usage);
        std::cout << "Memoria usada: " << usage.ru_maxrss << " KB\n";
    #elif _WIN32
        PROCESS_MEMORY_COUNTERS pmc;
        GetProcessMemoryInfo(GetCurrentProcess(), &pmc, sizeof(pmc));
        auto usedKB = pmc.WorkingSetSize / 1024; 
        std::cout << "Memoria usada: " << usedKB << " KB\n";
        return usedKB; 
    #endif
}

// Función para medir performance que acepta cualquier función sin parámetros
template<typename Func>
void measurePerformance(Func func) {
    std::cout << "=== Iniciando medida de performance ===" << std::endl;
    
    auto start = std::chrono::high_resolution_clock::now();
    std::cout << "Memoria antes: ";
    auto before = printMemoryUsage();
    
    // Ejecutar la función a medir
    func();
    
    std::cout << "Memoria al finalizar: ";
    auto after = printMemoryUsage();
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    std::cout << "Tiempo de ejecutado: " << duration.count() << " ms" << std::endl;
    std::cout << "Memoria adicional empleada " << after-before << " kb" << std::endl;
    std::cout << "=== Medida completada ===" << std::endl << std::endl;

}
#endif

// // Caso 1: Función simple sin parámetros
// measurePerformance(ejemploFuncionSimple);

// // Caso 2: Función con parámetros usando lambda
// measurePerformance([]() {
//     funcionConParametros("test", 500000);
// });

// // Caso 3: Bloque de código directo
// measurePerformance([]() {
//     std::cout << "Ejecutando bloque de código directo" << std::endl;
//     double result = 0;
//     std::vector<int> v = {}; 
//     for (int i = 0; i < 1000000; ++i) {
//         result += 1.0 / (i + 1);
//         v.push_back(i);
//     }
// });
    
