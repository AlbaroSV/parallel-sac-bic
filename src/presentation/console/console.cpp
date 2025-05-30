#include<iostream>
#include "../../presentation/console/zuc/zuc_console_interface.cpp"
#include "../../presentation/console/snow/snow_console_interface.cpp"
#include "../../presentation/console/trivium/trivium_console_interface.cpp"
#include "../../presentation/console/chacha20/chacha20_console_interface.cpp"

#include "../../core/shared/global.cpp"


#ifndef CONSOLE_APP
#define CONSOLE_APP

int console_run(){
    int mustContinue = 1; 
    while(mustContinue)
    {
        std::cout << cleanConsole; 
        std::cout << ".// CONSOLE \n"; 
        std::cout << "Console application running \n"; 

        std::cout << "1: CHACHA20" << std::endl; 
        std::cout << "2: SNOW" << std::endl; 
        std::cout << "3: TRIVIUM" << std::endl; 
        std::cout << "4: ZUC" << std::endl; 
        
        int option; 
        std::cin >> option;

        switch (option)
        {
            case 1:
                mustContinue = console_interface_chacha20(); 
            break;
            case 2:
                mustContinue = console_interface_snow(); 
            break;
            case 3:
                mustContinue = console_interface_trivium(); 
            break;
            case 4:
                mustContinue = console_interface_zuc(); 
            break;
            default:
                console_run(); 
            break;
        }
    }
    return 0;
}

#endif