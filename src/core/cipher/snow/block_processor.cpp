#include <iostream>
#include <fstream>
#include <bitset>
#include <vector>
#include <filesystem>
#include <string>
#include "./snow.cpp"


#ifndef SNOW_PROCESSOR 
#define SNOW_PROCESSOR
using namespace std; 

vector<char> snowBlockProcessorByte(vector<char> buffer) {
    // TODO: decide the size of the block...
    SNOW_V.keyiv_setup(snow_key, snow_iv, is_aead_mode);    
    return stream_cipher_SNOW3G_BYTES(buffer); 
}

#endif