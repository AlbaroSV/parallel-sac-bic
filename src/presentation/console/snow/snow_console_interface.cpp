#include<iostream>
#include "../console.h"
#include "../../../core/command/encrypt_files/encrypt_files.cpp"
#include "../../../core/services/performance_analyzer.cpp"
#include "../../../core/cipher/snow/snow.cpp"
#include "../../../core/cipher/snow/block_processor.cpp"
#include "../../../core/shared/global.cpp"
#include "../../../core/tests/performance/performance.cpp"

#ifndef SNOW_CONSOLE_INTERFACE
#define SNOW_CONSOLE_INTERFACE


void console_interface_snow()
{
    std::cout << cleanConsole; 
    std::cout << ".// CONSOLE // SNOW" << std::endl; 
    std::cout << "Seleccione una de las siguientes acciones: " << std::endl; 
    std::cout << "1: Test BIC" << std::endl; 
    std::cout << "2: Test SAC" << std::endl;
    std::cout << "3: Performance" << std::endl;
    std::cout << "4: Exit and go back." << std::endl; 
    
    int input, n, m, l; 
    bool itSatisfiesBIC; 
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
            SNOW_V.keyiv_setup(snow_key, snow_iv, is_aead_mode);    
            itSatisfiesBIC = BIC_stream_cipher___Parallel(stream_cipher_SNOW3G,0.1,n,m,l,D);
            std::cout << "El cifrador " << (itSatisfiesBIC ? "satisface" : "no satisface") << " BIC.";  
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
            SNOW_V.keyiv_setup(snow_key, snow_iv, is_aead_mode);    
            itSatisfiesBIC = SAC_stream_cipher___Parallel(stream_cipher_SNOW3G,0.1,n,m,l,D);
            std::cout << "El cifrador " << (itSatisfiesBIC ? "satisface" : "no satisface") << " SAC.";  
            break;
        case 3:
            std::cout << cleanConsole; 
            std::cout << "Especifica la ruta donde se leeran los archivos" << std::endl;
            std::cin >> input_files;
            
            std::cout << "Especifica la ruta donde se escribiran los archivos cifrados" << std::endl;
            std::cin >> output_files;    
            runPerformanceTestWithByteProcessor(snowBlockProcessorByte, input_files, output_files); 
            break;
        case 4:
            console_run();
            break;
        
        default:
            std::cout << "Wrong choice, please try other." << std::endl;
            console_interface_snow();  
            break;
    }
}
#endif