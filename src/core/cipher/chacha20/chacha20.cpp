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

uint32_t initialState[16] = {
    0x61707865, 0x3320646e, 0x79622d32, 0x6b206574,
    0x03020100, 0x07060504, 0x0b0a0908, 0x0f0e0d0c,
    0x13121110, 0x17161514, 0x1b1a1918, 0x1f1e1d1c,
    0x00000001, 0x09000000, 0x4a000000, 0x00000000
};

inline uint32_t rotl(uint32_t value, int shift) {
    return (value << shift) | (value >> (32 - shift));
}

inline void quarter_round(uint32_t &a, uint32_t &b, uint32_t &c, uint32_t &d) {
    a += b; d ^= a; d = rotl(d, 16);
    c += d; b ^= c; b = rotl(b, 12);
    a += b; d ^= a; d = rotl(d, 8);
    c += d; b ^= c; b = rotl(b, 7);
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
        chacha20_block(block, initialState);

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
#endif
