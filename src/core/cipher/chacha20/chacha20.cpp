#include <iostream>
#include <algorithm>
#include <bitset>
#include <set>
#include <vector>
#include <chrono>
#include <random>
#include <thread>
#include <functional>
#include <numeric>
#include <string>
#include <cstdint>
#include <cstring>
#include "../../shared/global.cpp"

#ifndef CHACHA20_CIPHER
#define CHACHA20_CIPHER

bool DEBUG_CHACHA20 = false;

// Variables globales para la configuración de ChaCha20
uint8_t CHACHA20_KEY[32] = {
    0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
    0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
    0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
    0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f
};

uint8_t CHACHA20_NONCE[12] = {
    0x00, 0x00, 0x00, 0x09, 0x00, 0x00, 
    0x00, 0x4a, 0x00, 0x00, 0x00, 0x00
};

uint32_t CHACHA20_COUNTER = 0;

uint32_t state[16];

inline uint32_t rotl(uint32_t value, int shift) {
    return (value << shift) | (value >> (32 - shift));
}

inline void quarter_round(uint32_t &a, uint32_t &b, uint32_t &c, uint32_t &d) {
    a += b; d ^= a; d = rotl(d, 16);
    c += d; b ^= c; b = rotl(b, 12);
    a += b; d ^= a; d = rotl(d, 8);
    c += d; b ^= c; b = rotl(b, 7);
}

void chacha20_init() {
    // Constantes
    state[0] = 0x61707865; state[1] = 0x3320646e; 
    state[2] = 0x79622d32; state[3] = 0x6b206574;
    
    // Clave (256 bits)
    for (int i = 0; i < 8; ++i) {
        state[4 + i] = ((uint32_t)CHACHA20_KEY[i * 4 + 0] << 0) |
                      ((uint32_t)CHACHA20_KEY[i * 4 + 1] << 8) |
                      ((uint32_t)CHACHA20_KEY[i * 4 + 2] << 16) |
                      ((uint32_t)CHACHA20_KEY[i * 4 + 3] << 24);
    }
    
    // Contador
    state[12] = CHACHA20_COUNTER;
    
    // Nonce (96 bits)
    state[13] = ((uint32_t)CHACHA20_NONCE[0] << 0) | ((uint32_t)CHACHA20_NONCE[1] << 8) |
                ((uint32_t)CHACHA20_NONCE[2] << 16) | ((uint32_t)CHACHA20_NONCE[3] << 24);
    state[14] = ((uint32_t)CHACHA20_NONCE[4] << 0) | ((uint32_t)CHACHA20_NONCE[5] << 8) |
                ((uint32_t)CHACHA20_NONCE[6] << 16) | ((uint32_t)CHACHA20_NONCE[7] << 24);
    state[15] = ((uint32_t)CHACHA20_NONCE[8] << 0) | ((uint32_t)CHACHA20_NONCE[9] << 8) |
                ((uint32_t)CHACHA20_NONCE[10] << 16) | ((uint32_t)CHACHA20_NONCE[11] << 24);
}

void chacha20_block(uint32_t output[16], const uint32_t input[16]) {
    std::memcpy(output, input, sizeof(uint32_t) * 16);
    for (int i = 0; i < 10; ++i) {
        // Odd round
        quarter_round(output[0], output[4], output[8], output[12]);
        quarter_round(output[1], output[5], output[9], output[13]);
        quarter_round(output[2], output[6], output[10], output[14]);
        quarter_round(output[3], output[7], output[11], output[15]);
        // Even round
        quarter_round(output[0], output[5], output[10], output[15]);
        quarter_round(output[1], output[6], output[11], output[12]);
        quarter_round(output[2], output[7], output[8], output[13]);
        quarter_round(output[3], output[4], output[9], output[14]);
    }
    for (int i = 0; i < 16; ++i) {
        output[i] += input[i];
    }
}

void serialize_state_from_32(uint8_t block[64], const uint32_t state[16]) {
    for (int i = 0; i < 16; ++i) {
        block[i * 4 + 0] = (state[i] >> 0) & 0xff;
        block[i * 4 + 1] = (state[i] >> 8) & 0xff;
        block[i * 4 + 2] = (state[i] >> 16) & 0xff;
        block[i * 4 + 3] = (state[i] >> 24) & 0xff;
    }
}

