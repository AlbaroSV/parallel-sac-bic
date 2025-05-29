#include <iostream>
#include <fstream>
#include <bitset>
#include <vector>
#include <filesystem>
#include <string>
#include "./zuc.cpp"

using namespace std; 

vector<char> blockProcessorByte(vector<char> buffer) {
    
    // TODO: decide the size of the block...
    Initialization(k,iv); 
    return stream_cipher_ZUC_BYTES(buffer, -1,-1); 
}


vector<uint32_t> blockProcessorWord(vector<uint32_t> buffer) {
    
    // TODO: decide the size of the block...
    Initialization(k,iv); 
    return stream_cipher_ZUC_WORD(buffer, -1,-1); 
}