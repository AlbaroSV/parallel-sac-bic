#include <iostream>
#include <iomanip>
#include <cstdint>
#include <cstring>
#include <string>
#include "../../../core/cipher/chacha20/chacha20.cpp"

#ifndef CHECK_IMPLEMENTATION_T
#define CHECK_IMPLEMENTATION_T

int check_implementation_CHACHA20() {
    // Based on RFC8439 section 2.4.2 
    initialState[0]  = 0x61707865;
    initialState[1]  = 0x3320646e;
    initialState[2]  = 0x79622d32;
    initialState[3]  = 0x6b206574;
    initialState[4]  = 0x03020100;
    initialState[5]  = 0x07060504;
    initialState[6]  = 0x0b0a0908;
    initialState[7]  = 0x0f0e0d0c;
    initialState[8]  = 0x13121110;
    initialState[9]  = 0x17161514;
    initialState[10] = 0x1b1a1918;
    initialState[11] = 0x1f1e1d1c;
    initialState[12] = 0x00000001;
    initialState[13] = 0x09000000;
    initialState[14] = 0x4a000000;
    initialState[15] = 0x00000000;  
    DEBUG_CHACHA20   = true; 

    char input[115] = "Ladies and Gentlemen of the class of '99: If I could offer you only one tip for the future, sunscreen would be it."; 
    // convert input to bitset
    std::bitset<MAX_SIZE> input_bits;
    for (int i = 0; i < 115; ++i) {
        uint8_t byte = static_cast<uint8_t>(input[i]);
        for (int b = 0; b < 8; ++b) {
            bool bit = (byte >> b) & 1;
            input_bits[i * 8 + b] = bit;
        }
    }
    auto s = stream_cipher_CHACHA20(input_bits, 115*8, -1); 

    // convert to string interpreted char and print 
    std::string output;
    for (int i = 0; i < 115; ++i) {
        uint8_t byte = 0;
        for (int b = 0; b < 8; ++b) {
            if (s[i * 8 + b])
                byte |= (1U << b);
        }
        output += static_cast<char>(byte);
    }
    std::cout << "Output: " << output << std::endl;
    return 0;
}

#endif