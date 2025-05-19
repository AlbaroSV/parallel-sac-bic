#include <iostream>
#include <bitset>
#include <cassert>
#include "../../../core/cipher/zuc/zuc.cpp"


// Función para convertir hex string a bitset
template<size_t N>
std::bitset<N> hex_to_bitset(const std::string& hex) {
    std::bitset<N> bits;
    for (size_t i = 0; i < hex.size(); ++i) {
        char c = hex[i];
        int value;
        if (c >= '0' && c <= '9') value = c - '0';
        else if (c >= 'A' && c <= 'F') value = 10 + c - 'A';
        else if (c >= 'a' && c <= 'f') value = 10 + c - 'a';
        else continue;
        
        for (int j = 0; j < 4; ++j) {
            bits.set(4*i + (3-j), (value >> j) & 1);
        }
    }
    return bits;
}

// Función para imprimir bitset como hex
template<size_t N>
void print_hex(const std::bitset<N>& bits) {
    for (size_t i = 0; i < N; i += 4) {
        int value = 0;
        for (int j = 0; j < 4 && (i+j) < N; ++j) {
            value |= bits.test(i+j) << (3-j);
        }
        std::cout << std::hex << value;
    }
    std::cout << std::dec << std::endl;
}

void check_implementation_zuc() {
    std::cout << "\n=== Testing ZUC-128 ===" << std::endl;
    
    // Test Vector 1 from ZUC specification
    {
        std::cout << "\nTest Vector 1:" << std::endl;
        
        // Key and IV from specification
        std::string key_hex = "00000000000000000000000000000000";
        std::string iv_hex = "00000000000000000000000000000000";
        
        std::bitset<128> key = hex_to_bitset<128>(key_hex);
        std::bitset<128> iv = hex_to_bitset<128>(iv_hex);
        
        // Expected keystream (first 32 bits)
        std::string expected_hex = "27bede74";
        
        // Initialize with key and IV
        // (Note: En la implementación actual, key e IV están hardcodeados a 0)
        // En una implementación completa, deberías modificar Initialization() para aceptar estos parámetros
        
        // Generate keystream
        std::bitset<32> input; // 32-bit input (all zeros)
        std::bitset<32> output = stream_cipher_ZUC(input, 0, 0);
        
        std::cout << "Expected: " << expected_hex << std::endl;
        std::cout << "Actual:   ";
        print_hex(output);
        
        // Verify
        std::bitset<32> expected = hex_to_bitset<32>(expected_hex);
        assert(output == expected);
        std::cout << "Test 1 passed!" << std::endl;
    }
}
