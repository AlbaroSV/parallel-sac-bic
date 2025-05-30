#include <iostream>
#include <fstream>
#include <bitset>
#include <vector>
#include <filesystem>
#include <string>
#include "./trivium.cpp"


#ifndef TRIVIUM_PROCESSOR 
#define TRIVIUM_PROCESSOR
using namespace std; 

vector<char> triviumBlockProcessorByte(vector<char> buffer) {
    // TODO: decide the size of the block...
    trivium_init(trivium_key, 80, trivium_iv, 80, &ctx);
    return stream_cipher_TRIVIUM_BYTES(buffer, buffer.size()); 
}

#endif