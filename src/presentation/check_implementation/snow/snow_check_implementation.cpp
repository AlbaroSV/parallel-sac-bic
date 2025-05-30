#include <iostream>
#include <iomanip>
#include <cstdint>
#include <cstring>
#include <string>
#include "../../../core/cipher/snow/snow.cpp"

#ifndef CHECK_IMPLEMENTATION_SNOW
#define CHECK_IMPLEMENTATION_SNOW
#include <iostream>
#include <iomanip>
#include <vector>
#include <bitset>
#include <cassert>

int check_implementation_SNOW() {
    // Enable debug mode
    snow_is_debug_mode = 1;

    // Test with all-zero inputs
    cout << "=== SNOW 3G Debug Test ===" << endl;
    cout << "Using all-zero key and IV with all-zero input" << endl;
    
    bitset<MAX_SIZE> input;
    bitset<MAX_SIZE> output = stream_cipher_SNOW3G(input, 256, 0);
    
    // Print final output
    if (snow_is_debug_mode) {
        cout << "\nFinal Output (first 256 bits):" << endl;
        for (int i = 0; i < 32; i++) {
            u8 byte = 0;
            for (int j = 0; j < 8; j++) {
                if (output[i*8 + j]) byte |= (1 << j);
            }
            printf("%02x ", byte);
            if ((i+1) % 16 == 0) cout << endl;
        }
    }
    
    return 0;
}

#endif