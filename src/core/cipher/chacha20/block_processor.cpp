#include <iostream>
#include <fstream>
#include <bitset>
#include <vector>
#include <filesystem>
#include <string>
#include "./chacha20.cpp"


#ifndef CHACHA20_PROCESSOR 
#define CHACHA20_PROCESSOR
using namespace std; 

vector<char> chacha20BlockProcessorByte(vector<char> buffer) {
    // TODO: decide the size of the block...
    chacha20_init();    
    return stream_cipher_CHACHA20_BYTE(buffer); 
}

#endif