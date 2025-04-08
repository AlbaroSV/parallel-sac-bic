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

#ifndef test_cipher_h
#define test_cipher_h

// ChaCha20 constants
const uint32_t CHACHA_CONSTANTS[4] = {
    0x61707865, // "expa"
    0x3320646e, // "nd 3"
    0x79622d32, // "2-by"
    0x6b206574  // "te k"
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

std::bitset<MAX_SIZE> stream_cipher_CHACHA20(std::bitset<MAX_SIZE> input, int n, int m) {
    std::bitset<MAX_SIZE> output;

    // Convert bitset to byte array
    std::vector<uint8_t> input_bytes(n / 8);
    for (int i = 0; i < n; ++i)
        input_bytes[i / 8] |= input[i] << (i % 8);

    // Set up state (key, counter, nonce)
    uint32_t state[16];
    std::memcpy(state, CHACHA_CONSTANTS, sizeof(CHACHA_CONSTANTS));

    uint32_t key[8] = {0}; // 256-bit key (zeros by default)
    std::memcpy(&state[4], key, sizeof(key));

    state[12] = 0; // Counter
    state[13] = 0; // Nonce part 1
    state[14] = 0; // Nonce part 2
    state[15] = 0; // Nonce part 3

    std::vector<uint8_t> keystream(input_bytes.size());
    for (size_t i = 0; i < input_bytes.size(); i += 64) {
        uint32_t block[16];
        chacha20_block(block, state);

        for (size_t j = 0; j < 64 && i + j < input_bytes.size(); ++j) {
            keystream[i + j] = reinterpret_cast<uint8_t*>(block)[j];
        }

        state[12]++; // Increment counter
    }

    // XOR input with keystream
    for (size_t i = 0; i < input_bytes.size(); ++i) {
        input_bytes[i] ^= keystream[i];
    }

    // Convert back to bitset
    for (int i = 0; i < n; ++i) {
        output[i] = (input_bytes[i / 8] >> (i % 8)) & 1;
    }

    return output;
}
#endif
