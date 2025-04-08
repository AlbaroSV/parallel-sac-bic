#include <iostream>
#include "presentation/console/console.h"
#include "core/tests/test_setup.cpp"

#include <string>
using namespace std; 

const string PRESENTATION_CONFIG = "CONSOLE";

int main(){
    // For now just console interface, maybe in the future allow some open api
    if(PRESENTATION_CONFIG == "CONSOLE"){
        return console_run();
    }
    return 1; 
}