std::bitset<MAX_SIZE> stream_cipher_CHACHA20(std::bitset<MAX_SIZE> input, int n, int m) {
    if (n > MAX_SIZE) {
        throw std::runtime_error("Input size exceeds MAX_SIZE");
    }

    std::bitset<MAX_SIZE> output;

    // Convert bitset to byte array
    std::vector<uint8_t> input_bytes((n + 7) / 8);
    for (int i = 0; i < n; ++i)
        input_bytes[i / 8] |= input[i] << (i % 8);

    std::vector<uint8_t> keystream(input_bytes.size());

    // Procesar en bloques de 64 bytes (512 bits)
    for (size_t offset = 0; offset < input_bytes.size(); offset += 64) {
        uint32_t block[16];
        chacha20_block(block, state);

        uint8_t bytesResult[64] = {}; 

        if(DEBUG_CHACHA20){
            std::cout << "Block before serialize:\n";
            for (size_t i = 0; i < 16; ++i) {
                printf("%08x ", block[i]);
                if ((i + 1) % 4 == 0) std::cout << std::endl;
            }
            std::cout << std::endl;
        }

        serialize_state_from_32(bytesResult, block); 
        std::memcpy(keystream.data() + offset, bytesResult, std::min(64ULL, keystream.size() - offset));
        
        // Incrementar el contador para el próximo bloque
        state[12]++;
    }

    if(DEBUG_CHACHA20){
        std::cout << "Serialized keystream:\n";
        for (size_t i = 0; i < 64; ++i) {
            printf("%02x", keystream[i]);
            if ((i + 1) % 4 == 0) std::cout << " ";
            if ((i + 1) % 16 == 0) std::cout << std::endl;
        }
        std::cout << std::endl;
    }
    // XOR entre input y keystream
    for (size_t i = 0; i < input_bytes.size(); ++i) {
        input_bytes[i] ^= keystream[i];
    }

    // Convertir de vuelta a bitset
    for (int i = 0; i < n; ++i) {
        output[i] = (input_bytes[i / 8] >> (i % 8)) & 1;
    }

    return output;
}

std::vector<char> stream_cipher_CHACHA20_BYTE(const std::vector<char>& input) {
    std::vector<char> output;
    
    if (input.empty()) {
        return output;
    }

    // Convertir vector<char> a vector<uint8_t> para trabajar con bytes
    std::vector<uint8_t> input_bytes(input.begin(), input.end());
    std::vector<uint8_t> keystream(input_bytes.size());

    // Procesar en bloques de 64 bytes (512 bits)
    for (size_t offset = 0; offset < input_bytes.size(); offset += 64) {
        uint32_t block[16];
        chacha20_block(block, state);

        uint8_t bytesResult[64] = {}; 

        if(DEBUG_CHACHA20) {
            std::cout << "Block before serialize:\n";
            for (size_t i = 0; i < 16; ++i) {
                printf("%08x ", block[i]);
                if ((i + 1) % 4 == 0) std::cout << std::endl;
            }
            std::cout << std::endl;
        }

        serialize_state_from_32(bytesResult, block); 
        std::memcpy(keystream.data() + offset, bytesResult, 
                   std::min(static_cast<size_t>(64), keystream.size() - offset));
        
        // Incrementar el contador para el próximo bloque
        state[12]++;
    }

    if(DEBUG_CHACHA20) {
        std::cout << "Serialized keystream:\n";
        for (size_t i = 0; i < std::min(static_cast<size_t>(64), keystream.size()); ++i) {
            printf("%02x", keystream[i]);
            if ((i + 1) % 4 == 0) std::cout << " ";
            if ((i + 1) % 16 == 0) std::cout << std::endl;
        }
        std::cout << std::endl;
    }

    // XOR entre input y keystream
    for (size_t i = 0; i < input_bytes.size(); ++i) {
        input_bytes[i] ^= keystream[i];
    }

    // Convertir de vuelta a vector<char>
    output.assign(input_bytes.begin(), input_bytes.end());

    return output;
}

// Función para configurar la clave, nonce y contador
void chacha20_configure(const uint8_t key[32], const uint8_t nonce[12], uint32_t counter = 0) {
    std::memcpy(CHACHA20_KEY, key, 32);
    std::memcpy(CHACHA20_NONCE, nonce, 12);
    CHACHA20_COUNTER = counter;
}

#endif