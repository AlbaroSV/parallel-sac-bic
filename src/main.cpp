#include <iostream>
#include "presentation/console/console.h"
#include "core/tests/test_setup.cpp"
#include "presentation/console/zuc/zuc_console_interface.cpp"

#include <string>
using namespace std; 

const string PRESENTATION_CONFIG = "CONSOLE";

int main(){
    // For now just console interface, maybe in the future allow some open api
    // if(PRESENTATION_CONFIG == "CONSOLE"){
    //     return console_run();
    // }

    console_interface_zuc(); 

    
    return 1; 
}