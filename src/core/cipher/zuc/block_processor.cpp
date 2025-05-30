#include <iostream>
#include <fstream>
#include <bitset>
#include <vector>
#include <filesystem>
#include <string>
#include "./zuc.cpp"

#ifndef ZUC_PROCESSOR
#define ZUC_PROCESSOR 

using namespace std; 

vector<char> zucblockProcessorByte(vector<char> buffer) {
    
    // TODO: decide the size of the block...
    Initialization(k,iv); 
    return stream_cipher_ZUC_BYTES(buffer, -1,-1); 
}


vector<uint32_t> zucblockProcessorWord(vector<uint32_t> buffer) {
    
    // TODO: decide the size of the block...
    Initialization(k,iv); 
    return stream_cipher_ZUC_WORD(buffer, -1,-1); 
}

#endif