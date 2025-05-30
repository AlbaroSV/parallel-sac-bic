#include<iostream>
#include "../console.h"
#include "../../../core/command/encrypt_files/encrypt_files.cpp"
#include "../../../core/services/performance_analyzer.cpp"
#include "../../../core/cipher/chacha20/chacha20.cpp"
#include "../../../core/cipher/chacha20/block_processor.cpp"
#include "../../../core/shared/global.cpp"
#include "../../../core/tests/performance/performance.cpp"

#ifndef CHACHA20_CONSOLE_INTERFACE
#define CHACHA20_CONSOLE_INTERFACE


void console_interface_chacha20()
{
    std::cout << cleanConsole; 
    std::cout << ".// CONSOLE // chacha20" << std::endl; 
    std::cout << "Seleccione una de las siguientes acciones: " << std::endl; 
    std::cout << "1: Test BIC" << std::endl; 
    std::cout << "2: Test SAC" << std::endl;
    std::cout << "3: Performance" << std::endl;
    std::cout << "4: Exit and go back." << std::endl; 
    
    int input, n, m, l; 
    bool itSatisfiesTest; 
    std::cin >> input;
    
    string input_files, output_files; 
 
    switch (input){
        case 1:
            std::cout << cleanConsole; 
            std::cout << "Introduce n" << std::endl;  
            std::cin >> n; 
            std::cout << "Introduce m" << std::endl;  
            std::cin >> m; 
            std::cout << "Introduce l" << std::endl;  
            std::cin >> l;
            std::cout << "Procesando... " << std::endl;  
            chacha20_init();
            itSatisfiesTest = BIC_stream_cipher___Parallel(stream_cipher_CHACHA20,0.1,n,m,l,D);
            std::cout << "El cifrador " << (itSatisfiesTest ? "satisface" : "no satisface") << " BIC.";  
            break;
        case 2:
            std::cout << cleanConsole; 
            std::cout << "Introduce n" << std::endl;  
            std::cin >> n; 
            std::cout << "Introduce m" << std::endl;  
            std::cin >> m; 
            std::cout << "Introduce l" << std::endl;  
            std::cin >> l;
            std::cout << "Procesando... " << std::endl;  
            chacha20_init();
            itSatisfiesTest = SAC_stream_cipher___Parallel(stream_cipher_CHACHA20,0.1,n,m,l,D);
            std::cout << "El cifrador " << (itSatisfiesTest ? "satisface" : "no satisface") << " SAC.";  
            break;
        case 3:
            std::cout << cleanConsole; 
            std::cout << "Especifica la ruta donde se leeran los archivos" << std::endl;
            std::cin >> input_files;
            
            std::cout << "Especifica la ruta donde se escribiran los archivos cifrados" << std::endl;
            std::cin >> output_files;    
            runPerformanceTestWithByteProcessor(chacha20BlockProcessorByte, input_files, output_files); 
            break;
        case 4:
            console_run();
            break;
        
        default:
            std::cout << "Wrong choice, please try other." << std::endl;
            console_interface_chacha20();  
            break;
    }
}
#endif